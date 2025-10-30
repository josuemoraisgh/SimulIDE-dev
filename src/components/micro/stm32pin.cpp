/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>
#include <QDebug>

#include "stm32pin.h"

Stm32Pin::Stm32Pin( uint8_t port, int i, QString id, QemuDevice* mcu )
        : IoPin( 0, QPoint(0,0), id, 0, mcu, input )
        , QemuModule( mcu, i )
{
    //m_id     = id;
    m_port   = port;

    m_pullAdmit = 1e5; // 10k

    double vdd = 3.3; //m_port->getMcu()->vdd();
    m_outHighV = vdd;
    m_inpHighV = vdd/2;
    m_inpLowV  = vdd/2;

    m_pinMask = 1<<i;
}
Stm32Pin::~Stm32Pin() {}

void Stm32Pin::initialize()
{
    //m_outCtrl = false;
    //m_dirCtrl = false;
    //m_isAnalog = false;
    ////m_portState = false;
    //
    //double vdd = 3.3; //m_port->getMcu()->vdd();
    //m_outHighV = vdd;
    //m_inpHighV = vdd/2;
    //m_inpLowV  = vdd/2;
    //
    IoPin::initialize();
}

void Stm32Pin::stamp()
{
    IoPin::stamp();

    m_alternate = false;
    m_analog = false;
    m_pull = false;
    setPinMode( input );

    //setPull( true );
    updateStep();

    //if( !m_dirCtrl ) setDirection( m_outMask );
    //setPullup( m_puMask ? 1e5 : 0 );
    //if( !m_outCtrl && m_outMask ) IoPin::setOutState( true );
    //update();
}

void Stm32Pin::voltChanged()
{
    bool oldState = m_inpState;
    bool newState = IoPin::getInpState();

    if( oldState == newState ) return;

    while( m_arena->qemuAction )        // Wait for previous action executed
    {
        ; /// TODO: add timeout
    }
    m_arena->data8 = m_port;
    m_arena->mask8 = m_number;
    m_arena->data16 = newState;
    m_arena->qemuAction = SIM_GPIO_IN;
}

void Stm32Pin::setPull( bool p )
{
    if( m_pull == p ) return;
    m_pull = p;
    setOutState( m_outState );
}

bool Stm32Pin::setAlternate( bool a ) // If changing to Not Alternate, return false
{
    if( m_alternate == a ) return true;
    m_alternate = a;
    if( a ) qDebug() << "Stm32Pin::setAlternate" << this->m_id;
    return a;
}

void Stm32Pin::setAnalog( bool a ) /// TODO: if changing to Not Analog, return false
{
    if( m_analog == a ) return;
    m_analog = a;
}

void Stm32Pin::setPortState(  bool high ) // Set output from Port register
{
    if( m_alternate ) return;
    setPinState( high );
}

void Stm32Pin::setOutState( bool high ) // Set output from Alternate (peripheral)
{
    if( m_alternate ) setPinState( high );
}

void Stm32Pin::scheduleState( bool high, uint64_t time )
{
    if( m_alternate ) IoPin::scheduleState( high, time );
}

void Stm32Pin::setPinState( bool high ) // Set Output to Hight or Low
{
    m_outState = m_nextState = high;
    //if( m_pinMode < openCo  || m_stateZ ) return;

    //if( m_inverted ) high = !high;

    switch( m_pinMode )
    {
        case undef_mode: return;
        case input:
            if( m_pull ){
                m_outVolt = high ? m_outHighV : m_outLowV;
                ePin::stampCurrent( m_outVolt*m_pullAdmit );
            }
            break;
        case output:
            m_outVolt = high ? m_outHighV : m_outLowV;
            ePin::stampCurrent( m_outVolt*m_admit );
            break;
        case openCo:
            m_gndAdmit = high ? 1/1e8 : 1/m_outputImp;
            updtState();
            break;
        default: return;
    }
}

void Stm32Pin::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    if( !isVisible() ) return;
    Pin::paint( p, o, w );

    if( !m_pull ) return;
    if( m_pinMode > openCo ) return;

    // Draw pullUp/Down dot

    if( m_outState ) p->setBrush( QColor( 255, 180,   0 ) );
    else             p->setBrush( QColor(   0, 180, 255 ) );

    QPen pen = p->pen();
    pen.setWidthF( 0 );
    p->setPen(pen);
    int start = (m_length > 4) ? m_length-4.5 : 3.5;
    QRectF rect( start+0.6,-1.5, 3, 3 );
    p->drawEllipse(rect);
}
