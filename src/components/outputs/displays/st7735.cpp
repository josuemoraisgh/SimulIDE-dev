/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "st7735.h"
#include "itemlibrary.h"

#include "intprop.h"

#define tr(str) simulideTr("St7735",str)

Component* St7735::construct( QString type, QString id )
{ return new St7735( type, id ); }

LibraryItem* St7735::libraryItem()
{
    return new LibraryItem(
        "St7735" ,
        "Displays",
        "st7735.png",
        "St7735",
        St7735::construct );
}

St7735::St7735( QString type, QString id )
      : St77xx( type, id )
{
    m_graphical = true;

    m_maxWidth  = 132;
    m_maxHeight = 162;
    setDisplaySize( m_maxWidth, m_maxHeight );
    setScale( 1 );
    updateSize();

    setLabelPos(-m_width/2+16,-m_height/2-20, 0);
    setShowId( true );
}
St7735::~St7735(){}

void St7735::displayReset()
{
    TftController::displayReset();
    /// Spi3Pins::reset();
    //m_dataBytes = 2; //16bit mode
}

void St7735::endTransaction()
{
    m_rxReg = m_buffer;
    if( m_isData ) dataReceived();
    else           commandReceived();
}
