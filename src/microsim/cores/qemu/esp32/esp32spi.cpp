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
    //m_prescList = {2,4,8,16,32,64,128,256};
}
Esp32Spi::~Esp32Spi(){}

void Esp32Spi::writeRegister()
{

}

void Esp32Spi::readRegister()
{

}

void Esp32Spi::endTransaction()
{
    SpiModule::endTransaction();
    m_dataReg = m_srReg;
    //m_interrupt->raise();
}
