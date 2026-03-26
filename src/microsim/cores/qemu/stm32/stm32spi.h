/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "qemuspi.h"


class Stm32Spi : public QemuSpi
{
    public:
        Stm32Spi( QemuDevice* mcu, QString name, int n, uint32_t* clk, uint64_t memStart, uint64_t memEnd );
        ~Stm32Spi();

        void reset() override;

        void endTransaction() override;

    private:
        void writeRegister() override;
        void readRegister()  override;

        void writeCR1();
        void writeCR2();
        void writeDR();

        uint16_t m_status;

        uint16_t m_CR1;
        uint16_t m_CR2;
};

