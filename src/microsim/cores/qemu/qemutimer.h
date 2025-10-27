/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "mcutimer.h"
#include "qemumodule.h"

class QemuTimer : public QemuModule, public McuTimer
{
    public:
        QemuTimer( QemuDevice* mcu, QString name, int number );
        ~QemuTimer();

        enum timerAction_t{
            QTIMER_CR1=1,
            QTIMER_READ,
            QTIMER_WRITE,
            QTIMER_SET_FREQ,
            QTIMER_SET_LIMIT,
            QTIMER_OVF,
        };

        virtual void initialize() override;
        virtual void runEvent() override;

        void doAction();

    private:
        void writeCR1();

        bool m_oneShot;
};
