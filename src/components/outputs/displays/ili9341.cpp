/***************************************************************************
 *   Copyright (C) 2019 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "ili9341.h"
#include "itemlibrary.h"

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
       : St77xx( type, id )
{
    m_isILI = true;

    m_pinRS.setId( id+"-PinRst");  // Compatibility with older versions
    m_pinDI.setId( id+"-PinMosi"); // Compatibility with older versions
    m_pinCK.setId( id+"-PinSck");  // Compatibility with older versions

    m_maxWidth  = 240;
    m_maxHeight = 320;
    setDisplaySize( m_maxWidth, m_maxHeight );
    setScale( 1 );

    Ili9341::initialize();
}
Ili9341::~Ili9341(){}

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
