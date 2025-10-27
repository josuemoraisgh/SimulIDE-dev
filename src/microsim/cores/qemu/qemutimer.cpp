/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>

#include "qemutimer.h"
#include "simulator.h"

QemuTimer::QemuTimer( QemuDevice* mcu, QString name, int number )
         : QemuModule( mcu, number )
         , McuTimer( nullptr, name )
{
    m_maxCount = 0xFFFF;
}
QemuTimer::~QemuTimer() {}

void QemuTimer::initialize()
{
    m_oneShot = false;

    m_running = false;
    m_bidirec = false;
    m_reverse = false;
    m_extClock = false;

    m_countVal   = 0;
    m_countStart = 0;
    m_ovfMatch   = 0;
    m_ovfPeriod  = 0;
    m_ovfTime    = 0;
    //m_timeOffset = 0;
    m_circTime   = 0;
    m_mode       = 0;

    m_prescaler = 1;
    m_prIndex = 0;

    //m_clkSrc  = clkMCU;
    m_clkEdge = 1;
}

void QemuTimer::doAction() /// TODO: Move to Stm32Timer
{
    uint8_t  action = m_arena->data8;

    qDebug() << "QemuTimer::doAction" << action;
    switch( action )
    {
    case QTIMER_CR1:
        {
        writeCR1();
        }break;
    case QTIMER_READ:
        {
            updtCount();
            m_arena->data16 = m_countVal;
            qDebug() << "QemuTimer::doAction Read:"<< m_countVal;
        }break;
    case QTIMER_WRITE:
        {
            updtCount();
            m_countVal = m_arena->data32;
            sheduleEvents(); //updtCycles();
        }break;
    case QTIMER_SET_FREQ:
        {
            double freq = m_arena->data32;
            double pres = m_arena->mask32;

            double ps_instr = freq/1000000;
            ps_instr = 1000000/ps_instr;
            ps_instr /= pres;
            m_psPerTick = ps_instr;
            qDebug() << "QemuTimer::doAction Freq:"<< freq <<ps_instr<<m_psPerTick;
            sheduleEvents();
        }break;
    case QTIMER_SET_LIMIT:
        {
            m_ovfMatch = m_arena->data32;
            if( m_bidirec ) m_ovfPeriod = m_ovfMatch;
            else            m_ovfPeriod = m_ovfMatch+1;
            sheduleEvents();
        }break;
    default: break;
    }
}

void QemuTimer::runEvent()           // Overflow
{
    if( !m_running ) return;

    McuTimer::runEvent();
    //static uint64_t lastTime = 0;
    //uint64_t circTime = Simulator::self()->circTime();
    //qDebug() << "\nQemuTimer::runEvent Timer"<< m_number <<"OVF at time" << circTime-lastTime;
    //lastTime = circTime;

    while( m_arena->qemuAction )        // Wait for previous action executed
    {
        ; /// TODO: add timeout
    }
    m_arena->mask8  = QTIMER_OVF;
    m_arena->data8  = m_number;
    m_arena->qemuAction = SIM_TIMER;
}

#define CR1_CEN 1<<0
#define CR1_OPM 1<<3

void QemuTimer::writeCR1()
{
    uint32_t CR1 = m_arena->data32;

    bool enabled = CR1 & CR1_CEN;
    m_oneShot = CR1 & CR1_OPM;

    //Can't switch from edge-aligned to center-aligned if enabled (CEN=1)
    uint8_t CMS = (CR1 & 0b1100000) >> 5;

    switch( CMS ) {
    case 0:
        m_bidirec = 0;
        m_reverse = ( CR1 & 1<<4 ) ? true : false;
        break;
    case 1: //break;
    case 2: //break;
    case 3: m_bidirec = 1; break;
    default: break;
    }

    enable( enabled );// Enable will update & reschedule
}
