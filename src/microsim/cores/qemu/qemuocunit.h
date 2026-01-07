/***************************************************************************
 *   Copyright (C) 2026 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "qemumodule.h"

class QemuOcUnit : public QemuModule
{
    public:
        QemuOcUnit( QemuDevice* mcu, QString name, int number, uint32_t* frequency=nullptr, uint64_t memStart=0, uint64_t memEnd=0 );
        ~QemuOcUnit();

        //void doAction( uint32_t action, uint32_t data );

    protected:

};
