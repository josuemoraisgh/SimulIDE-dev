/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "st7789.h"
#include "itemlibrary.h"

#define tr(str) simulideTr("St7789",str)

Component* St7789::construct( QString type, QString id )
{ return new St7789( type, id ); }

LibraryItem* St7789::libraryItem()
{
    return new LibraryItem(
        "St7789" ,
        "Displays",
        "st7789.png",
        "St7789",
        St7789::construct );
}

St7789::St7789( QString type, QString id )
      : St77xx( type, id )
{
    m_graphical = true;

    m_maxWidth  = 240;
    m_maxHeight = 320;
    setDisplaySize( m_maxWidth, m_maxHeight );
    setScale( 1 );

    setLabelPos(-m_width/2+16,-m_height/2-20, 0);
    setShowId( true );
}
St7789::~St7789(){}

void St7789::displayReset()
{
    TftController::displayReset();
    Spi5Pins::reset();
    //m_dataBytes = 2; //16bit mode
}

void St7789::endTransaction()
{
    Spi5Pins::endTransaction();
    m_rxReg = m_buffer;
    if( m_isData ) dataReceived();
    else           commandReceived();
}
