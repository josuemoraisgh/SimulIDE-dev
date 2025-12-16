/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>
#include <QDebug>

#include "esp32pin.h"
#include "qemumodule.h"
#include "simulator.h"

esp32Pin::esp32Pin( int i, QString id, QemuDevice* mcu, IoPin* dummyPin )
        : IoPin( 0, QPoint(0,0), mcu->getId()+"-"+id, i, mcu, input )
        , QemuModule( mcu, i )
{
    //m_id     = id;

    m_pullAdmit = 1e5; // 10k

    double vdd = 3.3; //m_port->getMcu()->vdd();
    m_outHighV = vdd;
    m_inpHighV = vdd/2;
    m_inpLowV  = vdd/2;

    m_pullUp = 0;
    m_pullDown = 0;

    m_pinMask = 1<<i;

    m_dummyPin =  dummyPin;

    m_pinLabel = id;

    for( int i=0; i<6; ++i ) m_iomuxPin[i] = { nullptr, nullptr, "- -" };
}
esp32Pin::~esp32Pin() {}

void esp32Pin::initialize()
{
    //Pin::setLabelText( m_pinLabel );
    //Pin::setLabelColor( QColor( 250, 250, 200 ) );

    //m_isAnalog = false;
    ////m_portState = false;

    //double vdd = 3.3; //m_port->getMcu()->vdd();
    //m_outHighV = vdd;
    //m_inpHighV = vdd/2;
    //m_inpLowV  = vdd/2;
    //
    IoPin::initialize();
}

void esp32Pin::updateStep()
{
    Pin::setLabelText( m_iomuxPin[m_iomuxIndex].label );
    Simulator::self()->remFromUpdateList( this );
}

void esp32Pin::stamp()
{
    //m_alternate = false;
    //m_analog = false;
    //m_pull = false;

    m_pullUp   = 0;
    m_pullDown = 0;
    m_inputEn  = 0;
    setPinMode( input );

    m_iomuxIndex = -1;
    setIoMuxFunc( 0 );

    //setPull( true );
    //updateStep();

    //update();

    IoPin::stamp();
}

void esp32Pin::voltChanged()
{
    bool oldState = m_inpState;
    bool newState = IoPin::getInpState();

    if( oldState == newState ) return;

    // while( m_arena->qemuAction )        // Wait for previous action executed
    // {
    //     ; /// TODO: add timeout
    // }
    // m_arena->data8 = m_port-1;
    // m_arena->mask8 = m_number;
    // m_arena->data16 = newState;
    // m_arena->qemuAction = SIM_GPIO_IN;
}

void esp32Pin::setPinMode( pinMode_t mode )
{
    IoPin::setPinMode( mode );
    changeCallBack( this, mode == input );
}

//void esp32Pin::setPull( bool p )
//{
//    if( m_pull == p ) return;
//    m_pull = p;
//    setOutState( m_outState );
//}
//
//bool esp32Pin::setAlternate( bool a ) // If changing to Not Alternate, return false
//{
//    if( m_alternate == a ) return true;
//    m_alternate = a;
//    if( a ) qDebug() << "esp32Pin::setAlternate" << this->m_id;
//    return a;
//}
//
//void esp32Pin::setAnalog( bool a ) /// TODO: if changing to Not Analog, return false
//{
//    if( m_analog == a ) return;
//    m_analog = a;
//}

void esp32Pin::setPortState(  bool high ) // Set output from Port register
{
    //if( m_alternate ) return;
    setPinState( high );
}

void esp32Pin::setOutState( bool high ) // Set output from Alternate (peripheral)
{
    //if( m_alternate )
        setPinState( high );
}

void esp32Pin::scheduleState( bool high, uint64_t time )
{
    //if( m_alternate )
        IoPin::scheduleState( high, time );
}

