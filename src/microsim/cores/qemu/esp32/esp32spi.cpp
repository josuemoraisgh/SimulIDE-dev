/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>

#include "esp32spi.h"
#include "qemudevice.h"
#include "iopin.h"
#include "simulator.h"

Esp32Spi::Esp32Spi( QemuDevice* mcu, QString name, int n, uint32_t* clk, uint64_t memStart, uint64_t memEnd )
        : QemuSpi( mcu, name, n, clk, memStart, memEnd )
{
}
Esp32Spi::~Esp32Spi(){}

void Esp32Spi::connected( bool c )
{
    if( c && m_mode == SPI_MASTER ){
        m_dataOutPin = m_MOSI;
        m_dataInPin  = m_MISO;
    }
}

void Esp32Spi::writeRegister()
{
    uint64_t offset = m_eventAddress - m_memStart;

    switch( offset ) {
        case 0x18:{             //SPI_CLOCK_REG
            m_clockPeriod = m_eventValue/2;
            //qDebug() <<"Esp32Spi::writeRegister Period"<< m_number << m_eventValue ;
        }break;
        case 0x38:{             //SPI_SLAVE_REG
            uint32_t slave = m_eventValue & 1<<30;
            setMode( slave ? SPI_SLAVE : SPI_MASTER );
            //qDebug() <<"Esp32Spi::writeRegister Slave"<< m_number<< slave<< m_MOSI->pinId() ;
        }break;
        case 0x80:{            //SPI Data buffer
            //qDebug() <<"Esp32Spi::writeRegister Send"<< m_number<< m_mode << m_eventValue ;
            m_srReg = m_eventValue;
            StartTransaction();
        }break;
    }
}

void Esp32Spi::readRegister()
{
    uint64_t offset = m_eventAddress - m_memStart;
//qDebug() <<"Esp32Spi::readRegister"<< m_number << offset<< m_eventValue ;
    switch( offset ) {
        case 0x80:{          //SPI Data buffer
            m_arena->regData = m_dataReg;
            m_arena->qemuAction = SIM_READ;

            if( m_eventValue > 0xFF ) return;
            m_srReg = m_eventValue;
            StartTransaction();
        }break;
        default:{
            m_arena->regData = 0;
            m_arena->qemuAction = SIM_READ;
        }
    }
}

void Esp32Spi::endTransaction()
{
    SpiModule::endTransaction();
    m_dataReg = m_srReg;
    //qDebug() <<"Esp32Spi::endTransaction"<< m_number<<Simulator::self()->circTime();
    //m_interrupt->raise();
}
