/***************************************************************************
 *   Copyright (C) 2026 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "qemumodule.h"

class Esp32Gpio;

class Esp32IoMux : public QemuModule
{
    public:
        Esp32IoMux( QemuDevice* mcu, QString name, int n, uint32_t* clk, uint64_t memStart, uint64_t memEnd );
        ~Esp32IoMux();

        void reset();
        void readRegister();
        void writeRegister();

        void setGpio( Esp32Gpio* gpio ) { m_gpio = gpio; }

    private:
        int getMuxGpio( uint64_t addr );

        uint16_t m_iomuxReg[40];

        Esp32Gpio* m_gpio;
};
