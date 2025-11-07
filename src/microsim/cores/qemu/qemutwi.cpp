/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>

#include "qemutwi.h"
#include "qemudevice.h"
#include "iopin.h"
#include "simulator.h"

QemuTwi::QemuTwi( QemuDevice* mcu, QString name, int number )
       : QemuModule( mcu, number )
       , TwiModule( name )
       , m_runner( this )
{
    m_nextAction = nullptr;
    m_lastAction = nullptr;
}
QemuTwi::~QemuTwi(){}

void QemuTwi::reset()
{
    m_nextAction = nullptr;
    m_lastAction = nullptr;
}

void QemuTwi::doAction()
{
    uint8_t   data = m_arena->data32;
    uint8_t action = m_arena->data8;

    if( m_mode != TWI_MASTER )
    {
        setFreqKHz( 100 );
        setMode( TWI_MASTER );
        m_scl->setPinMode( openCo );
        m_sda->setPinMode( openCo );
    }
    i2cPending_t* newAction = new i2cPending_t{ action, data, nullptr };

    if( m_lastAction )
    {
        m_lastAction->next = newAction;
        m_lastAction = newAction;
    }
    else if( m_nextAction )
    {
        m_lastAction = newAction;
        m_nextAction->next = m_lastAction;
    }
    else
    {
        m_nextAction = newAction;
        //qDebug() <<".....................";
        runNextAction();
    }
}

void QemuTwi::runNextAction()
{
    if( !m_nextAction ) return;

    uint64_t simTime = Simulator::self()->circTime()/1000000;

    switch( m_nextAction->action )
    {
        case QI2C_START_READ:
        qDebug()<< simTime << "QI2C START_READ" ;
            //m_write = false;
            //m_device->getArena()->data32 = 0; // report operation end
            break;
        case QI2C_START_WRITE:
            //qDebug()<< simTime << "QI2C START_WRITE" ;
            masterWrite( m_txAddress<<1, true, true );
            break;
        case QI2C_START_WRITE_ASYNC:
            qDebug()<< simTime << "QI2C START_WRITE_ASYNC" ;

            //m_write = true;
            break;
        case QI2C_STOP:
            //qDebug()<< simTime << "QI2C STOP" ;
            masterStop();
            m_txAddress = 0;
            //m_device->getArena()->data32 = 0; // report operation end
            break;
        case QI2C_NOACK: // Masker NACKed a receive byte.
            qDebug()<< simTime << "QI2C NOACK" ;
            break;
        case QI2C_WRITE:
            //qDebug()<< simTime << "QI2C WRITE" << m_nextAction->data ;
            masterWrite( m_nextAction->data, false, false );
            break;
        case QI2C_READ:
            qDebug()<< simTime << "QI2C READ";
            break;
        case QI2C_MATCH:
            //qDebug()<< simTime << "QI2C MATCH" << m_nextAction->data;
            m_txAddress = m_nextAction->data;
            masterStart();
            //m_device->getArena()->data32 = 0; // report operation end
            break;
    }
}

void QemuTwi::advanceAction()
{
    if( !m_nextAction ) return;
    i2cPending_t* firstACtion = m_nextAction;
    m_nextAction = m_nextAction->next;
    if( m_nextAction == m_lastAction ) m_lastAction = nullptr;
    delete firstACtion;
}

void QemuTwi::setTwiState( twiState_t state )
{
    //uint64_t simTime = Simulator::self()->circTime()/1000000;

    TwiModule::setTwiState( state );
    //qDebug()<< simTime << "QemuTwi::setTwiState"<<state ;

    if( state == TWI_START || state == TWI_REP_START ) // We just sent Start, send slave address
    {
        //qDebug() << "TWI_START completed";
        Simulator::self()->addEvent( 1, &m_runner );
        //m_device->getArena()->data32 = 0; // report operation end
        //masterWrite( m_txAddress, true, m_write );
    }
    else if( state == TWI_MTX_ADR_ACK
          || state == TWI_MTX_ADR_NACK )
    {
        //qDebug() << "Address sent";
        Simulator::self()->addEvent( 1, &m_runner );
    }
    else if( state == TWI_MTX_DATA_ACK
          || state == TWI_MTX_DATA_NACK ) /// TODO separate by ACK
    {
        //qDebug() << "Data sent";
        Simulator::self()->addEvent( 1, &m_runner );
        //m_device->clearData32(); // report operation end
    }
    else if( state == TWI_NO_STATE )
    {
        if( m_i2cState == I2C_STOP ) Simulator::self()->addEvent( 10*1000, &m_runner );
    }
}

void QemuTwi::readByte()
{

    TwiModule::readByte();
}

void QemuTwi::writeByte() // Master is reading, we send byte m_txReg
{
    TwiModule::writeByte();
}

/*void QemuTwi::sendByte( uint8_t data )
{
    if( m_mode == TWI_SLAVE ) m_txReg = data;
    if( m_mode != TWI_MASTER ) return;

    bool write = false;
    bool isAddr = (m_i2cState == TWI_START
                || m_i2cState == TWI_REP_START); // We just sent Start, so this must be slave address

    if( isAddr ) write = (data & 1) == 0;        // Sending address for Read or Write?

    masterWrite( data, isAddr, write );         /// Write data or address to Slave
}*/

void QemuTwi::setMode( twiMode_t mode )
{
    //qDebug() << "QemuTwi::setMode" << mode << (m_scl && m_sda);
    if( m_scl && m_sda ) TwiModule::setMode( mode );
}

// -----------  Class I2cRunner  ---------------------------
//----------------------------------------------------------

I2cRunner::I2cRunner( QemuTwi* twi )
         : eElement("")
{
    m_twi = twi;
}

void I2cRunner::runEvent()
{
    m_twi->advanceAction();
    m_twi->runNextAction();
}

