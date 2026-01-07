/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>

#include "stm32spi.h"
#include "qemudevice.h"
#include "iopin.h"
#include "simulator.h"

Stm32Spi::Stm32Spi( QemuDevice* mcu, QString name, int number )
        : QemuSpi( mcu, name, number )
{
    m_prescList = {2,4,8,16,32,64,128,256};
}
Stm32Spi::~Stm32Spi(){}

void Stm32Spi::doAction()
{
    //uint8_t  action = m_arena->data8;

    //qDebug() << "Stm32Spi::doAction" << action;
    //switch( action )
    //{
    //    case STM32_SPI_CR1:
    //    {
    //        uint16_t newCR1 = m_arena->data32;

    //        bool clkPha = newCR1 & 1<<0; // Bit 0  CPHA:     Clock phase

    //        bool clkPol = newCR1 & 1<<1; // Bit 1  CPOL:     Clock polarity
    //        m_leadEdge = clkPol ? Clock_Falling : Clock_Rising;
    //        m_tailEdge = clkPol ? Clock_Rising  : Clock_Falling;
    //        m_sampleEdge = ( clkPol == clkPha ) ? Clock_Rising : Clock_Falling;
    //        m_clkPin->setOutState( clkPol );
    //        updateClock();

    //        bool master = newCR1 & 1<<2; // Bit 2  MSTR:     Master selection: 1 = Master
    //        spiMode_t mode = master ? SPI_MASTER : SPI_SLAVE;
    //        setMode( mode );

    //        uint8_t spr = (newCR1 & 0b00111000) >> 3; // Bit 3-5 BR: Baud rate control: 2,4,8,16,32,64,128,256
    //        m_prescaler = m_prescList[spr];
    //        m_clockPeriod = m_arena->ps_per_inst*m_prescaler;

    //        m_enabled =  newCR1 & 1<<6; // Bit 6  SPE:      SPI enable
    //        if( !m_enabled ) setMode( SPI_OFF );  // Disable SPI


    //        m_lsbFirst = newCR1 & 1<<7; // Bit 7  LSBFIRST: Frame format: 1 = LSB first

    //        // Bit 8  SSI:      Internal slave select
    //        // Bit 9  SSM:      Software slave management
    //        // Bit 10 RXONLY:   Receive only
    //        // Bit 11 DFF:      Data frame format: 1 = 16 bit
    //        // Bit 12 CRCNEXT:  CRC transfer next
    //        // Bit 13 CRCEN:    Hardware CRC calculation enable
    //        // Bit 14 BIDIOE:   Output enable in bidirectional mode
    //        // Bit 15 BIDIMODE: Bidirectional data mode enable
    //    }break;
    //    case STM32_SPI_CR2:
    //    {
    //        uint16_t newCR2 = m_arena->data32;

    //        // Bit 0 RXDMAEN: Rx buffer DMA enable
    //        // Bit 1 TXDMAEN: Tx buffer DMA enable
    //        // Bit 2 SSOE:    SS output enable
    //        // Bits 4:3       Reserved
    //        // Bit 5 ERRIE:   Error interrupt enable
    //        // Bit 6 RXNEIE:  RX buffer not empty interrupt enable
    //        // Bit 7 TXEIE:   Tx buffer empty interrupt enable
    //    }break;
    //    case  STM32_SPI_READ:
    //    {
    //        m_arena->data16 = m_dataReg;
    //    }break;
    //    case  STM32_SPI_WRITE:
    //    {
    //        uint16_t newDR = m_arena->data32;
    //        m_srReg = newDR;
    //        if( m_mode == SPI_MASTER ) StartTransaction();
    //    }break;
    //}
}

void Stm32Spi::endTransaction()
{
    SpiModule::endTransaction();
    m_dataReg = m_srReg;
    //m_interrupt->raise();
}
