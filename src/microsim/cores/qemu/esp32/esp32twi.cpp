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
    m_opDone = true;
}

void Esp32Twi::doAction()
{
    //uint32_t  data = m_arena->data32;
    //uint8_t action = m_arena->data8;

    //if( !m_clkPin ) m_clkPin = m_scl; /// FIXME

    ////uint64_t now = Simulator::self()->circTime();
    //if( !m_opDone ) qDebug() << "Esp32Twi::doAction ERROR----------------------------";
    //switch( action ) {
    //case QEMU_I2C_START: //qDebug() << "Esp32Twi Start" << now<<now+m_clockPeriod;
    //    masterStart();
    //    m_opDone = false;
    //    //m_arena->qemuEvent = now+m_clockPeriod;
    //    break;
    //case QEMU_I2C_START_READ: //qDebug() << "Esp32Twi Read"  << now<<now+9*m_clockPeriod*2;
    //    masterWrite( data<<1, true, false );
    //    m_opDone = false;
    //    //m_arena->qemuEvent = now+19*m_clockPeriod;
    //    break;
    //case QEMU_I2C_START_WRITE: //qDebug() << "Esp32Twi Write"<< now<<now+19*m_clockPeriod;
    //    masterWrite( data<<1, true, true );
    //    m_opDone = false;
    //    //m_arena->qemuEvent = now+19*m_clockPeriod;
    //    break;
    //case QEMU_I2C_STOP: //qDebug() << "Esp32Twi Stop"  << now<<now+m_clockPeriod*3;
    //    masterStop();
    //    m_opDone = false;
    //    //m_arena->qemuEvent = now+m_clockPeriod*3;
    //    break;
    //case QEMU_I2C_WRITE: //qDebug() << "Esp32Twi write" << now<<now+19*m_clockPeriod;
    //    masterWrite( data, false, true );
    //    m_opDone = false;
    //    //m_arena->qemuEvent = now+19*m_clockPeriod;
    //    break;
    //case QEMU_I2C_READ: //qDebug() << "Esp32Twi read"  << data;
    //    masterRead( true );
    //    break;
    //case QEMU_I2C_FREQ: //qDebug() << "Esp32Twi Freq"  << data;
    //    m_clockPeriod = data*1000/2;
    //    //setFreqKHz( data/1000);
    //    break;
    //case ESP32_TWI_CTR: //qDebug() << "Esp32Twi CTR"   << data;
    //    writeCTR( data );
    //    break;

    //default: break;
    //}
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
    m_opDone = true;

    /// uint8_t ackT = 1;
    /// uint8_t ackR = 1;
    ///
    /// switch( state )
    /// {                        // MASTER
    /// case TWI_START    : //fall through // START transmitted
    /// case TWI_REP_START:
    ///     //printf("Qemu: esp32_i2c_event START %i\n", s->number); fflush( stdout );
    ///     break;     // Repeated START transmitted
    ///
    /// case TWI_MTX_ADR_ACK  : ackT = 0; //fall through // SLA+W transmitted, ACK  received
    /// case TWI_MTX_ADR_NACK :
    ///     //printf("Qemu: esp32_i2c_event ADR %i %i\n", s->number, ackT ); fflush( stdout );
    ///     break; // SLA+W transmitted, NACK received
    /// case TWI_MTX_DATA_ACK : ackT = 0; //fall through // Data transmitted, ACK  received
    /// case TWI_MTX_DATA_NACK:
    ///     //printf("Qemu: esp32_i2c_event DATA %i %i\n", s->number, ackT ); fflush( stdout );
    ///     break; // Data transmitted, NACK received
    ///
    /// case TWI_MRX_ADR_ACK  : ackT = 0; //fall through // SLA+R transmitted, ACK  received
    /// case TWI_MRX_ADR_NACK :               break; // SLA+R transmitted, NACK received
    /// case TWI_MRX_DATA_ACK : ackR = 0; //fall through // Data received, ACK  returned
    /// case TWI_MRX_DATA_NACK: s->bytesRx--; break; // Data received, NACK returned
    ///
    ///     // SLAVE
    /// case TWI_SRX_ADR_ACK      : ackR = 0; break; // Own SLA+W received, ACK returned
    /// case TWI_SRX_GEN_ACK      : ackR = 0; break; // General call received, ACK returned
    /// case TWI_SRX_ADR_DATA_ACK : ackR = 0; break; // data received, ACK returned
    /// case TWI_SRX_ADR_DATA_NACK:           break; // data received, NACK returned
    /// case TWI_SRX_GEN_DATA_ACK : ackR = 0; break; // general call; data received, ACK  returned
    /// case TWI_SRX_GEN_DATA_NACK:           break; // general call; data received, NACK returned
    ///
    /// case TWI_STX_ADR_ACK      : ackR = 0; break; // Own SLA+R received, ACK returned
    /// case TWI_STX_DATA_ACK     : ackT = 0;        // Data transmitted, ACK received
    /// case TWI_STX_DATA_NACK    :           break; // Data transmitted, NACK received
    ///
    /// case TWI_NO_STATE:                    break; // STOP transmitted, Transmission ended
    /// }

    /// //qDebug() << "Esp32Twi::setTwiState" << Simulator::self()->circTime();
    /// m_arena->qemuEvent = state;

    //while( m_arena->qemuAction )        // Wait for previous action executed
    //{
    //    ; /// TODO: add timeout
    //}
    //m_arena->mask8  = ESP32_TWI_EVENT;
    //m_arena->data8  = m_number;
    //m_arena->data16 = state;
    //m_arena->qemuAction = SIM_I2C;
}

