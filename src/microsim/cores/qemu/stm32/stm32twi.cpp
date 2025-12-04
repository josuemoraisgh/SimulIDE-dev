/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>

#include "stm32twi.h"
#include "qemudevice.h"
#include "iopin.h"
#include "simulator.h"

Stm32Twi::Stm32Twi( QemuDevice* mcu, QString name, int number )
       : QemuTwi( mcu, name, number )
{
}
Stm32Twi::~Stm32Twi(){}

void Stm32Twi::reset()
{
}

void Stm32Twi::doAction()
{
    uint16_t  data = m_arena->data32;
    uint8_t action = m_arena->data8;

    switch( action ) {
    case QEMU_I2C_FREQ: setFreqKHz( data*1000); break; /// FIXME: check that freq is sent in MHz
    case STM32_TWI_CR1: writeCR1( data ); break;
    case STM32_TWI_CR2: writeCR2( data ); break;

    default: break;
    }

    //if( m_mode != TWI_MASTER )
    //{
    //    setFreqKHz( 100 );
    //    setMode( TWI_MASTER );
    //    m_scl->setPinMode( openCo );
    //    m_sda->setPinMode( openCo );
    //}
    //i2cPending_t* newAction = new i2cPending_t{ action, data, nullptr };

    //if( m_lastAction ){
    //    m_lastAction->next = newAction;
    //    m_lastAction = newAction;
    //}
    //else if( m_nextAction ){
    //    m_lastAction = newAction;
    //    m_nextAction->next = m_lastAction;
    //}else{
    //    m_nextAction = newAction;
    //    //qDebug() <<".....................";
    //    runNextAction();
    //}
}

void Stm32Twi::writeCR1( uint16_t newCR1 )
{
                                    // Bit 0 PE: Peripheral enable
                                    // Bit 1 SMBUS: SMBus mode
                                    // Bit 2 Reserved, must be kept at reset value
                                    // Bit 3 SMBTYPE: SMBus type
                                    // Bit 4 ENARP: ARP enable
                                    // Bit 5 ENPEC: PEC enable
    m_genCall = newCR1 & 1<<6;      // Bit 6 ENGC: General call enable
                                    // Bit 7 NOSTRETCH: Clock stretching disable (Slave mode)
    if( newCR1 & 1<<8 )             // Bit 8 START: Start generation
    {
        if( m_mode == TWI_MASTER ) masterStart();
    }
    if( newCR1 & 1<<9 )             // Bit 9 STOP: Stop generation
    {
        if( m_mode == TWI_MASTER ){;} // Stop after current byte transfer or after the current Start condition is sent.
    }
    m_sendACK = newCR1 & 1<<10;     // Bit 10 ACK: Acknowledge enable
                                    // Bit 11 POS: Acknowledge/PEC Position (for data reception)
                                    // Bit 12 PEC: Packet error checking
                                    // Bit 13 ALERT: SMBus alert
                                    // Bit 14 Reserved, must be kept at reset value
    if( newCR1 & 1<<15 )            // Bit 15 SWRST: Software reset
    {
        // Reset
    }
}

void Stm32Twi::writeCR2( uint16_t newCR2 )
{

}
