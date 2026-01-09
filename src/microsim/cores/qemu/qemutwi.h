/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "twimodule.h"
#include "qemumodule.h"


class QemuTwi : public QemuModule, public TwiModule
{
    friend class I2cRunner;

    public:
        QemuTwi( QemuDevice* mcu, QString name, int n, uint32_t* clk=nullptr, uint64_t memStart=0, uint64_t memEnd=0 );
        ~QemuTwi();

        void setMode( twiMode_t mode ) override;

        IoPin** getSclPinPtr() { return &m_scl; }
        IoPin** getSdaPinPtr() { return &m_sda; }

    protected:

        //uint8_t m_txAddress;
};

