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
      , Spi3Pins( id, this )
{
    m_graphical = true;

    m_maxWidth  = 132;
    m_maxHeight = 162;
    setDisplaySize( m_maxWidth, m_maxHeight );
    updateSize();

    m_pin.resize( 4 );
    m_pin[0] = &m_pinCS;
    m_pin[1] = &m_pinDI;
    m_pin[2] = &m_pinCK;
    m_pin[3] = &m_pinRS;

    setLabelPos(-m_width/2+16,-m_height/2-20, 0);
    setShowId( true );

    addPropGroup( { tr("Main"), {
        new IntProp<St7735>("Width", tr("Width"), "_px"
                             , this, &St7735::width, &St7735::setWidth, propNoCopy,"uint" ),

        new IntProp<St7735>("Height", tr("Height"), "_px"
                             ,this,&St7735::height, &St7735::setHeight, propNoCopy,"uint" ),
    }, 0} );
}
St7735::~St7735(){}

void St7735::displayReset()
{
    TftController::displayReset();
    Spi3Pins::reset();
    //m_dataBytes = 2; //16bit mode
}

void St7735::endTransaction()
{
    m_rxReg = m_buffer;
    if( m_isData ) dataReceived();
    else           commandReceived();
}

void St7735::updateSize()
{
    m_pinRS.setY( m_height/2+ 24 );
    m_pinCS.setY( m_height/2+ 24 );
    m_pinDI.setY( m_height/2+ 24 );
    m_pinCK.setY( m_height/2+ 24 );

    m_pinRS.isMoved();
    m_pinCS.isMoved();
    m_pinDI.isMoved();
    m_pinCK.isMoved();

    St77xx::updateSize();
}
