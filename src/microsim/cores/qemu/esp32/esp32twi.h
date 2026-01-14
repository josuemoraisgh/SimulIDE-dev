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

        enum esp32TwiAction_t {
            ESP32_TWI_CTR=100,
            //ESP32_TWI_CR2,
            ESP32_TWI_EVENT
        };

        void reset();

        //void doAction() override;

    protected:

        void writeRegister() override;
        void readRegister()  override;

        void writeCTR( uint16_t data );

        void setTwiState( twiState_t state ) override;

        bool m_opDone;
};

