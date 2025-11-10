/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>
#include <QPainter>
#include <QFileInfo>

#include "stm32.h"
#include "stm32pin.h"
#include "qemutwi.h"
#include "stm32spi.h"
#include "qemuusart.h"

#define tr(str) simulideTr("Stm32",str)

enum armActions{
    ARM_GPIO_OUT = 1,
    ARM_GPIO_CRx,
    ARM_GPIO_IN,
    ARM_ALT_OUT,
    ARM_REMAP
};


Stm32::Stm32( QString type, QString id, QString device )
     : QemuDevice( type, id )
{
    m_area = QRect(-32,-32, 64, 64 );

    m_device = device;
    QString model = device.right( 3 );

    QStringList pkgs = { "T", "C", "R", "V", "Z" };
    uint32_t pkg = pkgs.indexOf( model.mid(1,1) );
    switch( pkg ){
    case 0: m_packageFile = "stm32_dip36.package";  break; // T
    case 1: m_packageFile = "stm32_dip48.package";  break; // C
    case 2: m_packageFile = "stm32_dip64.package";  break; // R
    case 3: m_packageFile = "stm32_dip100.package"; break; // V
    //case 4: m_packageFile = "stm32_dip144.package"; break; // Z

    default: m_packageFile = "stm32_dip64.package";  break;
    }

    QStringList vars = { "4", "6", "8", "B", "C", "D", "E", "F", "G" };
    uint32_t var = vars.indexOf( model.right(1) );
    switch( var ){
        case 0:                                                               // 4
        case 1: { m_portN = 4; m_usartN = 2; m_i2cN = 1; m_spiN = 1; } break; // 6
        case 2:                                                               // 8
        case 3: { m_portN = 5; m_usartN = 3; m_i2cN = 2; m_spiN = 2; } break; // B
        case 4:                                                               // C
        case 5:                                                               // D
        case 6: { m_portN = 7; m_usartN = 5; m_i2cN = 2; m_spiN = 3; } break; // E
        //else if( var == "F" ) m_usartN = 3; m_timerN = 4; m_i2cN = 2;
        //else if( var == "G" ) m_usartN = 3; m_timerN = 4; m_i2cN = 2;
        default: break;
    }

    uint32_t fam = model.left(1).toInt();

    m_model = fam << 16 | pkg << 8 | var;
    //qDebug() << "Stm32::Stm32 model" << device << m_model;
    m_executable = "./data/STM32/qemu-system-arm";

    m_firmware ="";

    createPins();

    m_i2cs.resize( m_i2cN );
    for( int i=0; i<m_i2cN; ++i ) m_i2cs[i] = new QemuTwi( this, "I2C"+QString::number(i), i );

    m_spis.resize( m_spiN );
    for( int i=0; i<m_spiN; ++i ) m_spis[i] = new Stm32Spi( this, "I2C"+QString::number(i), i );

    m_usarts.resize( m_usartN );
    for( int i=0; i<m_usartN; ++i ) m_usarts[i] = new QemuUsart( this, "Usart"+QString::number(i), i );

    //m_timers.resize( m_timerN );
    //for( int i=0; i<m_timerN; ++i ) m_timers[i] = new QemuTimer( this, "Timer"+QString::number(i), i );
}
Stm32::~Stm32(){}

void Stm32::stamp()
{
    if( m_i2cN > 0 ) m_i2cs[0]->setPins( m_ports[1].at(6) , m_ports[1].at(7)  );
    if( m_i2cN > 1 ) m_i2cs[1]->setPins( m_ports[1].at(10), m_ports[1].at(11) );

    if( m_spiN > 0 ) m_spis[0]->setPins( m_ports[0].at(7) , m_ports[0].at(6) , m_ports[0].at(5) , m_ports[0].at(4)  );
    if( m_spiN > 1 ) m_spis[1]->setPins( m_ports[1].at(15), m_ports[1].at(14), m_ports[1].at(13), m_ports[1].at(12) );
    if( m_spiN > 2 ) m_spis[2]->setPins( m_ports[1].at(5) , m_ports[1].at(4) , m_ports[1].at(3) , m_ports[0].at(15) );

    if( m_usartN > 0 ) m_usarts[0]->setPins({m_ports[0].at(9) , m_ports[0].at(10)}); // No Remap (TX/PB6, RX/PB7)
    if( m_usartN > 1 ) m_usarts[1]->setPins({m_ports[0].at(2) , m_ports[0].at(3) }); // No remap (CTS/PA0, RTS/PA1, TX/PA2, RX/PA3, CK/PA4), Remap (CTS/PD3, RTS/PD4, TX/PD5, RX/PD6, CK/PD7)
    if( m_usartN > 2 ) m_usarts[2]->setPins({m_ports[1].at(10), m_ports[1].at(11)});
    if( m_usartN > 3 ) m_usarts[3]->setPins({m_ports[2].at(10), m_ports[2].at(11)});
    if( m_usartN > 4 ) m_usarts[4]->setPins({m_ports[2].at(12), m_ports[3].at(2) });

    QemuDevice::stamp();
}

