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
    m_lastCommand = -1;
}

void Esp32Twi::connected( bool c )
{
    m_clkPin = m_scl;
}

void Esp32Twi::writeRegister()
{
    uint64_t offset = m_eventAddress - m_memStart;
//qDebug() <<"Esp32Twi::writeRegister"<< offset ;
    switch( offset ) {
    case 0x00: setPeriod(); break; // LOW_PERIOD
    case 0x04: writeCTR();  break; // I2C_CTR
    case 0x58: runCMD();    break; // I2C_CMD
    default: break;
    }
}

void Esp32Twi::readRegister()
{
    uint64_t offset = m_eventAddress - m_memStart;
    //qDebug() << "Esp32Twi::readRegister" << offset;
    switch( offset ) {
    case 0x08:                         // I2C_STATUS
        m_arena->regData = m_twiState;
        break;
    default: break;
    }
    m_arena->qemuAction = SIM_READ;
}

void Esp32Twi::writeCTR()
{
    uint16_t data = m_eventValue;

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
    //if( data & 1<<5 )
    //{
    //    masterStart();
    //    m_lastCommand = 0;
    //    //qDebug() << "Esp32Twi::writeCTR START" ;
    //}


    //esp32_i2c_do_transaction(s);
    //m_eventValue &= ~(1<<5);

    // bit 6: I2C_TX_LSB_FIRST 1: send LSB; 0: send MSB.
    // bit 7: I2C_RX_LSB_FIRST 1: receive LSB; 0: receive MSB.

}

void Esp32Twi::runCMD()
{
    // ACK_CHECK_EN,  8, 1
    // ACK_EXP     ,  9, 1
    // ACK_VAL     , 10, 1
    bool ack = m_eventValue & 1<<10; //m_sendACK = ack;
    // OPCODE      , 11, 3
    uint32_t opcode = (m_eventValue & (0b111<<11)) >> 11;

    switch( opcode ) {
        case 0: masterStart();     break; // RSTART
        case 1:                           // WRITE
        {
            uint32_t data = m_eventValue & 0xFF;
            bool isAddr = (m_eventValue & 1<<16) != 0; //(m_lastCommand == 0);
            if( isAddr ){
                masterStart();
                m_write = (data & 1) == 0;
                m_slaveCode = data;
                //qDebug() <<"Esp32Twi::runCMD WRITE addr"<<data;
            }else{
                masterWrite( data, false, m_write );
                //qDebug() <<"Esp32Twi::runCMD WRITE data"<<data;
            }
        }break;
        case 2: masterRead( ack ); break; // READ
        case 3: masterStop();      break; // STOP
        case 4:                    break; // END
        default: break;
    }
    m_lastCommand = opcode;
}

void Esp32Twi::setPeriod()
{
    uint32_t period_ns = m_eventValue;
    m_clockPeriod = period_ns*1000/2;
    //qDebug() << "Esp32Twi::setPeriod" << m_clockPeriod<< 1e9/period_ns<<"Hz";
}

void Esp32Twi::setTwiState( twiState_t state )
{
    TwiModule::setTwiState( state );
    uint8_t ackT = 1;
    uint8_t ackR = 1;

    switch( state )
    {              // MASTER
    case TWI_START    :                              // START transmitted
    case TWI_REP_START:                              // Repeated START transmitted
        masterWrite( m_slaveCode, true, m_write );
        m_lastState = I2C_WRITE;
        break;
    case TWI_MTX_ADR_ACK  : ackT = 0;                // SLA+W transmitted, ACK  received
        //fall through
    case TWI_MTX_ADR_NACK :                          // SLA+W transmitted, NACK received
        break;
    case TWI_MTX_DATA_ACK : ackT = 0;                // Data transmitted, ACK  received
        //fall through
    case TWI_MTX_DATA_NACK:                          // Data transmitted, NACK received
        break;
    case TWI_MRX_ADR_ACK  : ackT = 0;                // SLA+R transmitted, ACK  received
        //fall through
    case TWI_MRX_ADR_NACK :                          // SLA+R transmitted, NACK received
        break;
    case TWI_MRX_DATA_ACK : ackR = 0;                // Data received, ACK  returned
        //fall through
    case TWI_MRX_DATA_NACK:                          // Data received, NACK returned
        break;

        // SLAVE
    case TWI_SRX_ADR_ACK      : ackR = 0; break;     // Own SLA+W received, ACK returned
    case TWI_SRX_GEN_ACK      : ackR = 0; break;     // General call received, ACK returned
    case TWI_SRX_ADR_DATA_ACK : ackR = 0; break;     // data received, ACK returned
    case TWI_SRX_ADR_DATA_NACK:                      // data received, NACK returned
        //m_DR = m_rxReg;
        break;
    case TWI_SRX_GEN_DATA_ACK : ackR = 0; break;     // general call; data received, ACK  returned
    case TWI_SRX_GEN_DATA_NACK:                ;     // general call; data received, NACK returned
        //m_DR = m_rxReg;
        break;

    case TWI_STX_ADR_ACK      : ackR = 0; break;     // Own SLA+R received, ACK returned
    case TWI_STX_DATA_ACK     : ackT = 0;            // Data transmitted, ACK received
    case TWI_STX_DATA_NACK    :           break;     // Data transmitted, NACK received

    case TWI_NO_STATE:                    break;     // STOP transmitted, Transmission ended
    default: break;
    }

    //if( state != TWI_START ) qDebug() << "Esp32Twi::setTwiState" << state << Simulator::self()->circTime();
}

