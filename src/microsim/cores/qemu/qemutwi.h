/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "twimodule.h"
#include "qemumodule.h"

class QemuTwi;

class I2cRunner : public eElement
{
    public:
        I2cRunner( QemuTwi* twi );
        void runEvent() override;
    private:
        QemuTwi* m_twi;
};

class QemuTwi : public QemuModule, public TwiModule
{
    friend class I2cRunner;

    public:
        QemuTwi( QemuDevice* mcu, QString name, int number );
        ~QemuTwi();

        enum i2c_action_t {
            QI2C_START_READ=1,
            QI2C_START_WRITE,
            QI2C_START_WRITE_ASYNC,
            QI2C_STOP,
            QI2C_NOACK, /* Masker NACKed a receive byte.  */
            QI2C_WRITE,
            QI2C_READ,
            QI2C_MATCH,
        };

        struct i2cPending_t
        {
            uint32_t action;
            uint8_t  data;
            i2cPending_t* next;
        };

        void reset();

        void doAction( uint32_t action, uint8_t data );

        void readByte() override;

        void writeByte() override;
        //void sendByte( uint8_t data );

        void setMode( twiMode_t mode ) override;

    protected:
        void setTwiState( twiState_t state ) override;
        void runNextAction();
        void advanceAction();

        uint8_t m_txAddress;

        i2cPending_t* m_nextAction;
        i2cPending_t* m_lastAction;

        I2cRunner m_runner;
};

