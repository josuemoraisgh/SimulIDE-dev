/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

//#include "mcutimer.h"
#include "qemumodule.h"


class QemuTimer : public QemuModule //, public McuTimer
{
    public:
        QemuTimer( QemuDevice* mcu, QString name, int number, uint32_t* frequency=nullptr, uint64_t memStart=0, uint64_t memEnd=0 );
        ~QemuTimer();

        //void doAction( uint32_t action, uint32_t data );

    protected:

        uint8_t m_bidirectional;
        uint8_t m_direction;

        uint8_t m_enabled;
        uint8_t m_oneShot;
};