void Stm32::createPins()
{
    m_ports.resize( m_portN );
    for( int i=0; i<m_portN; ++i )
        createPort( &m_ports[i], i+1, QString('A'+i), 16 );

    setPackageFile("./data/STM32/"+m_packageFile);
    Chip::setName( m_device );
}

void Stm32::createPort( std::vector<Stm32Pin*>* port, uint8_t number, QString pId, uint8_t n )
{
    for( int i=0; i<n; ++i )
    {
        //qDebug() << "Stm32::createPort" << m_id+"-P"+pId+QString::number(i);
        Stm32Pin* pin = new Stm32Pin( number, i, m_id+"-P"+pId+QString::number(i), this );
        port->emplace_back( pin );
        pin->setVisible( false );
    }
}

bool Stm32::createArgs()
{
    //QFileInfo fi = QFileInfo( m_firmware );

    /*if( fi.size() != 1048576 )
    {
        qDebug() << "Error firmware file size:" << fi.size() << "must be 1048576";
        return false;
    }*/
    m_arena->data32 = m_model;

    m_arguments.clear();

    m_arguments << m_shMemKey;          // Shared Memory key

    m_arguments << "qemu-system-arm";

    QStringList extraArgs = m_extraArgs.split(",");
    for( QString arg : extraArgs )
    {
        if( arg.isEmpty() ) continue;
        m_arguments << arg;
    }

    //m_arguments << "-d";
    //m_arguments << "in_asm";

    //m_arguments << "-machine";
    //m_arguments << "help";

    m_arguments << "-M";
    m_arguments << "stm32-f10xxx-simul";

    m_arguments << "-drive";
    m_arguments << "file="+m_firmware+",if=pflash,format=raw";

    //m_arguments << "-accel";
    //m_arguments << "tcg,tb-size=100";

    //m_arguments << "-rtc";
    //m_arguments <<"clock=vm";

    m_arguments << "-icount";
    m_arguments <<"shift=14,align=off,sleep=off";

    //m_arguments << "-kernel";
    //m_arguments << m_firmware;

    return true;
}

