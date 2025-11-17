/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "resistor.h"
#include "itemlibrary.h"
#include "pin.h"

#include "doubleprop.h"

#define tr(str) simulideTr("Resistor",str)

Component* Resistor::construct( QString type, QString id )
{ return new Resistor( type, id ); }

LibraryItem* Resistor::libraryItem()
{
    return new LibraryItem(
        tr("Resistor"),
        "Resistors",
        "resistor.png",
        "Resistor",
        Resistor::construct);
}

Resistor::Resistor( QString type, QString id )
        : Comp2Pin( type, id )
        , eResistor( id )
{
    m_ePin[0] = m_pin[0];
    m_ePin[1] = m_pin[1];

    m_pin[0]->setLength( 4.4 );
    m_pin[1]->setLength( 4.4 );

    addPropGroup( { tr("Main"), {
        new DoubProp<Resistor>("Resistance", tr("Resistance"), "Ω"
                              , this, &Resistor::resistance, &Resistor::setResistance )
    }, 0 } );

    setShowProp("Resistance");
    setPropStr("Resistance", "100");
}
Resistor::~Resistor(){}

void Resistor::drawAnsi( QPainter* p, int x, int y, double sX, double sY )
{
    QPen pen = p->pen();
    pen.setWidth(2);
    p->setPen( pen );
    p->drawLine( QPointF( sX*(-12+x),sY*( 0)+y ), QPointF( sX*(-10+x),sY*(-4)+y ) );
    p->drawLine( QPointF( sX*(-10+x),sY*(-4)+y ), QPointF( sX*( -6+x),sY*( 4)+y ) );
    p->drawLine( QPointF( sX*( -6+x),sY*( 4)+y ), QPointF( sX*( -2+x),sY*(-4)+y ) );
    p->drawLine( QPointF( sX*( -2+x),sY*(-4)+y ), QPointF( sX*(  2+x),sY*( 4)+y ) );
    p->drawLine( QPointF( sX*(  2+x),sY*( 4)+y ), QPointF( sX*(  6+x),sY*(-4)+y ) );
    p->drawLine( QPointF( sX*(  6+x),sY*(-4)+y ), QPointF( sX*( 10+x),sY*( 4)+y ) );
    p->drawLine( QPointF( sX*( 10+x),sY*( 4)+y ), QPointF( sX*( 12+x),sY*( 0)+y ) );
}

void Resistor::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );

    if( m_ansiSymbol ) Resistor::drawAnsi( p, 0, 0 );
    else               p->drawRect( m_area );

    Component::paintSelected( p );
}
