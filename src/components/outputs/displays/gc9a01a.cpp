/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "gc9a01a.h"
#include "itemlibrary.h"
#include "circuit.h"

#include "intprop.h"

#define tr(str) simulideTr("GC9A01A",str)

Component* GC9A01A::construct( QString type, QString id )
{ return new GC9A01A( type, id ); }

LibraryItem* GC9A01A::libraryItem()
{
    return new LibraryItem(
        "GC9A01A" ,
        "Displays",
        "gc9a01a.png",
        "GC9A01A",
        GC9A01A::construct );
}

GC9A01A::GC9A01A( QString type, QString id )
       : St77xx( type, id )
{
    m_graphical = true;

    m_maxWidth  = 240;
    m_maxHeight = 240;
    setDisplaySize( m_maxWidth, m_maxHeight );
    setScale( 1 );

    setLabelPos(-m_width/2+16,-m_height/2-20, 0);
    setShowId( true );

    remProperty("Width");
}
GC9A01A::~GC9A01A(){}

void GC9A01A::displayReset()
{
    TftController::displayReset();
    Spi5Pins::reset();
    //m_dataBytes = 2; //16bit mode
}

void GC9A01A::endTransaction()
{
    m_rxReg = m_buffer;
    if( m_isData ) dataReceived();
    else           commandReceived();
}

void GC9A01A::setHeight( int h )
{
    if     ( h > m_maxHeight ) h = m_maxHeight;
    else if( h <  16         ) h = 16;

    if( m_height == h ) return;

    //m_rows = h/8;
    setDisplaySize( h, h );
    updateSize();
}

void GC9A01A::paint( QPainter* p, const QStyleOptionGraphicsItem*, QWidget* )
{
    p->setRenderHint( QPainter::Antialiasing, true );
    QPen pen( Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    p->setPen( pen );

    p->setBrush( QColor(50, 70, 100) );
    p->drawRoundedRect( QRectF(-m_scaledWidth*3/8, 0, m_scaledWidth*6/8, m_scaledHeight/2+12+4), 5, 5 );
    p->drawEllipse( QRectF(-m_scaledWidth/2-6,-m_scaledHeight/2-6, m_scaledWidth+12, m_scaledHeight+12 ) );

    QRectF imgRect = QRectF(-m_scaledWidth/2,-m_scaledHeight/2, m_scaledWidth, m_scaledHeight );

    if( !m_dispOn ){                // Display Off
        p->setPen( Qt::NoPen );
        p->setBrush( Qt::black );
    }else{
        printImage();
        p->setBrush( QBrush( m_image ) );
    }
    p->drawEllipse( imgRect );
    Component::paintSelected( p );
}
