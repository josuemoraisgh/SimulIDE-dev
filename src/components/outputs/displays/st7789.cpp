/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "st7789.h"
#include "itemlibrary.h"

#include "intprop.h"

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
      , Spi5Pins( id, this )
{
    m_graphical = true;

    m_maxWidth  = 240;
    m_maxHeight = 320;
    setDisplaySize( m_maxWidth, m_maxHeight );
    updateSize();

    m_pin.resize( 6 );
    m_pin[0] = &m_pinDC;
    m_pin[1] = &m_pinCS;
    m_pin[2] = &m_pinDI;
    m_pin[3] = &m_pinCK;
    m_pin[4] = &m_pinRS;
    m_pin[5] = &m_pinDO;

    setLabelPos(-m_width/2+16,-m_height/2-20, 0);
    setShowId( true );

    addPropGroup( { tr("Main"), {
        new IntProp<St7789>("Width", tr("Width"), "_px"
                             , this, &St7789::width, &St7789::setWidth, propNoCopy,"uint" ),

        new IntProp<St7789>("Height", tr("Height"), "_px"
                             ,this,&St7789::height, &St7789::setHeight, propNoCopy,"uint" ),
    }, 0} );
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
    m_rxReg = m_buffer;
    if( m_isData ) dataReceived();
    else           commandReceived();
}

void St7789::updateSize()
{
    m_pinDC.setY( m_height/2+ 24 );
    m_pinRS.setY( m_height/2+ 24 );
    m_pinCS.setY( m_height/2+ 24 );
    m_pinDI.setY( m_height/2+ 24 );
    m_pinCK.setY( m_height/2+ 24 );
    m_pinDO.setY( m_height/2+ 24 );

    m_pinDC.isMoved();
    m_pinRS.isMoved();
    m_pinCS.isMoved();
    m_pinDI.isMoved();
    m_pinCK.isMoved();
    m_pinDO.isMoved();

    St77xx::updateSize();
}
