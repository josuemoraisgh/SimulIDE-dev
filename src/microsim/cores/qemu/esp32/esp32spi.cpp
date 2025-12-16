/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>

#include "esp32spi.h"
#include "qemudevice.h"
#include "iopin.h"
#include "simulator.h"

Esp32Spi::Esp32Spi( QemuDevice* mcu, QString name, int number )
        : QemuSpi( mcu, name, number )
{
    //m_prescList = {2,4,8,16,32,64,128,256};
}
Esp32Spi::~Esp32Spi(){}

void Esp32Spi::doAction()
{
    uint8_t  action = m_arena->data8;

    qDebug() << "Esp32Spi::doAction" << action;
    switch( action )
    {
        case  STM32_SPI_READ:
        {
            m_arena->data16 = m_dataReg;
        }break;
        case  STM32_SPI_WRITE:
        {
            uint16_t newDR = m_arena->data32;
            m_srReg = newDR;
            if( m_mode == SPI_MASTER ) StartTransaction();
        }break;
    }
}

void Esp32Spi::endTransaction()
{
    SpiModule::endTransaction();
    m_dataReg = m_srReg;
    //m_interrupt->raise();
}
