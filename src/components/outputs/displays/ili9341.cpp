/***************************************************************************
 *   Copyright (C) 2019 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "ili9341.h"
#include "itemlibrary.h"
#include "simulator.h"

#define tr(str) simulideTr("Ili9341",str)

Component* Ili9341::construct( QString type, QString id )
{ return new Ili9341( type, id ); }

LibraryItem* Ili9341::libraryItem()
{
    return new LibraryItem(
        "Ili9341" ,
        "Displays",
        "ili9341.png",
        "Ili9341",
        Ili9341::construct );
}

Ili9341::Ili9341( QString type, QString id )
       : TftController( type, id )
       , eClockedDevice( id )
       , m_pinCS (  270, QPoint(-56, 184), id+"-PinCS"  , 0, this, input )
       , m_pinRst(  270, QPoint(-48, 184), id+"-PinRst" , 0, this, input )
       , m_pinDC (  270, QPoint(-40, 184), id+"-PinDC"  , 0, this, input )
       , m_pinMosi( 270, QPoint(-32, 184), id+"-PinMosi", 0, this, input )
       , m_pinSck(  270, QPoint(-24, 184), id+"-PinSck" , 0, this, input )
{
    m_isILI = true;

    setDisplaySize( 240, 320 );
    setScale( 1 );
    
    m_area = QRectF( -126, -168, 252, 344 );

    m_pinCS.setLabelText( "CS" );
    m_pinCS.setInputHighV( 2.31 );
    m_pinCS.setInputLowV( 0.99 );
    m_pinRst.setLabelText( "Rst" );
    m_pinRst.setInputHighV( 2.31 );
    m_pinRst.setInputLowV( 0.99 );
    m_pinDC.setLabelText( "D/C" );
    m_pinDC.setInputHighV( 2.31 );
    m_pinDC.setInputLowV( 0.99 );
    m_pinMosi.setLabelText( "Mosi" );
    m_pinMosi.setInputHighV( 2.31 );
    m_pinMosi.setInputLowV( 0.99 );
    m_pinSck.setLabelText( "SCK" );
    m_pinSck.setInputHighV( 2.31 );
    m_pinSck.setInputLowV( 0.99 );
    //m_pinMiso.setLabelText( "Miso" );

    m_pin.resize( 5 );
    m_pin[0] = &m_pinCS;
    m_pin[1] = &m_pinRst;
    m_pin[2] = &m_pinDC;
    m_pin[3] = &m_pinMosi;
    m_pin[4] = &m_pinSck;

    m_clkPin = &m_pinSck;

    Simulator::self()->addToUpdateList( this );
    
    setLabelPos(-32,-180, 0);
    setShowId( true );

    Ili9341::initialize();
}
Ili9341::~Ili9341(){}

void Ili9341::stamp()
{
    m_pinSck.changeCallBack( this ); // Register for Scl changes callback
    m_pinCS.changeCallBack( this ); // Register for CS changes callback
    m_pinRst.changeCallBack( this ); // Register for Rst changes callback
}

void Ili9341::initialize()
{
    clearDDRAM();
    displayReset();
    Ili9341::updateStep();
}

void Ili9341::updateStep()
{
    update();
}

void Ili9341::displayReset()
{
    TftController::displayReset();

    m_inBit  = 0;
    m_dataBytes = 2; //16bit mode
}

void Ili9341::voltChanged()
{
    bool ret = false;
    if( !m_pinRst.getInpState() )            // Reset Pin is Low
    {
        displayReset();
        ret = true;
    }
    if( m_pinCS.getInpState() )            // Cs Pin High: Lcd not selected
    {
        m_rxReg = 0;                       // Initialize serial buffer
        m_inBit  = 0;
        ret = true;
    }
    updateClock();

    if( m_clkState != Clock_Rising ) ret = true;
    if( ret ) return;

    m_rxReg &= ~1; //Clear bit 0
    if( m_pinMosi.getInpState() ) m_rxReg |= 1;

    if( m_inBit >= 7 )
    {
        if( m_pinDC.getInpState() ) dataReceived();
        else                        commandReceived();
        m_inBit = 0;
    }else{
        m_rxReg <<= 1;
        m_inBit++;
    }
}

void Ili9341::writeRam()
{
    m_data = (m_data<<8) | m_rxReg;
    m_dataIndex++;

    if( m_dataIndex >= m_dataBytes )       // 16/18 bits ready
    {
        m_dataIndex = 0;

        uint r,g,b;
        if( m_dataBytes == 2 ) // 16 bits format: RRRRRGGGGGGBBBBB
        {
            r = (m_data & 0b1111100000000000)<<8;
            g = (m_data & 0b0000011111100000)<<5;
            b = (m_data & 0b0000000000011111)<<3;
        }
        else // 18 bits format: RRRRRR00GGGGGG00BBBBBB00
        {
            r = (m_data & 0b111111000000000000000000);
            g = (m_data & 0b000000001111110000000000);
            b = (m_data & 0b000000000000000011111100);
        }
        m_data = r+g+b;
        TftController::writeRam();
        m_data = 0;
    }
}

void Ili9341::setPixelMode()
{
    int mode = m_rxReg & 1<<4;
    m_dataBytes = mode ? 2 : 3;
}
