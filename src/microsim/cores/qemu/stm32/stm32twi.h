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
        Stm32Twi( QemuDevice* mcu, QString name, int n, uint32_t* clk, uint64_t memStart, uint64_t memEnd );
        ~Stm32Twi();

        void reset();


    private:
        void writeRegister() override;
        void readRegister()  override;

        void setTwiState( twiState_t state ) override;

        void writeCR1( uint16_t newCR1 );
        void writeCR2( uint16_t newCR2 );
        void writeDR( uint16_t newDR );

        void i2cStop();
        void updateIrq();

        bool m_enabled;

        uint16_t m_CR1  ;
        uint16_t m_CR2  ;
        uint16_t m_OAR1 ;
        uint16_t m_OAR2 ;
        uint16_t m_DR   ;
        uint16_t m_SR1  ;
        uint16_t m_SR2  ;
        uint16_t m_CCR  ;
        uint16_t m_TRISE;

        uint8_t m_sr1Read;
        uint8_t m_needStop;
        uint8_t m_nextData;

        uint8_t  m_evtInterrupt;
        uint16_t m_evtIrqLevel;

        uint8_t  m_errInterrupt;
        uint16_t m_errIrqLevel;
};

