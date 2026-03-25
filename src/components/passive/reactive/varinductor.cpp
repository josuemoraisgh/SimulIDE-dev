/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>
#include <QGraphicsProxyWidget>

#include "varinductor.h"
#include "itemlibrary.h"
#include "simulator.h"
#include "resistor.h"
#include "pin.h"

#include "doubleprop.h"
#include "propdialog.h"

#define tr(str) simulideTr("VarInductor",str)

Component* VarInductor::construct( QString type, QString id )
{ return new VarInductor( type, id ); }

LibraryItem* VarInductor::libraryItem()
{
    return new LibraryItem(
        tr("Variable Inductor"),
        "Reactive",
        "varinductor.png",
        "VarInductor",
        VarInductor::construct);
}

VarInductor::VarInductor( QString type, QString id )
            : Dialed( type, id  )
            , eReactive( id )
{
    m_area = QRectF(-10,-8, 18, 14 );

    m_pin.resize(2);
    m_pin[0] = new Pin( 180, QPoint(-16, 0 ), id+"-lPin", 0, this);
    m_pin[1] = new Pin( 0,   QPoint( 16, 0 ), id+"-rPin", 1, this);

    m_pin[0]->setLength( 4 );
    m_pin[1]->setLength( 4 );

    m_ePin[0] = m_pin[0];
    setNumEpins( 3 );

    m_resistor = new eResistor( m_elmId+"-resistor");
    m_resistor->setResistance( 1e-6 );
    m_resistor->setEpin( 0, m_ePin[2] );
    m_resistor->setEpin( 1, m_pin[1] );

    setValLabelPos( -8,-18, 0 );
    setLabelPos(-26,-28, 0);

    m_maxVal = 10e-3;

    setVal( 0 );

    addPropGroup( { tr("Main"), {
        new DoubProp<VarInductor>("Min_Inductance", tr("Minimum Inductance"), "mH"
                                 , this, &VarInductor::minVal, &VarInductor::setMinVal ),

        new DoubProp<VarInductor>("Max_Inductance", tr("Maximum Inductance"), "mH"
                                 , this, &VarInductor::maxVal, &VarInductor::setMaxVal ),

        new DoubProp<VarInductor>("Inductance", tr("Current Value"), "mH"
                                 , this, &VarInductor::getVal, &VarInductor::setVal ),

        new DoubProp<VarInductor>("Resistance" , tr("Resistance"), "µΩ"
                                  , this, &VarInductor::resist , &VarInductor::setResist ),

        new DoubProp<VarInductor>("InitVolt"   , tr("Initial Voltage"), "V"
                                  , this, &VarInductor::initVolt, &VarInductor::setInitVolt ),

        new DoubProp<VarInductor>("Dial_Step", tr("Dial Step"), "mH"
                                 , this, &VarInductor::getStep, &VarInductor::setStep ),
    },0 } );

    addPropGroup( { tr("Dial"), Dialed::dialProps(), groupNoCopy } );
}
VarInductor::~VarInductor()
{
    delete m_resistor;
}

void VarInductor::initialize()
{
    m_crashed = false;
    m_warning = false;

    m_midEnode = new eNode( m_elmId+"-mideNode");

    m_needUpdate = true;
}

void VarInductor::stamp()
{
    m_ePin[1]->setEnode( m_midEnode );
    m_ePin[2]->setEnode( m_midEnode );

    if( m_pin[0]->isConnected() && m_pin[1]->isConnected() )
        eReactive::stamp();
}

void VarInductor::updateStep()
{
    if( !m_needUpdate ) return;
    m_needUpdate = false;

    m_inductance = Dialed::m_value;

    updtReactStep();
    if( m_propDialog ) m_propDialog->updtValues();
    else setValLabelText( getPropStr( showProp() ) );

    update();
}

void VarInductor::setResist( double resist )
{
    m_resistor->setResSafe( resist );
}

void VarInductor::updateProxy()
{
    m_proxy->setPos( QPoint(-m_dialW.width()/2, 7) );
}

void VarInductor::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    if( m_hidden ) return;
    Component::paint( p, o, w );

    p->drawLine(-6, 6, 8,-8 );
    p->drawLine( 8,-6, 8,-8 );
    p->drawLine( 8,-8, 6,-8 );

    QPen pen = p->pen();
    pen.setWidth( 2 );
    p->setPen( pen );

    p->drawArc( QRectF(-12,-4.5, 10, 10 ),-45*16 , 220*16 );
    p->drawArc( QRectF( -5,-4.5, 10, 10 ), 225*16,-270*16 );
    p->drawArc( QRectF(  2,-4.5, 10, 10 ), 225*16,-220*16 );

    Component::paintSelected( p );
}
