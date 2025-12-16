/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>

#include "esp32twi.h"
#include "qemudevice.h"
#include "iopin.h"
#include "simulator.h"

Esp32Twi::Esp32Twi( QemuDevice* mcu, QString name, int number )
        : QemuTwi( mcu, name, number )
{
}
Esp32Twi::~Esp32Twi(){}

void Esp32Twi::reset()
{
}

void Esp32Twi::doAction()
{
    uint32_t  data = m_arena->data32;
    uint8_t action = m_arena->data8;

    if( !m_clkPin ) m_clkPin = m_scl; /// FIXME

    uint64_t now = Simulator::self()->circTime();

    switch( action ) {
    case QEMU_I2C_START: qDebug() << "Esp32Twi Start" << now<<now+m_clockPeriod;
        masterStart();
        m_arena->qemuEvent = now+m_clockPeriod;
        break;
    case QEMU_I2C_START_READ: qDebug() << "Esp32Twi Read"  << now<<now+9*m_clockPeriod*2;
        masterWrite( data<<1, true, false );
        m_arena->qemuEvent = now+9*m_clockPeriod*2;
        break;
    case QEMU_I2C_START_WRITE: qDebug() << "Esp32Twi Write"<< now<<now+19*m_clockPeriod;
        masterWrite( data<<1, true, true );
        m_arena->qemuEvent = now+19*m_clockPeriod;
        break;
    case QEMU_I2C_STOP: qDebug() << "Esp32Twi Stop"  << now<<now+m_clockPeriod;
        masterStop();
        m_arena->qemuEvent = now+m_clockPeriod;
        break;
    case QEMU_I2C_WRITE: qDebug() << "Esp32Twi write" << now<<now+19*m_clockPeriod;
        masterWrite( data, false, true );
        m_arena->qemuEvent = now+19*m_clockPeriod;
        break;
    case QEMU_I2C_READ: //qDebug() << "Esp32Twi read"  << data;
        masterRead( true );
        break;
    case QEMU_I2C_FREQ: qDebug() << "Esp32Twi Freq"  << data/1000;
        setFreqKHz( data/1000);
        break;
    case ESP32_TWI_CTR: //qDebug() << "Esp32Twi CTR"   << data;
        writeCTR( data );
        break;

    default: break;
    }
}

void Esp32Twi::writeCTR( uint16_t data )
{
    // bit 0: I2C_SDA_FORCE_OUT 0: direct output; 1: open drain output.
    pinMode_t sdaMode = (data & 1<<0) ? openCo : output;
    this->m_sda->setPinMode( sdaMode );

    // bit 1: I2C_SCL_FORCE_OUT 0: direct output; 1: open drain output.
    pinMode_t sclMode = (data & 1<<1) ? openCo : output;
    this->m_scl->setPinMode( sclMode );

    // bit 2: I2C_SAMPLE_SCL_LEVEL 1: sample SDA on SCL low; 0: sample SDA on SCL high.

    // bit 4: I2C_MS_MODE 1: I2C Master. 0: I2C Slave.
    twiMode_t mode = (data & 1<<4) ? TWI_MASTER : TWI_SLAVE;
    if( m_mode != mode ) setMode( mode );
    // bit 5: I2C_TRANS_START Set this bit to start sending the data in txfifo.
    // bit 6: I2C_TX_LSB_FIRST 1: send LSB; 0: send MSB.
    // bit 7: I2C_RX_LSB_FIRST 1: receive LSB; 0: receive MSB.

}

void Esp32Twi::setTwiState( twiState_t state )
{
    TwiModule::setTwiState( state );

    qDebug() << "Esp32Twi::setTwiState" << Simulator::self()->circTime();
    m_arena->qemuEvent = state;

    //while( m_arena->qemuAction )        // Wait for previous action executed
    //{
    //    ; /// TODO: add timeout
    //}
    //m_arena->mask8  = ESP32_TWI_EVENT;
    //m_arena->data8  = m_number;
    //m_arena->data16 = state;
    //m_arena->qemuAction = SIM_I2C;
}

