/***************************************************************************
 *   Copyright (C) 2026 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "qemumodule.h"
#include "e-element.h"

class Esp32Pin;
class LedTimer;
class LedPwm ;

class Esp32Led : public QemuModule
{
    public:
        Esp32Led( QemuDevice* mcu, QString name, int n, uint32_t* clk, uint64_t memStart, uint64_t memEnd );
        ~Esp32Led();

        void reset() override;

        void setDummy( IoPin* pin );

        IoPin** getPinPtr( int n );
    private:
        void writeRegister() override;
        void readRegister()  override;

        LedTimer* m_timers[8];
        LedPwm*   m_leds[16];
};
// -------------------------------------------------

class LedPwm : public eElement
{
    friend class Esp32Led;

    public:
        LedPwm( QString id );
        ~LedPwm();

        void initialize() override;
        void runEvent() override;

        void ovf( uint64_t p );
        void scheduleEvents();

        void setTimer( LedTimer* t );

    private:

        uint32_t m_matchTime;
        uint32_t m_duty;

        IoPin* m_pin;

        LedTimer* m_timer;
};
// -------------------------------------------------

class LedTimer : public eElement
{
    friend class Esp32Led;

    public:
        LedTimer( QString id );
        ~LedTimer();

        void initialize() override;
        void runEvent() override;

        void setPeriod(uint64_t p );
        uint32_t readCounter();

        void addLedPwm( LedPwm* l );
        void remLedPwm( LedPwm* l );

    private:
        //void UpdatePeriods();

        uint32_t m_period;

        QList<LedPwm*> m_leds;
};
