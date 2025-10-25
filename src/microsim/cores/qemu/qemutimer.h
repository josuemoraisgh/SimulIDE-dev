/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "mcutimer.h"

class QemuDevice;

class QemuTimer : public McuTimer
{
    public:
        QemuTimer( QemuDevice* mcu, QString name, int number );
        ~QemuTimer();

        void doAction( uint32_t action, uint32_t data );

    private:
        QemuDevice* m_mcu;
};