void Stm32::doAction()
{
    switch( m_arena->simuAction )
    {
        case ARM_GPIO_OUT:       // Set Output
        {
            uint8_t  port  = m_arena->data8;
            uint16_t state = m_arena->data16;

            if( port >= m_portN ) return;
            if( m_state[port] == state ) return;
            m_state[port] = state;

            //qDebug() << "Stm32::doAction GPIO_OUT Port:"<< port << "State:" << state;

            setPortState( port, state );
        } break;
        case ARM_GPIO_CRx:       // Configure Pins
        {
            uint8_t  port   = m_arena->data8;
            uint8_t  shift  = m_arena->mask8;
            uint32_t config = m_arena->data32;

            if( port < m_portN ) cofigPort( port, config, shift );
        } break;
        case ARM_GPIO_IN:       // Read Inputs
        {
            uint8_t port = m_arena->data8;
            if( port < m_portN ) m_arena->data16 = readInputs( port );
        } break;
        case ARM_ALT_OUT:      // Set Alternate Output
        {
            uint8_t port  = m_arena->data8;
            uint8_t pin   = m_arena->mask8;
            bool    state = (m_arena->data16 > 0);

            if( port < m_portN ) setPinState( port, pin, state );
        } break;
        case ARM_REMAP:       // AFIO Remap
        {
            uint32_t mapr = m_arena->data32;

            uint8_t spi1Map = mapr & 1;
            switch( spi1Map )       // SPI1
            {
                case 0:{        // No remap (NSS/PA4, SCK/PA5, MISO/PA6, MOSI/PA7)
                    m_spis[0]->setPins( m_ports[0].at(7), m_ports[0].at(6), m_ports[0].at(5), m_ports[0].at(4) );
                }break;
                case 1:{        // Remap (NSS/PA15, SCK/PB3, MISO/PB4, MOSI/PB5)
                    m_spis[0]->setPins( m_ports[1].at(5), m_ports[1].at(4), m_ports[1].at(3), m_ports[0].at(15) );
                }break;
            }
            mapr >>= 1;

            switch( mapr & 1 )      // I2C1
            {
                case 0:{        // No remap (SCL/PB6, SDA/PB7)
                    m_i2cs[0]->setSclPin( m_ports[1].at(6) );
                    m_i2cs[0]->setSdaPin( m_ports[1].at(7) );
                }break;
                case 1:{        // Remap (SCL/PB8, SDA/PB9)
                    m_i2cs[0]->setSclPin( m_ports[1].at(8) );
                    m_i2cs[0]->setSdaPin( m_ports[1].at(9) );
                }break;
            }
            mapr >>= 1;

            switch( mapr & 1 )      // USART1
            {
                case 0: m_usarts[0]->setPins({m_ports[0].at(9), m_ports[0].at(10)}); break; //No remap (TX/PA9, RX/PA10)
                case 1: m_usarts[0]->setPins({m_ports[1].at(6), m_ports[1].at(7)});  break;  //Remap (TX/PB6, RX/PB7)
            }
            mapr >>= 1;

            switch( mapr & 1 )      // USART2
            {
                case 0: m_usarts[1]->setPins({m_ports[0].at(3), m_ports[0].at(3)}); break; //No remap (CTS/PA0, RTS/PA1, TX/PA2, RX/PA3, CK/PA4)
                case 1: m_usarts[1]->setPins({m_ports[3].at(5), m_ports[3].at(6)}); break;  //Remap (CTS/PD3, RTS/PD4, TX/PD5, RX/PD6, CK/PD7)
            }
            mapr >>= 1;

            switch( mapr & 0b11 )   // USART3
            {
                case 0: m_usarts[2]->setPins({m_ports[1].at(10), m_ports[1].at(11)}); break; //No remap (TX/PB10, RX/PB11, CK/PB12, CTS/PB13, RTS/PB14)
                case 1: m_usarts[2]->setPins({m_ports[2].at(10), m_ports[2].at(11)}); break; //Partial remap (TX/PC10, RX/PC11, CK/PC12, CTS/PB13, RTS/PB14)
                case 2:                                                               break; //not used
                case 3: m_usarts[2]->setPins({m_ports[3].at(8), m_ports[3].at(9)});   break; // Full remap (TX/PD8, RX/PD9, CK/PD10, CTS/PD11, RTS/PD12)
            }
        }break;
        case SIM_I2C:         // I2C
        {
            uint16_t id = m_arena->data16;

            //qDebug()<< "Stm32::doAction I2C id"<< id<<"data"<<data<<"event"<<event;

            if( id < m_i2cN ) m_i2cs[id]->doAction();
        } break;
        case SIM_SPI:        // SPI
        {
            uint16_t id = m_arena->data16;
            if( id < m_spiN ) m_spis[id]->doAction();
        }break;
        case SIM_USART:      // USART
        {
            uint16_t id = m_arena->data16;

            //qDebug() << "Stm32::doAction SIM_USART Uart:"<< id << "action:"<< event<< "byte:" << data;
            if( id < m_usartN ) m_usarts[id]->doAction();
        } break;
        //case SIM_TIMER:
        //{
        //    uint16_t id = m_arena->data16;

        //    if( id < 5 ) m_timers[id]->doAction();
        //    //qDebug() << "Stm32::doAction SIM_TIMER Timer:"<< id << "action:"<< m_arena->simuAction<< "byte:" << data;
        //} break;
        default: qDebug() << "Stm32::doAction Unimplemented"<< m_arena->simuAction;
    }
}

uint16_t Stm32::readInputs( uint8_t port )
{
    std::vector<Stm32Pin*> ioPort = m_ports[port-1]; //getPort( port );

    uint16_t state = 0;
    for( uint8_t i=0; i<ioPort.size(); ++i )
    {
        Stm32Pin* ioPin = ioPort.at( i );
        if( ioPin->getInpState() ) state |= 1<<i;
    }
    //qDebug() << "Stm32::doAction GPIO_IN"<< port << state;
    return state;
}

