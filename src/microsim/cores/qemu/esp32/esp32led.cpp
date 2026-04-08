/***************************************************************************
 *   Copyright (C) 2026 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>

#include "esp32led.h"
#include "qemudevice.h"
#include "simulator.h"
#include "iopin.h"

Esp32Led::Esp32Led( QemuDevice* mcu, QString name, int n, uint32_t* clk, uint64_t memStart, uint64_t memEnd )
        : QemuModule( mcu, name, n, clk, memStart, memEnd )
{
    for( int i=0; i<8; ++i )
        m_timers[i] = new LedTimer( name+"-Timer"+QString::number(i) );

    for( int i=0; i<16; ++i )
        m_leds[i] = new LedPwm( name+"-Led"+QString::number(i) );
}
Esp32Led::~Esp32Led(){}

void Esp32Led::reset()
{
    for( int i=0; i<8; ++i ) m_timers[i]->m_leds.clear();

    for( int i=0;  i<8; ++i ) m_leds[i]->setTimer( m_timers[0] );
    for( int i=8; i<16; ++i ) m_leds[i]->setTimer( m_timers[4] );
}

void Esp32Led::writeRegister()
{
    uint64_t offset = m_eventAddress - m_memStart;
    //qDebug() << "Esp32Led::writeRegister" << offset;
    switch( offset ) {
        case 0x000:   // LEDC_HSCHx_CONF0_REG
        case 0x014:
        case 0x028:
        case 0x03C:
        case 0x050:
        case 0x064:
        case 0x078:
        case 0x08C:
        case 0x0A0:   // LEDC_LSCHx_CONF0_REG
        case 0x0B4:
        case 0x0C8:
        case 0x0DC:
        case 0x0F0:
        case 0x104:
        case 0x118:
        case 0x12C:{
            int led = offset/0x14;
            int timer = m_eventValue & 0b11;
            m_leds[led]->setTimer( m_timers[timer] );
            //qDebug() << "LedPwm Timer" << led << timer;
        }break;
        case 0x008:   // LEDC_HSCHx_DUTY_REG
        case 0x01C:
        case 0x030:
        case 0x044:
        case 0x058:
        case 0x06C:
        case 0x080:
        case 0x094:
        case 0x0A8:   // LEDC_LSCHx_DUTY_REG
        case 0x0BC:
        case 0x0D0:
        case 0x0E4:
        case 0x0F8:
        case 0x10C:
        case 0x120:
        case 0x134:{
            int led = (offset-0x008)/0x14;
            m_leds[led]->m_duty = m_eventValue ;
            //qDebug() << "LedPwm Duty" << led << m_eventValue;
        }break;

        case 0x140:   // LEDC_HSTIMERx_CONF_REG
        case 0x148:
        case 0x150:
        case 0x158:
        case 0x160:   // LEDC_LSTIMERx_CONF_REG
        case 0x168:
        case 0x170:
        case 0x178:{
            int timer = (offset-0x140)/8;
            uint32_t freq = m_eventValue;
            uint32_t period_ps = 1e12/freq;
            m_timers[timer]->setPeriod( period_ps );
            //qDebug() << "LedPwm Timer freq" << timer << freq;
        }break;
    }
}

void Esp32Led::readRegister()
{
    uint64_t offset = m_eventAddress - m_memStart;
    uint32_t value = 0;
    //qDebug() <<"Esp32Led::readRegister"<< m_number << offset<< m_eventValue ;
    switch( offset ) {
    }
    m_arena->regData = value;
    m_arena->qemuAction = SIM_READ;
}

IoPin** Esp32Led::getPinPtr( int n )
{
    if( n > 15 ) return nullptr;
    return &m_leds[n]->m_pin;
}

void Esp32Led::setDummy( IoPin* pin )
{
    for( int i=0; i<16; ++i ) m_leds[i]->m_pin = pin;
}

// -------------------------------------------------

LedPwm::LedPwm( QString id )
      : eElement( id )
{
    m_timer = nullptr;
}
LedPwm::~LedPwm(){}

void LedPwm::initialize()
{
    m_duty = 0;
    m_timer = nullptr;
}

void LedPwm::runEvent()
{
    if( m_pin ) m_pin->setOutState( false );
}

void LedPwm::scheduleEvents()
{
    Simulator::self()->addEvent( m_matchTime, this );
}

void LedPwm::ovf( uint64_t p )
{
    if( !m_duty || p == 0 ){
        Simulator::self()->cancelEvents( this );
        return;
    }
    if( m_pin ) m_pin->setOutState( true );
    m_matchTime = p*m_duty/411205;
    //qDebug() << "LedPwm::ovf" << m_matchTime << p << m_duty;
    if( m_matchTime < p ) scheduleEvents();
}

void LedPwm::setTimer( LedTimer* t )
{
    if( m_timer == t ) return;

    if( m_timer ) m_timer->remLedPwm( this );
    m_timer = t;
    m_timer->addLedPwm( this );
}
// -------------------------------------------------

LedTimer::LedTimer( QString id )
        : eElement( id )
{

}
LedTimer::~LedTimer(){}

void LedTimer::initialize()
{
    m_period = 0;
    m_leds.clear();
}

void LedTimer::runEvent()
{
    for( LedPwm* pwm : m_leds ) pwm->ovf( m_period );

    if( m_period ) Simulator::self()->addEvent( m_period, this );
}

void LedTimer::setPeriod( uint64_t p )
{
    //qDebug() << "LedTimer::setPeriod" << m_elmId << p;
    if( m_period == p ) return;

    if( (m_period == 0) || (p == 0) )  // Start or Stop
    {
        if( p == 0 ) Simulator::self()->cancelEvents( this ); // Stop
        else         Simulator::self()->addEvent( p, this );  // Start
        for( LedPwm* pwm : m_leds ) pwm->ovf( p );
    }
    m_period = p;
}

uint32_t LedTimer::readCounter()
{
    return 0;
}

void LedTimer::addLedPwm( LedPwm* l )
{
    if( !m_leds.contains( l) ) m_leds.append( l );
}

void LedTimer::remLedPwm( LedPwm* l )
{
    m_leds.removeOne( l );
}