void esp32Pin::setPinState( bool high ) // Set Output to Hight or Low
{
    m_outState = m_nextState = high;
    //if( m_pinMode < openCo  || m_stateZ ) return;

    if( m_inverted ) high = !high;

    switch( m_pinMode )
    {
        case undef_mode: return;
        case input:
            /// if( m_pull ){
            ///     m_outVolt = high ? m_outHighV : m_outLowV;
            ///     ePin::stampCurrent( m_outVolt*m_pullAdmit );
            /// }
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

void esp32Pin::setIoMuxPins( QList<funcPin> funcPins )
{
    for( int i=0; i<6; ++i )
    {
        funcPin fp = funcPins.at(i);
        if( fp.label.isEmpty() ) fp.label = m_pinLabel;

        m_iomuxPin[i] = fp;
    }
}

void esp32Pin::setIoMuxFunc( uint64_t value )
{
    uint64_t puld = (value >> 7) & 1;

    if( m_pullDown != puld ){
        m_pullDown = puld;
        /// TODO: iplement IoPin::setPulldown
    }

    uint8_t pulu = (value >> 8) & 1;
    if( m_pullUp != pulu ){
        m_pullUp = pulu;
        double pullup = pulu ? m_pullAdmit : 0;
        IoPin::setPullup( pullup );
    }

    m_inputEn = (value >> 9) & 1;

    uint8_t func = (value >> 12) & 7;
    if( m_iomuxIndex == func ) return;
    selectIoMuxFunc( func );
}

void esp32Pin::selectIoMuxFunc( uint8_t func )
{
    if( func > 5 ){
        qDebug() << this->pinId() << "Selected func ERROR"<< func;
        return;
    }
    if( m_iomuxIndex < 6 ){
        if( m_iomuxPin[m_iomuxIndex].pinPointer )
            *m_iomuxPin[m_iomuxIndex].pinPointer = m_dummyPin;

        QemuModule* mod = m_iomuxPin[m_iomuxIndex].module;
        if( mod ) mod->connected( false );
    }
    if( m_iomuxPin[func].label.isEmpty() ) m_iomuxPin[func].label = m_pinLabel;
    Simulator::self()->addToUpdateList( this );

    if( m_iomuxPin[func].pinPointer )
    {
        //qDebug() << this->pinId() << "Selected func"<< func << m_iomuxPin[func].label;
        *m_iomuxPin[func].pinPointer = this;

        QemuModule* mod = m_iomuxPin[func].module;
        if( mod ) mod->connected( true );

        Pin::setLabelColor( QColor( 255, 255, 100 ) );
    }else{
        Pin::setLabelColor( QColor( 100, 100, 100 ) );
    }
    m_iomuxIndex = func;
    //qDebug() << this->pinId() << "Selected func"<< func << m_iomuxPin[func].label;
    //update();
}

void esp32Pin::setMatrixFunc( uint16_t val, funcPin func )
{
    // val & 1<<11: OEN_INV_SEL 1: Invert the output enable signal
    // val & 1<<10: 1: use output enable from bit n of GPIO_ENABLE_REG;
    //              0: use output enable from peripheral. (R/W)
    // val & 1<< 9: 1. Invert the output value

    //qDebug() << this->pinId() << "Matrix function"<< func.label<< (val & 0b111000000000);
    m_iomuxPin[2] = func;
    if( m_iomuxIndex == 2 ) selectIoMuxFunc( 2 );
}

void esp32Pin::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    if( !isVisible() ) return;
    Pin::paint( p, o, w );

    if( !m_pullUp && !m_pullDown ) return;
    if( m_pinMode > openCo ) return;

    // Draw pullUp/Down dot

    if( m_pullUp ) p->setBrush( QColor( 255, 180,   0 ) );
    else           p->setBrush( QColor(   0, 180, 255 ) );

    QPen pen = p->pen();
    pen.setWidthF( 0 );
    p->setPen(pen);
    int start = (m_length > 4) ? m_length-4.5 : 3.5;
    QRectF rect( start+0.6,-1.5, 3, 3 );
    p->drawEllipse(rect);
}
