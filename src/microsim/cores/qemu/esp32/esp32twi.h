/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "qemutwi.h"

class Esp32Twi : public QemuTwi
{
    friend class I2cRunner;

    public:
        Esp32Twi( QemuDevice* mcu, QString name, int number );
        ~Esp32Twi();

        enum stm32TwiAction_t {
            ESP32_TWI_CR1=100,
            ESP32_TWI_CR2,
            ESP32_TWI_
        };

        void reset();

        void doAction() override;

    protected:

};

