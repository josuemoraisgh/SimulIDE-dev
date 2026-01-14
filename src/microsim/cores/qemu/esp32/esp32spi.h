/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "qemuspi.h"


class Esp32Spi : public QemuSpi
{
    public:
        Esp32Spi( QemuDevice* mcu, QString name, int n, uint32_t* clk, uint64_t memStart, uint64_t memEnd );
        ~Esp32Spi();

        void endTransaction() override;

    private:
        void writeRegister() override;
        void readRegister()  override;
};

