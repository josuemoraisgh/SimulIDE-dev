/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "qemutwi.h"

struct esp32TwiArena_t
{
    uint8_t state;
};

class Esp32Twi : public QemuTwi
{
    friend class I2cRunner;

    public:
        Esp32Twi( QemuDevice* mcu, QString n, int number, uint32_t* clk, uint64_t memStart, uint64_t memEnd );
        ~Esp32Twi();

        void reset();

        void connected( bool c ) override;

    protected:
        void writeRegister() override;
        void readRegister()  override;

        void writeCTR();
        void runCMD();
        void setPeriod();

        void setTwiState( twiState_t state ) override;

        uint8_t m_slaveCode;
        int m_lastCommand;
        bool m_opDone;
};

