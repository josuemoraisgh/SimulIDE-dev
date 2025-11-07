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
        QemuSpi( QemuDevice* mcu, QString name, int number );
        ~QemuSpi();

        //enum spi_action_t {
        //    QSPI_STOP,
        //};
        //
        //void doAction() override;

        //void setMode( spiMode_t mode ) override;
        //void endTransaction() override;

    protected:
        uint8_t m_dataReg;

        uint8_t  m_prIndex;                 // Prescaler index
        uint16_t m_prescaler;               // Actual Prescaler value
        std::vector<uint16_t> m_prescList;  // Prescaler values
};

