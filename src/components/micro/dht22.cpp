/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QtMath>
#include <QPainter>
#include <QGraphicsProxyWidget>

#include "dht22.h"
#include "iopin.h"
#include "simulator.h"
#include "circuit.h"
#include "itemlibrary.h"
#include "updobutton.h"

#include "doubleprop.h"
#include "stringprop.h"

#define tr(str) simulideTr("Dht22",str)

Component* Dht22::construct( QString type, QString id )
{ return new Dht22( type, id ); }

LibraryItem* Dht22::libraryItem()
{
    return new LibraryItem(
        "DHt22/11",
        "Sensors",
        "dht22_ico.png",
        "Dht22",
        Dht22::construct );
}

Dht22::Dht22( QString type, QString id )
     : Component( type, id )
     , eElement( id )
{
    m_area = QRect(-16,-38, 32, 62 );

    m_graphical = true;

    m_temp = 22.5;
    m_humi = 68.5;
    m_tempInc = 0.5;
    m_humiInc = 5;

    m_pin.resize(4);
    m_pin[0] = m_inpin = new IoPin( 270, QPoint(-4, 32 ), id+"-inPin" , 0, this, input );
    m_pin[1] = new Pin  ( 270, QPoint(-12, 32 ), id+"-vccPin", 0, this );
    m_pin[2] = new Pin  ( 270, QPoint( 12, 32 ), id+"-gdnPin", 0, this );
    m_pin[3] = new Pin  ( 270, QPoint(  4, 32 ), id+"-ncPin" , 0, this );
    m_pin[1]->setUnused( true );
    m_pin[2]->setUnused( true );
    m_pin[3]->setUnused( true );

    m_inpin->setOutHighV( 5 );

    m_font.setFamily("Ubuntu Mono");
    m_font.setBold( true );
    m_font.setLetterSpacing( QFont::PercentageSpacing, 100 );
    setLabelPos(-28,-32, 0);
    setModel( "DHT22" );

    UpDoButton* tUpButton = new UpDoButton( true );
    QGraphicsProxyWidget* proxy = Circuit::self()->addWidget( tUpButton );
    proxy->setParentItem( this );
    proxy->setPos( QPoint(-20,-41 ) );

    UpDoButton* tDoButton = new UpDoButton( false );
    proxy = Circuit::self()->addWidget( tDoButton );
    proxy->setParentItem( this );
    proxy->setPos( QPoint(-20,-36 ) );

    QObject::connect( tUpButton, &UpDoButton::pressed, [=](){ tempUpClicked(); } );
    QObject::connect( tDoButton, &UpDoButton::pressed, [=](){ tempDoClicked(); } );

    UpDoButton* hUpButton = new UpDoButton( true );
    proxy = Circuit::self()->addWidget( hUpButton );
    proxy->setParentItem( this );
    proxy->setPos( QPoint(-20,-30 ) );

    UpDoButton* hDoButton = new UpDoButton( false );
    proxy = Circuit::self()->addWidget( hDoButton );
    proxy->setParentItem( this );
    proxy->setPos( QPoint(-20,-25 ) );

    QObject::connect( hUpButton, &UpDoButton::pressed, [=](){ humidUpClicked(); } );
    QObject::connect( hDoButton, &UpDoButton::pressed, [=](){ humidDoClicked(); } );

    addPropGroup( { tr("Main"), {
        new StrProp <Dht22>("DHT22", tr("Model"),"DHT11,DHT22"
                           , this, &Dht22::model, &Dht22::setModel, propNoCopy,"enum"  ),

        new DoubProp<Dht22>("Temp", tr("Tempature") ,"°C"
                            , this, &Dht22::temperature, &Dht22::setTemperature ),

        new DoubProp<Dht22>("TempInc", tr("Temp. increment") ,"°C"
                           , this, &Dht22::tempInc, &Dht22::setTempInc ),

        new DoubProp<Dht22>("Humi", tr("Humidity"),"_%"
                            , this, &Dht22::humidity, &Dht22::setHumidity ),

        new DoubProp<Dht22>("HumiInc", tr("Humid. increment"),"_%"
                           , this, &Dht22::humidInc, &Dht22::setHumidInc ),
    },0} );
}
Dht22::~Dht22(){}

void Dht22::stamp()   // Called at Simulation Start
{
    m_outStep = 0;
    m_bitStep = 0;
    m_lastIn = true;
    m_inpin->setPinMode( input );
    m_inpin->changeCallBack( this, true );

    if( m_changed ) calcData();
}

void Dht22::voltChanged()                              // Called when Input Pin changes
{
    bool inState = m_inpin->getVoltage() > 2.5;
    if( m_lastIn && !inState ) m_lastTime = Simulator::self()->circTime(); // Falling edge
    else if( !m_lastIn && inState )                                        // Rising edge
    {
        uint64_t time = Simulator::self()->circTime()-m_lastTime; // in picosecondss
        if( time > m_start )                              // Minimum input pulse
        {
            m_inpin->setPinMode( openCo );
            m_inpin->scheduleState( true, 0 );
            m_inpin->changeCallBack( this, false ); // Stop receiving voltChange() CallBacks
            Simulator::self()->addEvent( 30*1e6, this ); // Send ack after 30 us
    }   }
    m_lastIn = inState;
}

