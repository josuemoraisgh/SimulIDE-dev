/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "qemutimer.h"

QemuTimer::QemuTimer( QemuDevice* mcu, QString name, int number )
         : McuTimer( nullptr, name )
{
    m_mcu = mcu;
}
QemuTimer::~QemuTimer() {}

void QemuTimer::doAction( uint32_t action, uint32_t data )
{
    switch( action ) {

    default: break;
    }
}
