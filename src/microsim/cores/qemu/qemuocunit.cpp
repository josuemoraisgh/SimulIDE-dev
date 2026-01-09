/***************************************************************************
 *   Copyright (C) 2026 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "qemuocunit.h"

QemuOcUnit::QemuOcUnit( QemuDevice* mcu, QString name, int number, uint32_t* frequency, uint64_t memStart, uint64_t memEnd )
          : QemuModule( mcu, name, number, frequency, memStart, memEnd )
{
}
QemuOcUnit::~QemuOcUnit() {}