void Stm32::setPortState( uint8_t port, uint16_t state )
{
    std::vector<Stm32Pin*> ioPort = m_ports[port-1]; //getPort( port );

    for( uint8_t i=0; i<ioPort.size(); ++i )
    {
        Stm32Pin* ioPin = ioPort.at( i );
        ioPin->setPortState( state & (1<<i) );
    }
}

void Stm32::setPinState( uint8_t port, uint8_t pin, bool state )
{
    std::vector<Stm32Pin*> ioPort = m_ports[port-1]; //getPort( port );

    //qDebug() << "Stm32::setPinState" << port << pin << state;

    Stm32Pin* ioPin = ioPort.at( pin );
    ioPin->setOutState( state );
}

void Stm32::cofigPort( uint8_t port,  uint32_t config, uint8_t shift )
{
    std::vector<Stm32Pin*> ioPort = m_ports[port-1]; //getPort( port );

    //qDebug() << "Stm32::doAction GPIO_DIR Port:"<< port << "Directions:" << m_direction;

    for( uint8_t i=shift; i<shift+8; ++i )
    {
        Stm32Pin*  ioPin = ioPort.at( i );
        uint8_t cfgShift = i*4;
        uint32_t cfgMask = 0b1111 << cfgShift;
        uint32_t cfgBits = (config & cfgMask) >> cfgShift;

        uint8_t isOutput = cfgBits & 0b0011;  // 0 = Input

        if( isOutput ) // Output
        {
            uint8_t   open = cfgBits & 0b0100;
            pinMode_t pinMode = open ? openCo : output;
            ioPin->setPinMode( pinMode );
        }
        else          // Input
        {
            ioPin->setPinMode( input );
            uint8_t pull = cfgBits & 0b1000;
            ioPin->setPull( pull>0 );
        }

        uint8_t mode = cfgBits & 0b1100; // Analog if CNF0[1:0] == 0
        ioPin->setAnalog( mode==0 );
        /// TODO: if changing to Not Analog // Restore Port State

        uint8_t alternate = cfgBits & 0b1000;
        if( !ioPin->setAlternate( alternate>0 ) )              // If changing to No Alternate
            ioPin->setPortState( (m_state[port] & 1<<i) > 0 ); // Restore Port state
    }
}
      // CNF
        // Input mode:
        //     00: Analog mode
        //     01: Input Floating (reset state)
        //     10: Input with pull-up / pull-down
        //     11: Reserved
        // Output mode:
        // CNF0 0 push-pull
        //      1 Open-drain
        // CNF1 0 General purpose output
        //      1 Alternate function output

Pin* Stm32::addPin( QString id, QString type, QString label,
                   int n, int x, int y, int angle, int length, int space )
{
    IoPin* pin = nullptr;
    //qDebug() << "Stm32::addPin" << id;
    if( type.contains("rst") )
    {
        pin = new IoPin( angle, QPoint(x, y), m_id+"-"+id, n-1, this, input );
        m_rstPin = pin;
        m_rstPin->setOutHighV( 3.3 );
        m_rstPin->setPullup( 1e5 );
        m_rstPin->setInputHighV( 0.65 );
        m_rstPin->setInputLowV( 0.65 );
    }
    else{
        //qDebug() << "Stm32::addPin"<<id;
        uint n = id.right(2).toInt();
        QString portStr = id.at(1);
        std::vector<Stm32Pin*>* port = nullptr;
        if     ( portStr == "A" ) port = &m_ports[0];
        else if( portStr == "B" ) port = &m_ports[1];
        else if( portStr == "C" ) port = &m_ports[2];
        else if( portStr == "D" ) port = &m_ports[3];
        else if( portStr == "E" ) port = &m_ports[4];

        if( !port ) return nullptr; //new IoPin( angle, QPoint(x, y), m_id+"-"+id, n-1, this, input );

        if( n >= port->size() ) return nullptr;

        pin = port->at( n );
        if( !pin ) return nullptr;

        pin->setPos( x, y );
        pin->setPinAngle( angle );
        pin->setVisible( true );
    }
    QColor color = Qt::black;
    if( !m_isLS ) color = QColor( 250, 250, 200 );

    //if( type.startsWith("inv") ) pin->setInverted( true );

    pin->setLength( length );
    pin->setSpace( space );
    pin->setLabelText( label );
    pin->setLabelColor( color );
    pin->setFlag( QGraphicsItem::ItemStacksBehindParent, true );
    return pin;
}
