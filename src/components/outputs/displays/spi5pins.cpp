/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "spi5pins.h"

Spi5Pins::Spi5Pins( QString id, Component* comp )
        : SpiModule( id )
        , m_pinDC( 270, QPoint(-56, 66+22 ), id+"-PinDC", 0, comp, input )
        , m_pinRS( 270, QPoint(-48, 66+22 ), id+"-PinRS", 0, comp, input )
        , m_pinCS( 270, QPoint(-40, 66+22 ), id+"-PinCS", 0, comp, input )
        , m_pinDI( 270, QPoint(-32, 66+22 ), id+"-PinDI", 0, comp, input )
        , m_pinCK( 270, QPoint(-24, 66+22 ), id+"-PinCK", 0, comp, input )
        , m_pinDO( 270, QPoint(-16, 66+22 ), id+"-PinDO", 0, comp, output )
{
    /// FIXME: check voltages
    m_pinDC.setLabelText("DC");
    m_pinDC.setInputHighV( 2.31 );
    m_pinDC.setInputLowV( 0.99 );
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
    m_pinDO.setLabelText("DO");
    m_pinDO.setOutHighV( 3.3 );

    // TwiModule:
    m_MOSI   = &m_pinDI;
    m_MISO   = &m_pinDO;
    m_SS     = &m_pinCS;
    m_clkPin = &m_pinCK;

    m_useSS = true;
}
Spi5Pins::~Spi5Pins(){}

void Spi5Pins::stamp()
{
    reset();
    //clearDDRAM();

    m_pinCK.changeCallBack( this );
    m_pinRS.changeCallBack( this );
    m_pinCS.changeCallBack( this );

    m_useSS = true;

    SpiModule::setMode( SPI_SLAVE );
}

void Spi5Pins::reset()
{
    m_inBit  = 0;
    m_buffer = 0;
}

void Spi5Pins::endTransaction()
{
    SpiModule::endTransaction();
    m_isData = m_pinDC.getInpState();
    m_buffer = m_srReg;
}
