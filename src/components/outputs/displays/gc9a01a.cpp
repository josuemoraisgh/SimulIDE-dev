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
       , Spi5Pins( id, this )
{
    m_graphical = true;

    m_maxWidth  = 240;
    m_maxHeight = 240;
    setDisplaySize( m_maxWidth, m_maxHeight );
    updateSize();

    m_pin.resize( 6 );
    m_pin[0] = &m_pinDC;
    m_pin[1] = &m_pinCS;
    m_pin[2] = &m_pinDI;
    m_pin[3] = &m_pinCK;
    m_pin[4] = &m_pinRS;
    m_pin[5] = &m_pinDO;

    m_pinDC.setX(-20 );
    m_pinRS.setX(-12 );
    m_pinCS.setX( -4 );
    m_pinDI.setX(  4 );
    m_pinCK.setX( 12 );
    m_pinDO.setX( 20 );

    setLabelPos(-m_width/2+16,-m_height/2-20, 0);
    setShowId( true );

    addPropGroup( { tr("Main"), {
        new IntProp<GC9A01A>("Height", tr("Size"), "_px"
                             , this, &GC9A01A::height, &GC9A01A::setHeight, propNoCopy,"uint" ),

        //new IntProp<GC9A01A>("Height", tr("Height"), "_px"
        //                     ,this,&GC9A01A::height, &GC9A01A::setHeight, propNoCopy,"uint" ),
    }, 0} );
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
    if( h > m_height ) h += 8;
    if     ( h > m_maxHeight ) h = m_maxHeight;
    else if( h <  16         ) h = 16;

    h = (h/8)*8;
    if( m_height == h ) return;

    m_rows = h/8;

    setDisplaySize( h, h );
    updateSize();
}

void GC9A01A::updateSize()
{
    m_pinDC.setY( m_height/2+ 28 );
    m_pinRS.setY( m_height/2+ 28 );
    m_pinCS.setY( m_height/2+ 28 );
    m_pinDI.setY( m_height/2+ 28 );
    m_pinCK.setY( m_height/2+ 28 );
    m_pinDO.setY( m_height/2+ 28 );

    m_pinDC.isMoved();
    m_pinRS.isMoved();
    m_pinCS.isMoved();
    m_pinDI.isMoved();
    m_pinCK.isMoved();
    m_pinDO.isMoved();

    m_area = QRectF(-m_width/2-6,-m_height/2-6, m_width+12, m_height+12+15);
    Circuit::self()->update();
}

void GC9A01A::paint( QPainter* p, const QStyleOptionGraphicsItem*, QWidget* )
{
    p->setRenderHint( QPainter::Antialiasing, true );
    QPen pen( Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    p->setPen( pen );

    p->setBrush( QColor(50, 70, 100) );
    p->drawRoundedRect( QRectF(-m_width*3/8, 0, m_width*6/8, m_height/2+12+8), 5, 5 );
    p->drawEllipse( QRectF(-m_width/2-6,-m_height/2-6, m_width+12, m_height+12 ) );

    QRectF imgRect = QRectF(-m_width/2,-m_height/2, m_width, m_height );

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
