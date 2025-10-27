/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "qemudevice.h"

class QemuModule
{
    public:
        QemuModule( QemuDevice* mcu, int number );
        ~QemuModule();

    protected:

        int m_number;

        QemuDevice* m_device;

        volatile qemuArena_t* m_arena;
};

