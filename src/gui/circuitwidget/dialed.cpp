/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <math.h>

#include <QAbstractSlider>
#include <QGraphicsProxyWidget>
#include <QCoreApplication>

#include "dialed.h"
#include "dialwidget.h"
#include "propdialog.h"
#include "simulator.h"
#include "circuit.h"

#include "boolprop.h"
#include "doubleprop.h"

#define tr(str) simulideTr("Dialed",str)

Dialed::Dialed( QString type, QString id )
      : Component( type, id )
{
    m_areaDial = QRectF(-11, 8, 22, 22 );
    m_areaComp = QRectF(-11,-11, 22, 16 );
    m_area     = m_areaComp;
    m_graphical = true;

    setValLabelPos( 15,-20, 0 );
    setLabelPos(-16,-40, 0);

    m_proxy = Circuit::self()->addWidget( &m_dialW );
    m_proxy->setParentItem( this );
    m_proxy->setPos( QPoint(-m_dialW.width()/2, 7) );
    m_slider = false;

    m_minVal = 0;
    m_maxVal = 1000;
    m_step = 0;

    m_blocked = false;

    QObject::connect( m_dialW.dial(), &QAbstractSlider::valueChanged, [=](int v){ dialChanged(v); } );

    Simulator::self()->addToUpdateList( this );
}
Dialed::~Dialed() {}

QList<ComProperty*> Dialed::dialProps()
{
    return {
        new BoolProp<Dialed>( "Slider", tr("Slider"),""
                            , this, &Dialed::slider, &Dialed::setSlider ),
        new DoubProp<Dialed>( "Scale" , tr("Scale") ,""
                            , this, &Dialed::scale , &Dialed::setScale )
    };
}

void Dialed::setMinVal( double min )
{
    if( min < 1e-12    ) min = 1e-12;
    if( min > m_maxVal ) min = m_maxVal;
    m_minVal = min;

    updtValue();
}

void Dialed::setMaxVal( double max )
{
    if( max < 1e-12    ) max = 1e-12;
    if( max < m_minVal ) max = m_minVal;
    m_maxVal = max;

    updtValue();
}

void Dialed::setVal( double val )
{
    m_value = val;
    updtValue();
}

void Dialed::dialChanged( int val ) // Called when dial is rotated
{
    if( m_blocked ) return;
    m_value = m_minVal+val*( m_maxVal-m_minVal)/1000;
    if( m_step > 0 ) m_value = round( m_value/m_step )*m_step;

    m_needUpdate = true;
    if( !Simulator::self()->isRunning() ) updateStep();
}

void Dialed::updtValue()
{
    if     ( m_value > m_maxVal ) m_value = m_maxVal;
    else if( m_value < m_minVal ) m_value = m_minVal;

    m_blocked = true;
    double dialV = (m_value-m_minVal)*1000/(m_maxVal-m_minVal);
    m_dialW.setValue( dialV );
    m_blocked = false;

    if( m_propDialog ) m_propDialog->updtValues();
}

void Dialed::setLinkedValue( double v, int i )
{
    if( i == 0 ) m_dialW.setValue( v );
    else         setVal( v );
}

void Dialed::setSlider( bool s )
{
    m_slider = s;

    /// QObject::disconnect( m_dialW.dial(), &QAbstractSlider::valueChanged );

    m_dialW.setType( s ? 1: 0 );

    QObject::connect( m_dialW.dial(), &QAbstractSlider::valueChanged, [=](int v){ dialChanged(v); } );

    updateProxy();
}

void Dialed::setScale( double s )
{
    if( s <= 0 ) return;
    m_dialW.setScale(s);
    updateProxy();
}

void Dialed::setHidden( bool hide, bool hidArea, bool hidLabel )
{
    Component::setHidden( hide, hidArea, hidLabel );
    if  ( hidArea ) m_area = QRectF( 0, 0,-4,-4 );
    else if( hide ) m_area = m_areaDial;
    else            m_area = m_areaComp;

    m_proxy->setFlag( QGraphicsItem::ItemStacksBehindParent, hide && !hidArea );
}
