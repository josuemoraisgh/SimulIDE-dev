/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "qemuusart.h"


class Stm32Usart : public QemuUsart
{
    public:
        Stm32Usart( QemuDevice* mcu, QString name, int n, uint32_t* clk, uint64_t memStart, uint64_t memEnd );
        ~Stm32Usart();

        void reset() override;

        void freqChanged() override;


        void frameSent( uint8_t data ) override;
        void byteReceived( uint8_t data ) override;

    private:
        void writeRegister() override;
        void readRegister() override;

        void updateIrq();
        void sendNext();

        void writeCR1( uint16_t newCR1 );
        void writeCR2( uint16_t newCR2 );
        void writeCR3( uint16_t newCR3 );
        void writeSR(  uint16_t newSR );
        void writeDR(  uint16_t newDR );
        void writeBRR( uint16_t newBRR );

        uint16_t m_DR;
        uint16_t m_SR;
        double   m_divider;

        uint16_t m_intEnable;
        uint16_t m_irqLevel;
        uint16_t m_oreRead;

        uint8_t m_interrupt;
};

