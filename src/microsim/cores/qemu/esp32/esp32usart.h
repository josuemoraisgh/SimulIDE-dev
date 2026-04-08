/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QQueue>

#include "qemuusart.h"


class Esp32Usart : public QemuUsart
{
    public:
        Esp32Usart( QemuDevice* mcu, QString name, int n, uint32_t* clk, uint64_t memStart, uint64_t memEnd );
        ~Esp32Usart();

        void reset() override;

        void freqChanged() override;

        //void enable( bool e ) override;
        void connected( bool c ) override;

        void frameSent( uint8_t data ) override;

    private:
        void writeRegister() override;
        void readRegister()  override;

        void writeCR0();
        //void writeCR1();

        //void updateIrq();

        uint32_t m_divider;

        uint8_t m_apbClock;
        //uint8_t m_rxFullThrhd;
        //uint8_t m_txEmptyThrhd;

        //uint8_t m_irqLevel;

        //uint32_t m_intRaw;
        //uint32_t m_intEn;
        //uint32_t m_intSt;

        QQueue<uint8_t> m_txFifo;
        QQueue<uint8_t> m_rxFifo;
};

