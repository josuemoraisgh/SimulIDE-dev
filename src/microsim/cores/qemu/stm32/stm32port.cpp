/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>

#include "stm32port.h"

#define CRL_OFFSET  0x00
#define CRH_OFFSET  0x04
#define IDR_OFFSET  0x08
#define ODR_OFFSET  0x0C
#define BSRR_OFFSET 0x10
#define BRR_OFFSET  0x14
#define LCKR_OFFSET 0x18


Stm32Port::Stm32Port(QemuDevice* mcu, QString n, int number, uint32_t* clk, uint64_t memStart, uint64_t memEnd )
         : QemuModule( mcu, n, number, clk, memStart, memEnd )
         , eElement( n )
{
}
Stm32Port::~Stm32Port(){}

void Stm32Port::reset()
{
    m_pinState = 0;

    writeMem( m_memStart+CRL_OFFSET, 0x44444444 );
    writeMem( m_memStart+CRH_OFFSET, 0x44444444 );
    writeMem( m_memStart+ODR_OFFSET, 0 );
}

//void Stm32Port::doAction() //addEvent( uint64_t address, uint64_t value )
//{
    //qDebug() <<"Stm32Port::writeRegister"<< m_name << address << value;

    //// FIXME m_device->waitEvent( this );

    //uint64_t offset = address - m_memStart;

    //switch( offset )
    //{
    //case CRL_OFFSET:
    //    if( value == m_CRL ) break;
    //    m_CRL = value;
    //    cofigPort( value, 0 );
    //    break;
    //case CRH_OFFSET:
    //    if( value == m_CRH ) break;
    //    m_CRH = value;
    //    cofigPort( value, 8 ); // shift Pin number by 8
    //    break;
    //case IDR_OFFSET:
    //    /// STM32_RO_REG( offset );
    //    break;
    //case ODR_OFFSET:
    //    if( value == m_ODR ) break;
    //    m_ODR = value;
    //    setPortState( value );
    //    break;
    //case BSRR_OFFSET:{
    //    uint32_t set_mask = value & 0x0000FFFF;
    //    uint32_t reset_mask = ~(value >> 16) & 0x0000FFFF;
    //    value = (m_ODR & reset_mask) | set_mask; // Sets take priority over resets, so we do
    //    if( value == m_ODR ) break;
    //    m_ODR = value;
    //    setPortState( value );
    //} break;
    //case BRR_OFFSET:{
    //    uint32_t reset_mask = ~value & 0x0000FFFF;
    //    value = m_ODR & reset_mask;
    //    if( value == m_ODR ) break;
    //    m_ODR = value;
    //    setPortState( value );
    //} break;
    //case LCKR_OFFSET:
    //    /// TODO: Locking is not implemented
    //    break;
    //default: break;
    //}

    //uint64_t value   = m_arena->data32;
    //uint64_t address = m_arena->mask32;
    //uint64_t offset  = address - m_memStart;

    //if( m_arena->simuAction != SIM_WRITE )
    //{
    //    return;
    //}

    ////qDebug() <<"Stm32Port::writeRegister"<< m_name << address << value;

    //switch( offset )  // We add events only if there is Circuit action
    //{
    //case CRL_OFFSET:
    //    if( value == m_CRL ) break;
    //    m_CRL = value;
    //    QemuModule::addEvent( address, value );
    //    break;
    //case CRH_OFFSET:
    //    if( value == m_CRH ) break;
    //    m_CRH = value;
    //    QemuModule::addEvent( address, value );
    //    break;
    //case IDR_OFFSET:
    //    /// STM32_RO_REG( offset );
    //    break;
    //case ODR_OFFSET:
    //    if( value == m_ODR ) break;
    //    m_ODR = value;
    //    QemuModule::addEvent( address, value );
    //    break;
    //case BSRR_OFFSET:{
    //    uint32_t set_mask = value & 0x0000FFFF;
    //    uint32_t reset_mask = ~(value >> 16) & 0x0000FFFF;
    //    value = (m_ODR & reset_mask) | set_mask; // Sets take priority over resets, so we do
    //    if( value == m_ODR ) break;
    //    m_ODR = value;
    //    QemuModule::addEvent( address, value );
    //} break;
    //case BRR_OFFSET:{
    //    uint32_t reset_mask = ~value & 0x0000FFFF;
    //    value = m_ODR & reset_mask;
    //    if( value == m_ODR ) break;
    //    m_ODR = value;
    //    QemuModule::addEvent( address, value );
    //} break;
    //case LCKR_OFFSET:
    //    /// TODO: Locking is not implemented
    //    break;
    //default: break;
    //}
//}

