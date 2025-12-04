/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "qemutwi.h"

class Stm32Twi : public QemuTwi
{
    friend class I2cRunner;

    public:
        Stm32Twi( QemuDevice* mcu, QString name, int number );
        ~Stm32Twi();

        enum stm32TwiAction_t {
            STM32_TWI_CR1=100,
            STM32_TWI_CR2,
        };

        void reset();

        void doAction() override;

    protected:
        void writeCR1( uint16_t data );
        void writeCR2( uint16_t data );

};

