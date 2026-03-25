/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>
#include <QGraphicsProxyWidget>

#include "varcapacitor.h"
#include "itemlibrary.h"
#include "simulator.h"
#include "resistor.h"
#include "pin.h"

#include "doubleprop.h"
#include "propdialog.h"

#define tr(str) simulideTr("VarCapacitor",str)

Component* VarCapacitor::construct( QString type, QString id )
{ return new VarCapacitor( type, id ); }

LibraryItem* VarCapacitor::libraryItem()
{
    return new LibraryItem(
        tr("Variable Capacitor"),
        "Reactive",
        "varcapacitor.png",
        "VarCapacitor",
        VarCapacitor::construct);
}

VarCapacitor::VarCapacitor( QString type, QString id )
            : Dialed( type, id  )
            , eReactive( id )
{
    m_area = QRectF(-10,-8, 18, 14 );

    m_pin.resize(2);
    m_pin[0] = new Pin( 180, QPoint(-16, 0 ), id+"-lPin", 0, this);
    m_pin[1] = new Pin( 0,   QPoint( 16, 0 ), id+"-rPin", 1, this);

    m_pin[0]->setLength( 12 );
    m_pin[1]->setLength( 12 );

    m_ePin[0] = m_pin[0];
    setNumEpins( 3 );

    m_resistor = new eResistor( m_elmId+"-resistor");
    m_resistor->setResistance( 1e-6 );
    m_resistor->setEpin( 0, m_ePin[2] );
    m_resistor->setEpin( 1, m_pin[1] );

    setValLabelPos( -8,-18, 0 );
    setLabelPos(-26,-28, 0);

    m_maxVal = 10e-6;

    setVal( 0 );

    addPropGroup( { tr("Main"), {
        new DoubProp<VarCapacitor>("Min_Capacitance", tr("Minimum Capacitance"), "µF"
                                 , this, &VarCapacitor::minVal, &VarCapacitor::setMinVal ),

        new DoubProp<VarCapacitor>("Max_Capacitance", tr("Maximum Capacitance"), "µF"
                                 , this, &VarCapacitor::maxVal, &VarCapacitor::setMaxVal ),

        new DoubProp<VarCapacitor>("Capacitance", tr("Current Value"), "µF"
                                 , this, &VarCapacitor::getVal, &VarCapacitor::setVal ),

        new DoubProp<VarCapacitor>("Resistance" , tr("Resistance"), "µΩ"
                                  , this, &VarCapacitor::resist , &VarCapacitor::setResist ),

        new DoubProp<VarCapacitor>("InitVolt"   , tr("Initial Voltage"), "V"
                                  , this, &VarCapacitor::initVolt, &VarCapacitor::setInitVolt ),

        new DoubProp<VarCapacitor>("Dial_Step", tr("Dial Step"), "µF"
                                 , this, &VarCapacitor::getStep, &VarCapacitor::setStep ),
    },0 } );

    addPropGroup( { tr("Dial"), Dialed::dialProps(), groupNoCopy } );
}
VarCapacitor::~VarCapacitor()
{
    delete m_resistor;
}

void VarCapacitor::initialize()
{
    m_crashed = false;
    m_warning = false;

    m_midEnode = new eNode( m_elmId+"-mideNode");

    m_needUpdate = true;
}

void VarCapacitor::stamp()
{
    m_ePin[1]->setEnode( m_midEnode );
    m_ePin[2]->setEnode( m_midEnode );

    if( m_pin[0]->isConnected() && m_pin[1]->isConnected() )
        eReactive::stamp();
}

void VarCapacitor::updateStep()
{
    if( !m_needUpdate ) return;
    m_needUpdate = false;

    m_capacitance = Dialed::m_value;

    updtReactStep();
    if( m_propDialog ) m_propDialog->updtValues();
    else setValLabelText( getPropStr( showProp() ) );

    update();
}

void VarCapacitor::setResist( double resist )
{
    m_resistor->setResSafe( resist );
}

void VarCapacitor::updateProxy()
{
    m_proxy->setPos( QPoint(-m_dialW.width()/2, 7) );
}

void VarCapacitor::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    if( m_hidden ) return;
    Component::paint( p, o, w );

    p->drawLine(-8, 6, 8,-6 );
    p->drawLine( 9,-5, 9,-7 );
    p->drawLine( 9,-7, 7,-7 );

    QPen pen = p->pen();
    pen.setWidth(3);
    p->setPen(pen);

    p->drawLine(-3,-6,-3, 6 );
    p->drawLine( 3,-6, 3, 6 );

    Component::paintSelected( p );
}