void Dht22::runEvent()
{
    if( m_outStep == 0 )       // Send ACK
    {
        if( m_changed ) calcData();
        m_bit = 1ULL<<39;
        if( m_bitStep == 0 )   // Start ACK
        {
            m_bitStep = 1;
            m_inpin->scheduleState( false, 0 );
            Simulator::self()->addEvent( 80*1e6, this );
        }
        else{                  // End ACK
            m_bitStep = 0;
            m_outStep++;
            m_inpin->scheduleState( true, 0 );
            Simulator::self()->addEvent( 80*1e6, this );
    }   }
    else{                     // Send data
        if( m_bitStep == 0 )  // Start bit (low side)
        {
            m_bitStep = 1;
            m_inpin->scheduleState( false, 0 );
            Simulator::self()->addEvent( 50*1e6, this );
        }
        else{                 // End bit (high side)
            m_bitStep = 0;
            m_inpin->scheduleState( true, 0 );
            if( m_bit == 0 )             // Transmission finished
            {
                m_outStep = 0;
                m_inpin->setPinMode( input );
                m_inpin->changeCallBack( this, true );
                return;
            }
            uint64_t time = ( (m_data & m_bit) > 0 ) ? 70*1e6 : 27*1e6;
            Simulator::self()->addEvent( time, this );
            m_bit >>= 1;
}   }   }

void Dht22::calcData()
{
    m_changed = false;

    uint64_t tempI, tempD, humiI, humiD;
    if( m_DHT22 )
    {
        uint64_t temp = qFabs( m_temp*10 );
        if( m_temp < 0 ) temp |= 1<<15;
        tempI = temp >> 8;
        tempD = temp & 0xFF;

        uint64_t humi = m_humi*10;
        humiI = humi >> 8;
        humiD = humi & 0xFF;
    }else{                          // Using ASAIR DHT11 specifications
        tempI = m_temp;
        tempD = (m_temp-tempI)*10;
        if( m_temp < 0 ) tempD |= 1<<8;
        humiI = m_humi;
        humiD = 0; //(m_humi-humiI)*10;
    }
    uint8_t checksum = tempI+tempD+humiI+humiD;
    m_data = (humiI<<32)+(humiD<<24)+(tempI<<16)+(tempD<<8)+checksum;

    update();
}

void Dht22::setTemperature( double t )
{
    m_temp = t;
    if( m_DHT22) {
        if( m_temp > 80 )  m_temp = 80;
    }else{                             // Using ASAIR DHT11 specifications
        if( m_temp > 60 ) m_temp = 60;
    }
    m_changed = true;
    update();
}

void Dht22::tempUpClicked()
{
    setTemperature( m_temp + m_tempInc );
}

void Dht22::tempDoClicked()
{
    setTemperature( m_temp - m_tempInc );
}

void Dht22::setHumidity( double h )
{
    m_humi = h;
    if( m_DHT22) {
        if( m_humi > 100 ) m_humi = 100;
    }else{                             // Using ASAIR DHT11 specifications
        if( m_humi > 95 ) m_humi = 95;
    }
    m_changed = true;
    update();
}

void Dht22::humidUpClicked()
{
    setHumidity( m_humi + m_humiInc );
}

void Dht22::humidDoClicked()
{
    setHumidity( m_humi - m_humiInc );
}

void Dht22::setModel( QString model )
{
    m_DHT22 = (model == "DHT22");
    if( m_DHT22 ){
        setBackground("dht22.svg");
        m_start = 1e9;
    }else{
        setBackground("dht11.svg");
        m_start = 18e9;
    }
    m_changed = true;
}

QString Dht22::model() { return m_DHT22 ? "DHT22" : "DHT11"; }

double Dht22::trim( double data ) { return (double)(((int)(data*10)))/10; }

void Dht22::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );

    p->setRenderHint( QPainter::Antialiasing, true );

    p->setOpacity( .6 );
    p->fillRect( QRectF(-14,-40, 32, 19 ), QColor( Qt::white ) );
    p->setOpacity( 1 );

    p->drawPixmap( QRect(-18,-20, 36, 46 ), *m_backPixmap, m_backPixmap->rect() );

    m_font.setStretch( 100 );
    m_font.setPixelSize( 6 );
    p->setFont( m_font );

    QString label = m_DHT22 ? "DHT22" : "DHT11";
    p->drawText( QRectF(-16,-19, 32, 8 ), Qt::AlignCenter, label );

#ifdef _WIN32
    m_font.setStretch( 99 );
#else
    m_font.setStretch( 93 );
#endif
    m_font.setPixelSize( 9 );
    p->setFont( m_font );
    p->drawText( QRectF(-14,-40, 32, 8 ), Qt::AlignCenter, QString::number( m_temp, 'f', 1 )+"°C" );
    p->drawText( QRectF(-14,-29, 32, 8 ), Qt::AlignCenter, QString::number( m_humi, 'f', 1 )+" %" );

    Component::paintSelected( p );
}
