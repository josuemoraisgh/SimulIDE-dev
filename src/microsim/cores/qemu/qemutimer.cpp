/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "qemutimer.h"

QemuTimer::QemuTimer( QemuDevice* mcu, QString name, int number, uint32_t* frequency, uint64_t memStart, uint64_t memEnd )
         : QemuModule( mcu, name, number, frequency, memStart, memEnd )
         //: McuTimer( nullptr, name )
{
}
QemuTimer::~QemuTimer() {}

