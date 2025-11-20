/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/


#include <QDebug>

#include "qemuusart.h"
#include "qemudevice.h"
#include "serialmon.h"
#include "usartrx.h"
#include "usarttx.h"

#include "simulator.h"

QemuUsart::QemuUsart( QemuDevice* mcu, QString name, int number )
         : QemuModule( mcu, number )
         , UsartModule( nullptr, mcu->getId()+"-"+name )
{
    /// FIXME ----------------------------

    this->setBaudRate( 9600 );
}
QemuUsart::~QemuUsart( ){}

void QemuUsart::enable( bool e )
{
    //m_serData.clear();
    //m_uartData.clear();
    m_sender->enable( e );
    m_receiver->enable( e );
    //m_sending = false;
    //m_receiving = false;
}

void QemuUsart::doAction()
{
    qDebug() << "QemuUsart::doAction Uart ERROR:"<< m_number;
}

void QemuUsart::bufferEmpty()
{
    //if( m_interrupt ) m_interrupt->raise(); // USART Data Register Empty Interrupt
}

void QemuUsart::frameSent( uint8_t data )
{
    if( m_monitor ) m_monitor->printOut( data );
    //m_sender->raiseInt();
}

void QemuUsart::readByte( uint8_t )
{
    //if( m_mcu->isCpuRead() ) m_mcu->m_regOverride = m_receiver->getData();
}

void QemuUsart::byteReceived( uint8_t data )
{
    UsartModule::byteReceived( data );

    while( m_arena->qemuAction )        // Wait for previous action executed
    {
        ; /// TODO: add timeout
    }
    m_arena->mask8  = QEMU_USART_RECEIVE;
    m_arena->data8  = m_number;
    m_arena->data16 = m_receiver->getData();
    //qDebug() << "QemuUsart::byteReceived"<< m_number << m_arena->data16 << "at time" << Simulator::self()->circTime();
    m_arena->qemuAction = SIM_USART;
}

uint8_t QemuUsart::getBit9Tx()
{
    //return getRegBitsVal( m_bit9Tx );
    return 0;
}

void QemuUsart::setBit9Rx( uint8_t bit )
{
    //writeRegBits( m_bit9Rx, bit );
}

void QemuUsart::setPins( QList<IoPin*> pinList )
{
    m_sender->setPins( {pinList.at(0)} );
    m_receiver->setPins( {pinList.at(1)} );
}
