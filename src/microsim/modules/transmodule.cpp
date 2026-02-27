/***************************************************************************
 *   Copyright (C) 2026 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "transmodule.h"
#include "circuitwidget.h"
#include "serialmon.h"

TransModule::TransModule( QString name )
{
    m_moduleName = name.split("-").last();
    m_monitor = nullptr;
    m_monOpen = false;
}
TransModule::~TransModule(){}

void TransModule::openMonitor( QString id, int num, bool send )
{
    if( !m_monitor )
        m_monitor = new SerialMonitor( CircuitWidget::self(), this, send );

    //if( num > 0 ) id.append( QString::number(num) );
    setMonitorTittle( id );
    m_monitor->show();
    m_monOpen = true;
}

void TransModule::closeMonitor()
{
    if( m_monitor ) m_monitor->close();
}

void TransModule::printOut( uint8_t data )
{
    if( m_monitor ) m_monitor->printOut( data );
}

void TransModule::printIn( uint8_t data )
{
    if( m_monitor ) m_monitor->printIn( data );
}

void TransModule::setMonitorTittle( QString t )
{
    if( m_monitor ) m_monitor->setWindowTitle( t+"-"+m_moduleName );
}

void TransModule::monitorClosed()
{
    m_monOpen = false;
}

