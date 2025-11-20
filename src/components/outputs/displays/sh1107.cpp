/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "sh1107.h"
#include "itemlibrary.h"

#include "doubleprop.h"
#include "intprop.h"

#define tr(str) simulideTr("Sh1107",str)

Component* Sh1107::construct( QString type, QString id )
{ return new Sh1107( type, id ); }

LibraryItem* Sh1107::libraryItem()
{
    return new LibraryItem(
        "Sh1107",
        "Displays",
        "sh1107.png",
        "Sh1107",
        Sh1107::construct );
}

Sh1107::Sh1107( QString type, QString id )
       : OledController( type, id )
{
    m_address = m_cCode = 0b00111100; // 0x3A - 60

    setColorStr("White");

    setSize( 128, 128 );
    setLabelPos(-32,-92, 0);

    addPropGroup( { tr("Main"), {
        new IntProp <Sh1107>("Width", tr("Width"), "_px"
                             , this, &Sh1107::width, &Sh1107::setWidth, propNoCopy,"uint" ),

        new IntProp <Sh1107>("Height", tr("Height"), "_px"
                             ,this,&Sh1107::height, &Sh1107::setHeight, propNoCopy,"uint" ),
    }, 0} );

    addPropGroup( { tr("I2C"), {
        new IntProp <Sh1107>("Control_Code", tr("I2C Address"), ""
                             , this, &Sh1107::cCode, &Sh1107::setCcode,0,"uint" ),

        new DoubProp<Sh1107>("Frequency",tr("I2C Frequency"), "_kHz"
                             , this, &Sh1107::freqKHz, &Sh1107::setFreqKHz ),
    }, 0} );
}
Sh1107::~Sh1107(){}

void Sh1107::proccessCommand()
{
    m_lastCommand = m_rxReg;
    m_readIndex = 0;
    m_readBytes = 0;

    if( m_rxReg < 0x17 )
    {
        if( m_addrMode != PAGE_ADDR_MODE ) return;

        if( m_rxReg < 0x10 )                              // Lower Colum Start Address for Page Addresing mode
            m_addrX = (m_addrX & 0xF0) | (m_rxReg & 0x0F);
        else                                              // Higher Colum Start Address for Page Addresing mode
            m_addrX = (m_addrX & 0x0F) | ((m_rxReg & 0x0F) << 4);

        if( m_addrX >= m_maxWidth) m_addrX -= m_maxWidth;
    }
    else if( m_rxReg>=0x40 && m_rxReg<=0x7F )            // Display Start Line
    {
        m_ramOffset = m_rxReg & m_lineMask;
    }
    else if( m_rxReg>=0xB0 && m_rxReg<=0xB7 )            // Page Start Address for Page Addresing mode
    {
        if( m_addrMode == PAGE_ADDR_MODE ) m_addrY = m_rxReg & m_rowMask;
    }
    else{
    switch( m_rxReg )
    {
        case 0x20: m_addrMode = PAGE_ADDR_MODE; break; // Memory Addressing Mode
        case 0x21: m_addrMode = COLU_ADDR_MODE; break; // Column Address (Start-End)
        case 0x81: m_readBytes = 1;    break; // Contrast Control
        case 0xA0: m_remap = false;    break; // Segment Re-map OFF
        case 0xA1: m_remap = true;     break; // Segment Re-map ON
        case 0xA4: m_dispFull = false; break; // Entire Display Off
        case 0xA5: m_dispFull = true;  break; // Entire Display ON
        case 0xA6: m_dispInv  = false; break; // Inverse Display OFF
        case 0xA7: m_dispInv  = true;  break; // Inverse Display ON
        case 0xA8: m_readBytes = 1;    break; // Multiplex Ratio
        case 0xAE: reset();            break; // Display OFF
        case 0xAF: m_dispOn = true;    break; // Display ON
        case 0xC0: m_scanInv = false;  break; // COM Output Scan Inverted OFF
        case 0xC8: m_scanInv = true;   break; // COM Output Scan Inverted ON
        case 0xD3: m_readBytes = 1;    break; // Display Offset
        case 0xD5: m_readBytes = 1;    break; // Display Clock Divide Ratio/Oscillator Frequency
        case 0xD9: m_readBytes = 1;    break; // Precharge
        case 0xDA: m_readBytes = 1;    break; // COM Pins Hardware Configuration
        case 0xDB: m_readBytes = 1;    break; // VCOM DETECT
        case 0xDC: m_readBytes = 1;    break; // Display Start Line
        }
    }
}

void Sh1107::parameter()
{
    m_readIndex++;
    if( m_readIndex > m_readBytes ) return;
    if( m_readIndex == m_readBytes ) m_readBytes = 0;

    switch( m_lastCommand )
    {
        case  0xA8: // Multiplex Ratio
        {
            uint8_t muxRatio = m_rxReg & m_lineMask;
            if( muxRatio > 14 ) m_mr = muxRatio;
        }break;
        case 0xD3: m_dispOffset = m_rxReg & m_lineMask; break; // Display Offset Set vertical shift by COM from 0d~63d
        case 0xDC: m_ramOffset  = m_rxReg & m_lineMask; break; // Display Start Line
    }
}
