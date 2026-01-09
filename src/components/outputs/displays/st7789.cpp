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
    m_maxWidth  = 240;
    m_maxHeight = 320;
    setDisplaySize( m_maxWidth, m_maxHeight );
    setScale( 1 );
}
St7789::~St7789(){}
