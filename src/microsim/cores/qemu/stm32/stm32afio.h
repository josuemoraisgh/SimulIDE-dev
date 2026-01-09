/***************************************************************************
 *   Copyright (C) 2026 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "qemumodule.h"


class Stm32Afio : public QemuModule
{
    public:
        Stm32Afio( QemuDevice* mcu, QString name, int n, uint32_t* clk=nullptr, uint64_t memStart=0, uint64_t memEnd=0 );
        ~Stm32Afio();

        void reset();

    private:
        void writeRegister() override;
        void readRegister()  override;

        void writeEXTICR( uint8_t i );
        void writeMAPR();

        uint32_t m_MAPR;

        uint16_t m_EXTICR[4];
};
