/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>

#include "esp32twi.h"
#include "qemudevice.h"
#include "iopin.h"
#include "simulator.h"

Esp32Twi::Esp32Twi( QemuDevice* mcu, QString name, int n, uint32_t *clk, uint64_t memStart, uint64_t memEnd )
        : QemuTwi( mcu, name, n, clk, memStart, memEnd )
{
}
Esp32Twi::~Esp32Twi(){}

void Esp32Twi::reset()
{
    m_opDone = true;
}

void Esp32Twi::writeRegister()
{

}

void Esp32Twi::readRegister()
{

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
}

