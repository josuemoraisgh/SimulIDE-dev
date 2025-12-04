/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>

#include "esp32twi.h"
#include "qemudevice.h"
#include "iopin.h"
#include "simulator.h"

Esp32Twi::Esp32Twi( QemuDevice* mcu, QString name, int number )
        : QemuTwi( mcu, name, number )
{
}
Esp32Twi::~Esp32Twi(){}

void Esp32Twi::reset()
{
}

void Esp32Twi::doAction()
{
    uint16_t  data = m_arena->data32;
    uint8_t action = m_arena->data8;

    switch( action ) {
    case QEMU_I2C_FREQ: setFreqKHz( data/1000); break;
    //case STM32_TWI_CR1: writeCR1( data ); break;
    //case STM32_TWI_CR2: writeCR2( data ); break;

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


