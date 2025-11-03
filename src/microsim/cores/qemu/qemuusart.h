/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "usartmodule.h"
#include "qemumodule.h"

class QemuUsart : public QemuModule, public UsartModule
{
    public:
        QemuUsart( QemuDevice* mcu, QString name, int number );
        virtual ~QemuUsart();

        enum usartAction_t{
            QUSART_READ=1,
            QUSART_WRITE,
            QUSART_BAUD
        };

        void enable( bool e );

        void sendByte( uint8_t data ) override{ UsartModule::sendByte( data ); }
        void bufferEmpty() override;
        void frameSent( uint8_t data ) override;
        void readByte( uint8_t data ) override;
        void byteReceived( uint8_t data ) override;

        uint8_t getBit9Tx() override;
        void setBit9Rx( uint8_t bit ) override;

        void setPins( QList<IoPin*> pinList );

        void doAction();

    protected:
        //void readBuffer();

        bool m_speedx2;

        regBits_t m_bit9Tx;
        regBits_t m_bit9Rx;
};
