/***************************************************************************
 *   Copyright (C) 2026 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "e-element.h"

class Stm32Timer;

class Stm32OcUnit : public eElement // QemuOcUnit
{
    friend class Stm32Timer;

    public:
        Stm32OcUnit( Stm32Timer* timer, QString name, int number );
        ~Stm32OcUnit();

        void reset();

        void writeCCMR( uint8_t ccmr );
        void writeCCER( uint16_t ccer );

    private:
        void setPinState( bool s );

        uint8_t m_inverted;

        uint16_t m_CCR;

        uint8_t m_CCMR;
        uint8_t m_CCS;
        uint8_t m_OCM;
        uint8_t m_CCE;

        int m_ICpresc;

        int m_number;

        Stm32Timer* m_timer;
};
