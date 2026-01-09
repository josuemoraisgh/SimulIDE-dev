/***************************************************************************
 *   Copyright (C) 2026 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "qemutimer.h"

class Stm32OcUnit;

class Stm32Timer : public QemuTimer //, public McuTimer
{
    public:
        Stm32Timer( QemuDevice* mcu, QString name, int n, uint32_t* clk=nullptr, uint64_t memStart=0, uint64_t memEnd=0 );
        ~Stm32Timer();

        void reset();

        uint32_t getCount();
        void setCount();

    private:
        void writeRegister() override;
        void readRegister()  override;

        void writeCR1();
        void writeCCER();
        void writeEGR();
        void writeCCMR( int i );

        void updtFreq();
        void updtPeriod();
        void updateUIF();

        uint16_t m_cr1;
        uint16_t m_cr2;  //Extended modes not supported
        uint16_t m_smcr; //Slave mode not supported
        uint16_t m_dier;
        uint16_t m_sr;
        uint16_t m_egr;
        uint16_t m_ccmr[2];
        uint16_t m_ccer;
     // uint16_t m_cnt;
        uint16_t m_psc;
        uint16_t m_arr;
        uint16_t m_rcr; //Repetition count not supported

        uint16_t m_bdtr; //Break and deadtime not supported
        uint16_t m_dcr;  //DMA mode not supported
        uint16_t m_dmar; //DMA mode not supported

        Stm32OcUnit* m_channel[4];
};
