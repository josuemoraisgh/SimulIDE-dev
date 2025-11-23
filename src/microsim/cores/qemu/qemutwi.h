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

        enum qemuTwiAction_t {
            QEMU_I2C_START_READ=1,
            QEMU_I2C_START_WRITE,
            QEMU_I2C_START_WRITE_ASYNC,
            QEMU_I2C_STOP,
            QEMU_I2C_NOACK, /* Masker NACKed a receive byte.  */
            QEMU_I2C_WRITE,
            QEMU_I2C_READ,
            QEMU_I2C_MATCH,
            QEMU_I2C_FREQ
        };

        struct i2cPending_t
        {
            uint32_t action;
            uint32_t data;
            i2cPending_t* next;
        };

        void reset();

        void readByte() override;

        void writeByte() override;
        //void sendByte( uint8_t data );

        void setMode( twiMode_t mode ) override;

        virtual void doAction() override;

    protected:
        void setTwiState( twiState_t state ) override;
        void runNextAction();
        void advanceAction();

        uint8_t m_txAddress;

        i2cPending_t* m_nextAction;
        i2cPending_t* m_lastAction;

        I2cRunner m_runner;
};

