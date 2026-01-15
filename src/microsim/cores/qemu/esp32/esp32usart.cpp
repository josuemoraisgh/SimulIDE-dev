/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>

#include "esp32usart.h"
#include "qemudevice.h"
#include "usarttx.h"
#include "usartrx.h"
#include "iopin.h"
#include "simulator.h"

Esp32Usart::Esp32Usart( QemuDevice* mcu, QString name, int n, uint32_t* clk, uint64_t memStart, uint64_t memEnd )
          : QemuUsart( mcu, name, n, clk, memStart, memEnd )
{
    //m_prescList = {2,4,8,16,32,64,128,256};

}
Esp32Usart::~Esp32Usart(){}

void Esp32Usart::connected( bool c )
{
    enable( c );
    if( c ) m_sender->getPin()->setPinMode( output );
}

void Esp32Usart::writeRegister()
{

}

void Esp32Usart::readRegister()
{

}

void Esp32Usart::writeCR0( uint32_t data )
{
    uint8_t parityOdd = (data & 1<< 0) ? 1 : 0;
    uint8_t parityEn  = (data & 1<< 1) ? 1 : 0;

    if( parityEn ) m_parity = parityOdd ? parODD : parEVEN;
    else           m_parity = parNONE;

    setDataBits( (data & 0b001100) >> 2 );
    m_stopBits = (data & 0b110000) >> 4;

    //uint8_t enableRx = (data & 1<< 2) ? 1 : 0;
    //uint8_t enableTx = (data & 1<< 3) ? 1 : 0;
    //uint8_t enabled  = (data & 1<<13) ? 1 : 0;

    //m_receiver->enable( enabled && enableRx );
    //m_sender->enable( enabled && enableTx );
    //qDebug() << "Stm32Usart::writeCR1"<< data << enabled << enableRx << enableTx;
}

void Esp32Usart::writeCR1( uint32_t data )
{

}

void Esp32Usart::frameSent( uint8_t data )
{
    QemuUsart::frameSent( data );

    //qDebug() << "Esp32Usart::frameSent"<< m_number<<Simulator::self()->circTime();

    //m_device->cancelEvents( this );
    //while( m_arena->qemuAction )        // Wait for previous action executed
    //{
    //    ; /// TODO: add timeout
    //}

    //m_arena->mask8  = QEMU_USART_SENT;
    //m_arena->data8  = m_number;
    //m_arena->qemuAction = SIM_USART;
}

//void Esp32Usart::endTransaction()
//{
//    SpiModule::endTransaction();
//    m_dataReg = m_srReg;
//    //m_interrupt->raise();
//}
