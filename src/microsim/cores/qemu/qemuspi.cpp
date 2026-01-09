/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>

#include "qemuspi.h"
#include "qemudevice.h"
#include "iopin.h"
#include "simulator.h"

QemuSpi::QemuSpi(QemuDevice* mcu, QString name, int n, uint32_t* clk, uint64_t memStart, uint64_t memEnd )
       : QemuModule( mcu, name, n, clk, memStart, memEnd )
       , SpiModule( name )
{

}
QemuSpi::~QemuSpi(){}

//void QemuSpi::doAction()
//{
//
//}

//void QemuSpi::setMode( spiMode_t mode )
//{
//}
//
//void QemuSpi::endTransaction()
//{
//    SpiModule::endTransaction();
//    //*m_dataReg = m_srReg;
//    //m_interrupt->raise();
//}
