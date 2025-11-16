/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "tft3pins.h"

Tft3Pins::Tft3Pins( QString id, Component* comp )
        : eClockedDevice( id )
        , m_pinRS( 270, QPoint(-48, 66+22), id+"-PinRS", 0, comp, input )
        , m_pinCS( 270, QPoint(-40, 66+22), id+"-PinCS", 0, comp, input )
        , m_pinDI( 270, QPoint(-32, 66+22), id+"-PinDI", 0, comp, input )
        , m_pinCK( 270, QPoint(-24, 66+22), id+"-PinCK", 0, comp, input )
{
    /// FIXME: check voltages
    m_pinRS.setLabelText("RS");
    m_pinRS.setInputHighV( 2.31 );
    m_pinRS.setInputLowV( 0.99 );
    m_pinCS.setLabelText("CS");
    m_pinCS.setInputHighV( 2.31 );
    m_pinCS.setInputLowV( 0.99 );
    m_pinDI.setLabelText("DI");
    m_pinDI.setInputHighV( 2.31 );
    m_pinDI.setInputLowV( 0.99 );
    m_pinCK.setLabelText("CK");
    m_pinCK.setInputHighV( 2.31 );
    m_pinCK.setInputLowV( 0.99 );

    m_clkPin = &m_pinCK;
}
Tft3Pins::~Tft3Pins(){}

void Tft3Pins::stamp()
{
    reset();
    //clearDDRAM();

    m_pinCK.changeCallBack( this );
    m_pinRS.changeCallBack( this );
    m_pinCS.changeCallBack( this );
}

void Tft3Pins::reset()
{
    m_inBit  = 0;
    m_buffer   = 0;
}

void Tft3Pins::voltChanged()
{
    bool ret = false;
    if( !m_pinRS.getInpState() ) // Reset Pin is Low
    {
        reset();
        ret = true;
    }
    if( m_pinCS.getInpState() )  // Cs Pin High: Lcd not selected
    {
        m_buffer = 0;             // Initialize serial buffer
        m_inBit  = 0;
        ret = true;
    }
    updateClock();

    if( m_clkState != Clock_Rising ) ret = true;
    if( ret ) return;

    uint8_t bitIn = m_pinDI.getInpState();

    if( m_inBit == 0 ) m_isData = bitIn;
    else{
        m_buffer <<= 1;
        m_buffer |= bitIn;

        if( m_inBit > 7 )    // byte completed
        {
            endTransaction();

            m_inBit = 0;
            m_buffer = 0;
            return;
        }
    }
    m_inBit++;
}
