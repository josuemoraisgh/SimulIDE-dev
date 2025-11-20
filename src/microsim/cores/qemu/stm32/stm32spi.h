/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "qemuspi.h"


class Stm32Spi : public QemuSpi
{
    public:
        Stm32Spi( QemuDevice* mcu, QString name, int number );
        ~Stm32Spi();

        enum stm32_spi_action_t {
            STM32_SPI_CR1=1,
            STM32_SPI_CR2,
            STM32_SPI_READ,
            STM32_SPI_WRITE
        };

        void doAction() override;

        void endTransaction() override;

    private:
};