void Stm32Port::readRegister()
{
    //qDebug() <<"Stm32Port::readRegister"<< m_name << m_eventAddress << m_eventValue;

    uint64_t offset = m_eventAddress - m_memStart;
    uint32_t val = 0;
    switch( offset )
    {
        //case CRL_OFFSET: break;
        //case CRH_OFFSET: break;
        case IDR_OFFSET:  val = readPort(); break;
        //case ODR_OFFSET: break;
        case BSRR_OFFSET:                   break; // Write only
        case BRR_OFFSET:                    break; // Write only
        //case LCKR_OFFSET: break;
        default:          val = read();     break;
    }
    m_arena->regData = val;
    m_arena->qemuAction = SIM_READ;
}
void Stm32Port::writeRegister()
{
    uint64_t offset = m_eventAddress - m_memStart;

    switch( offset )
    {
        case CRL_OFFSET:
            if( m_eventValue == read() ) break;
            write();
            cofigPort( m_eventValue, 0 );
            break;
        case CRH_OFFSET:
            if( m_eventValue == read() ) break;
            write();
            cofigPort( m_eventValue, 8 ); // shift Pin number by 8
            break;
        case IDR_OFFSET:                 break; // Read only;

        case ODR_OFFSET:
            if( m_eventValue == read() ) break;
            write();
            setPortState( m_eventValue );
            break;
        case BSRR_OFFSET:{
            uint32_t set_mask = m_eventValue & 0x0000FFFF;
            uint32_t reset_mask = ~(m_eventValue >> 16) & 0x0000FFFF;
            uint32_t ODR = readMem( m_memStart+ODR_OFFSET );
            m_eventValue = ( ODR & reset_mask) | set_mask; // Sets take priority over resets, so we do
            if( m_eventValue == ODR ) break;
            writeMem( m_memStart+ODR_OFFSET, m_eventValue );
            setPortState( m_eventValue );
        } break;
        case BRR_OFFSET:{
            uint32_t reset_mask = ~m_eventValue & 0x0000FFFF;
            uint32_t ODR = readMem( m_memStart+ODR_OFFSET );
            m_eventValue = ODR & reset_mask;
            if( m_eventValue == ODR ) break;
            writeMem( m_memStart+ODR_OFFSET, m_eventValue );
            setPortState( m_eventValue );
        } break;
        case LCKR_OFFSET: write(); break; /// TODO: Locking is not implemented

        default: write(); break;
    }
}

void Stm32Port::cofigPort( uint32_t config, uint8_t shift )
{
    //if( config == 0 ) return;
    //qDebug() << "Stm32Port::cofigPort:" << m_name << config << shift;

    for( uint8_t i=shift; i<shift+8; ++i )
    {
        Stm32Pin*  ioPin = m_pins.at( i );
        uint8_t cfgShift = i*4;
        uint32_t cfgMask = 0b1111 << cfgShift;
        uint32_t cfgBits = (config & cfgMask) >> cfgShift;

        uint8_t isOutput = cfgBits & 0b0011;  // 0 = Input

        if( isOutput ) // Output
        {
            uint8_t   open = cfgBits & 0b0100;
            pinMode_t pinMode = open ? openCo : output;
            ioPin->setPinMode( pinMode );
        }
        else          // Input
        {
            ioPin->setPinMode( input );
            uint8_t pull = cfgBits & 0b1000;
            ioPin->setPull( pull>0 );
        }

        uint8_t mode = cfgBits & 0b1100; // Analog if CNF0[1:0] == 0
        ioPin->setAnalog( mode==0 );
        /// TODO: if changing to Not Analog // Restore Port State

        uint8_t alternate = cfgBits & 0b1000;
        if( !ioPin->setAlternate( alternate>0 ) )            // If changing to No Alternate
            ioPin->setPortState( (m_pinState & 1<<i) > 0 );  // Restore Port state
    }
}
#include "simulator.h"
void Stm32Port::setPortState( uint16_t state )
{
    //qDebug() << "   Stm32Port::setPortState:               " << m_name << state<< Simulator::self()->circTime();

    for( uint8_t i=0; i<m_pins.size(); ++i )
    {
        Stm32Pin* ioPin = m_pins.at( i );
        ioPin->setPortState( state & (1<<i) );
    }
}

uint32_t Stm32Port::readPort()
{
    uint32_t data = 0;
    for( uint8_t i=0; i<m_pins.size(); ++i )
        if( m_pins[i]->getInpState() ) data |= (1 << i);

    return data;
}

Stm32Pin* Stm32Port::createPin( int i, QString id , QemuDevice* mcu )
{
    Stm32Pin* pin = new Stm32Pin( m_number, i, id, mcu );

    m_pins.emplace_back( pin );

    return pin;
}
