/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>

#include "stm32usart.h"
#include "qemudevice.h"
#include "usarttx.h"
#include "usartrx.h"
#include "iopin.h"
#include "simulator.h"

Stm32Usart::Stm32Usart( QemuDevice* mcu, QString name, int number )
          : QemuUsart( mcu, name, number )
{
    //m_prescList = {2,4,8,16,32,64,128,256};
}
Stm32Usart::~Stm32Usart(){}

void Stm32Usart::doAction()
{
    uint8_t  action = m_arena->data8;
    uint32_t  data  = m_arena->data32;
    //qDebug() << "Stm32Usart::doAction Uart:"<< m_number << "action:"<< action<< "data:" << data;
    switch( action ) {
        case STM32_USART_CR1:   writeCR1( data ); break;
        case STM32_USART_CR2:   writeCR1( data ); break;
        //case STM32_USART_READ:  /*readByte( data );*/ break;
        case STM32_USART_WRITE: sendByte( data ); break;
        case STM32_USART_BAUD:  setBaudRate( data ); qDebug() << "Stm32Usart::doAction Baudrate:"<<data; break;

        default: break;
    }
}

void Stm32Usart::writeCR1( uint16_t data )
{
    uint8_t enableRx = (data & 1<< 2) ? 1 : 0;
    uint8_t enableTx = (data & 1<< 3) ? 1 : 0;
    uint8_t enabled  = (data & 1<<13) ? 1 : 0;

    m_receiver->enable( enabled && enableRx );
    m_sender->enable( enabled && enableTx );
    //qDebug() << "Stm32Usart::writeCR1"<< data << enabled << enableRx << enableTx;
}

void Stm32Usart::writeCR2( uint16_t data )
{

}

//void Stm32Usart::endTransaction()
//{
//    SpiModule::endTransaction();
//    m_dataReg = m_srReg;
//    //m_interrupt->raise();
//}
