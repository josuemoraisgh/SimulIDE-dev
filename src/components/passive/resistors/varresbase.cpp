/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <math.h>
#include <QAbstractSlider>
#include <QGraphicsProxyWidget>

#include "varresbase.h"
#include "propdialog.h"
#include "simulator.h"
#include "circuit.h"
#include "pin.h"
#include "label.h"

VarResBase::VarResBase( QString type, QString id )
          : Dialed( type, id )
          , eResistor( id )
{
    m_graphical = true;

    m_pin.resize(2);
    m_ePin[0] = m_pin[0] = new Pin( 180, QPoint(-16, 0 ), id+"-lPin", 0, this);
    m_ePin[1] = m_pin[1] = new Pin( 0,   QPoint( 16, 0 ), id+"-rPin", 1, this);

    m_pin[0]->setLength( 4.4 );
    m_pin[1]->setLength( 4.4 );

    m_idLabel->setPos(-12,-24);
    setLabelPos(-16,-24, 0);
    setValLabelPos(-16, 30, 0);
}
VarResBase::~VarResBase(){}

void VarResBase::initialize()
{
    m_needUpdate = true;
}

void VarResBase::updateProxy()
{
    m_proxy->setPos( QPoint(-m_dialW.width()/2, 7) );
}
