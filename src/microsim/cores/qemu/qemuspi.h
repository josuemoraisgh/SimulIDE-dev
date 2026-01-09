/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "spimodule.h"
#include "qemumodule.h"


class QemuSpi : public QemuModule, public SpiModule
{
    public:
        QemuSpi( QemuDevice* mcu, QString name, int n, uint32_t* clk=nullptr, uint64_t memStart=0, uint64_t memEnd=0 );
        ~QemuSpi();

        //void setMode( spiMode_t mode ) override;
        //void endTransaction() override;

        IoPin** getMoPinPtr() { return &m_MOSI; }
        IoPin** getMiPinPtr() { return &m_MISO; }
        IoPin** getCkPinPtr() { return &m_clkPin; }
        IoPin** getSsPinPtr() { return &m_SS; }

    protected:
        uint8_t m_dataReg;

        uint8_t  m_prIndex;                 // Prescaler index
        uint16_t m_prescaler;               // Actual Prescaler value
        std::vector<uint16_t> m_prescList;  // Prescaler values
};

