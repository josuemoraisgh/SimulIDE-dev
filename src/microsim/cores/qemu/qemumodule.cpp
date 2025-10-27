/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "qemumodule.h"

QemuModule::QemuModule( QemuDevice* mcu, int number )
{
    m_device = mcu;
    m_number = number;
    m_arena = m_device->getArena();
}
QemuModule::~QemuModule(){}
