/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>

#include "stm32spi.h"
#include "qemudevice.h"
#include "iopin.h"
#include "simulator.h"

#define CR1_OFFSET     0x00
#define CR2_OFFSET     0x04
#define SR_OFFSET      0x08
#define DR_OFFSET      0x0C
#define CRCPR_OFFSET   0x10
#define RXCRCR_OFFSET  0x14
#define TXCRCR_OFFSET  0x18
#define I2SCFGR_OFFSET 0x1C
#define I2SPR_OFFSET   0x20

#define R_SR_MASK       0x01FF
#define R_SR_BUSY      (1 << 7)
#define R_SR_OVR       (1 << 6)
#define R_SR_TXE       (1 << 1)
#define R_SR_RXNE      (1 << 0)

Stm32Spi::Stm32Spi( QemuDevice* mcu, QString name, int n, uint32_t* clk, uint64_t memStart, uint64_t memEnd )
        : QemuSpi( mcu, name, n, clk, memStart, memEnd )
{
    m_prescList = {2,4,8,16,32,64,128,256};
}
Stm32Spi::~Stm32Spi(){}

void Stm32Spi::reset()
{
    m_status = R_SR_TXE;

    m_CR1 = 0;
    m_CR2 = 0;
}

void Stm32Spi::writeRegister()
{
    uint64_t offset = m_eventAddress - m_memStart;
    //qDebug() << "Stm32Spi::writeRegister" << offset << m_eventValue;
    switch( offset ) {
        case CR1_OFFSET:     writeCR1();     break;
        case CR2_OFFSET:     writeCR2();     break;
        case SR_OFFSET :     /*Read Only*/   break;
        case DR_OFFSET:      writeDR();      break;
        case CRCPR_OFFSET:                  // Fall through
        case RXCRCR_OFFSET:                 // Fall through
        case TXCRCR_OFFSET:                 // Fall through
        case I2SCFGR_OFFSET:                // Fall through
        case I2SPR_OFFSET:                  // Fall through
        default:             write();
    }
}

void Stm32Spi::readRegister()
{
    uint64_t offset = m_eventAddress - m_memStart;
    uint64_t value = 0;

    switch( offset ) {
        case CR1_OFFSET:     value = m_CR1;      break;
        case CR2_OFFSET:     value = m_CR2;      break;
        case SR_OFFSET :     value = m_status;   break;
        case DR_OFFSET:      value = m_dataReg;  break;
        case CRCPR_OFFSET:                       // Fall through
        case RXCRCR_OFFSET:                      // Fall through
        case TXCRCR_OFFSET:                      // Fall through
        case I2SCFGR_OFFSET:                     // Fall through
        case I2SPR_OFFSET:                       // Fall through
        default:             value = read();
    }
    //qDebug() << "Stm32Spi::readRegister" << offset << value;
    m_arena->regData = value;
    m_arena->qemuAction = SIM_READ;
}

void Stm32Spi::writeCR1()
{
    uint16_t newCR1 = m_eventValue;

    if( m_CR1 == newCR1) return;

    bool clkPha = newCR1 & 1<<0; // Bit 0  CPHA:     Clock phase

    bool clkPol = newCR1 & 1<<1; // Bit 1  CPOL:     Clock polarity
    m_leadEdge  = clkPol ? Clock_Falling : Clock_Rising;
    m_tailEdge  = clkPol ? Clock_Rising  : Clock_Falling;
    m_sampleEdge = ( clkPol == clkPha ) ? Clock_Rising : Clock_Falling;

    bool master = newCR1 & 1<<2; // Bit 2  MSTR:     Master selection: 1 = Master
    spiMode_t mode = master ? SPI_MASTER : SPI_SLAVE;

    uint8_t spr = (newCR1 & 0b00111000) >> 3; // Bit 3-5 BR: Baud rate control: 2,4,8,16,32,64,128,256
    m_prescaler = m_prescList[spr];
    m_clockPeriod = m_arena->ps_per_inst*m_prescaler;

    m_enabled =  newCR1 & 1<<6; // Bit 6  SPE:      SPI enable
    if( !m_enabled ) setMode( SPI_OFF );  // Disable SPI
    else {
        setMode( mode );
        if( mode == SPI_MASTER)
        {
            if( m_clkPin ){
                m_clkPin->setOutState( clkPol );
                updateClock();
            }
        }

    }

    m_lsbFirst = newCR1 & 1<<7; // Bit 7  LSBFIRST: Frame format: 1 = LSB first

    // Bit 8  SSI:      Internal slave select
    // Bit 9  SSM:      Software slave management
    // Bit 10 RXONLY:   Receive only
    // Bit 11 DFF:      Data frame format: 1 = 16 bit
    // Bit 12 CRCNEXT:  CRC transfer next
    // Bit 13 CRCEN:    Hardware CRC calculation enable
    // Bit 14 BIDIOE:   Output enable in bidirectional mode
    // Bit 15 BIDIMODE: Bidirectional data mode enable

    m_CR1 = newCR1;
}

void Stm32Spi::writeCR2()
{
    m_CR2 = m_eventValue;

    // Bit 0 RXDMAEN: Rx buffer DMA enable
    // Bit 1 TXDMAEN: Tx buffer DMA enable
    // Bit 2 SSOE:    SS output enable
    // Bits 4:3       Reserved
    // Bit 5 ERRIE:   Error interrupt enable
    // Bit 6 RXNEIE:  RX buffer not empty interrupt enable
    // Bit 7 TXEIE:   Tx buffer empty interrupt enable
}

void Stm32Spi::writeDR()
{
    m_status |=  R_SR_BUSY;
    m_status &= ~R_SR_TXE;
    //if( m_status & R_SR_RXNE) m_status |= R_SR_OVR;

    //uint8_t data = m_eventValue;
    //if ( m_CR1 & R_CR1_LSBFIRST ) data = bitswap(ssi_transfer(s->spi, bitswap(data)));
    //else                         data = ssi_transfer(s->spi, data);

    //qDebug() << "Stm32Spi::writeDR" << m_eventValue;
    // SEND DATA
    m_srReg = m_eventValue;
    StartTransaction();
}

void Stm32Spi::endTransaction()
{
    SpiModule::endTransaction();
    m_dataReg = m_srReg;

    m_status |= R_SR_RXNE | R_SR_TXE;
    m_status &= ~R_SR_BUSY;
    //qDebug() << "Stm32Spi::endTransaction";
    //m_interrupt->raise();
}
