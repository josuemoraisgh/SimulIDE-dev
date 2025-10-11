/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>
#include <QPainter>
#include <QFile>
#include <QFileInfo>

#include "esp32.h"
#include "itemlibrary.h"
#include "iopin.h"
#include "utils.h"

#define tr(str) simulideTr("Esp32",str)

enum ESP32Actions{
    ESP_GPIO_OUT = 1,
    ESP_GPIO_DIR,
    ESP_GPIO_IN,
    ESP_IOMUX,
    ESP_MATRIX_IN,
    ESP_MATRIX_OUT,
};


Component* Esp32::construct( QString type, QString id )
{
    return new Esp32( type, id );
}

LibraryItem* Esp32::libraryItem()
{
    QString executable = "./data/esp32/qemu-system-xtensa";
#ifdef _WIN32
    executable += ".exe";
#endif
    if( !QFileInfo::exists( executable ) ) return nullptr;

    return new LibraryItem(
        "Esp32",
        "Espressif",
        "ic2.png",
        "Esp32",
        Esp32::construct );
}

Esp32::Esp32( QString type, QString id )
     : QemuDevice( type, id )
{
    m_area = QRect( 0, 0, 15*8, 15*8 );
    m_color = QColor( 50, 50, 70 );

    m_ClkPeriod = 1000*1*1000; // 1 ms

    m_executable = "./data/esp32/qemu-system-xtensa";

    m_firmware ="";

    m_i2c[0].setDevice( this );
    m_i2c[1].setDevice( this );

    createPins();
}
Esp32::~Esp32(){}

void Esp32::createPins()
{
    m_gpioSize = 40;
    m_ioPin.resize( 40, nullptr ); // 20,24,28,29,30,31=NULL

    setPackageFile("./data/esp32/esp32.package");

    //int i = 0;
    for( IoPin* pin : m_ioPin )
    {
        if( pin )
        {
            //qDebug() << i << pin->pinId();
            pin->setOutHighV( 3.3 );
            pin->setInputHighV( 1.65 );
            pin->setInputLowV( 1.65 );
        }
        //else      qDebug() << i << "Null pin";
        //i++;
    }
    if( m_rstPin )
    {
        m_rstPin->setOutHighV( 3.3 );
        m_rstPin->setPullup( 1e5 );
        m_rstPin->setInputHighV( 0.65 );
        m_rstPin->setInputLowV( 0.65 );
    }

    /*m_ioPin[0]->setPullup( 1e5 );
    m_ioPin[1]->setPullup( 1e5 );
    m_ioPin[3]->setPullup( 1e5 );
    m_ioPin[5]->setPullup( 1e5 );
    m_ioPin[6]->setPullup( 1e5 );
    m_ioPin[7]->setPullup( 1e5 );
    m_ioPin[8]->setPullup( 1e5 );
    m_ioPin[9]->setPullup( 1e5 );
    m_ioPin[10]->setPullup( 1e5 );
    m_ioPin[11]->setPullup( 1e5 );
    m_ioPin[15]->setPullup( 1e5 );*/
}

bool Esp32::createArgs()
{
    QFileInfo fi = QFileInfo( m_firmware );

    if( fi.size() != 4194304 )
    {
        qDebug() << "Error firmware file size:" << fi.size() << "must be 4194304";
        return false;
    }

    int index = m_firmware.lastIndexOf(".");
    QString firmware = m_firmware.left( index );
    QString efuses = firmware+".efuse";

    if( !QFileInfo::exists( efuses ) )
        efuses = "./data/esp32/esp32.efuse";

    m_arguments.clear();

    m_arguments << m_shMemKey;          // Shared Memory key

    m_arguments << "qemu-system-xtensa";

    //m_arguments << "-d";
    //m_arguments << "in_asm";

    m_arguments << "-M";
    m_arguments << "esp32-simul";

    m_arguments << "-L";    /// TODO: embed files in Simulide
    m_arguments << "./data/esp32/rom/bin/";

    m_arguments << "-drive";
    m_arguments << "file="+firmware+".bin,if=mtd,format=raw";

    m_arguments << "-drive";
    m_arguments << "file="+efuses+",if=none,format=raw,id=efuse";

    m_arguments << "-global";
    m_arguments << "driver=nvram.esp32.efuse,property=drive,value=efuse";

    //m_arguments << "-nic";
    //m_arguments << "user,model=esp32_wifi,id=u1,net=192.168.4.0/24";

    m_arguments << "-global";
    m_arguments << "driver=timer.esp32.timg,property=wdt_disable,value=true";

    /*m_arguments << "-gdb");
    sprintf( m_argv[m_argc++], "tcp::%i", 1234 );*/

    //m_arguments << "-rtc";
    //m_arguments << "clock=vm";

    //int icount = 10;                      // instruction count "prescaler"
    //m_arguments << "-icount";
    //m_arguments << "shift=10,align=off,sleep=on";
    //m_arguments <<  "shift=auto,align=off,sleep=off";

    /*qDebug() << " ";
    QString msg;
    for( int i=2; i<m_arguments.size(); i++)
    {
        QString arg = m_arguments.at(i);

        msg.append( arg );
        if( i&1 ) msg.append(" ");
        else{
            qDebug() << msg;
            msg.clear();
        }
    }
    qDebug() << " ";*/

    return true;
}

void Esp32::stamp()
{
    QemuDevice::stamp();

    m_direc = 0;
    m_state = 0;

    m_pullUps  = 0;
    m_pullDown = 0;
    m_inputEn  = 0;
}

void Esp32::readInputs()
{
    //qDebug("readInputs");
    int n = m_arena->data8;
    int start = n ? 32 : 0;
    int end   = n ? 40 : 32;

    int i = 0;
    uint32_t inputVal = 0;
    for( int pin=start; pin<end; ++pin )
    {
        IoPin* ioPin = m_ioPin[pin];

        if( ioPin )
        {
            uint32_t mask = 1<<i;
            bool state = ioPin->getInpState();
            if( state ) inputVal |= mask;
        }
        i++;
        //if( state != ((*m_nextInput & mask)!=0) ) *m_maskInput |= mask; // Pin changed
    }
    m_arena->mask32 = inputVal ^ m_arena->data32;
    m_arena->data32 = inputVal;
}

void Esp32::setPinFunction( IoPin* ioPin, int func )
{
    /*switch( pin )
    {
    case 0: break;
    case 1: break;
    case 2: break;
    case 3: break;
    case 4: break;
    case 5: break;
    case 6: break;
    case 7: break;
    case 8: break;
    case 9: break;
    case 10: break;
    case 11: break;
    case 12: break;
    case 13: break;
    case 14: break;
    case 15: break;
    case 16: break;
    case 17: break;
    case 18: break;
    case 19: break;
    case 20: break;
    case 21:
        break;
    case 22:
        break;
    case 23: break;
    case 24: break;
    case 25: break;
    case 26: break;
    case 27: break;
    case 28: break;
    case 29: break;
    case 30: break;
    case 31: break;
    case 32: break;
    case 33: break;
    case 34: break;
    case 35: break;
    case 35: break;
    case 36: break;
    case 37: break;
    case 38: break;
    case 39: break;
    }*/
}

void Esp32::doAction()
{
    switch( m_arena->action )
    {
        case ESP_GPIO_OUT:       // Set Output
        {
            //qDebug() << "i"<<m_arena->nextState<<eventEnter;
            uint32_t state = m_arena->data32;
            for( uint i=0; i<32; ++i )
            {
                IoPin* pin = m_ioPin[i];
                if( !pin ) continue;
                uint32_t mask = 1<<i;
                uint32_t oldState = m_state & mask;
                uint32_t newState = state & mask;
                if( oldState == newState ) continue;
                //qDebug() << "outChanged" << i << newState;
                pin->setOutState( newState );
            }
            m_state = m_arena->data32;
            //qDebug() << "o"<<m_state<<eventEnter;
        } break;
        case ESP_GPIO_DIR:       // Set Direction
        {
            for( uint i=0; i<32; ++i )
            {
                IoPin* pin = m_ioPin[i];
                if( !pin ) continue;
                uint32_t mask =1<<i;
                //if( m_arena->inputEn & mask ) continue;

                uint32_t oldDirec = m_direc & mask;
                uint32_t newDirec = m_arena->data32 & mask;
                if( oldDirec == newDirec ) continue;

                if( newDirec ) pin->setPinMode( output );
                else           pin->setPinMode( input );
                //qDebug() << "dirChanged" << i << newDirec;
                //pin->changeCallBack( this, !newDirec ); // CallBack only on Inputs
            }
            m_direc = m_arena->data32;
        } break;
        case ESP_GPIO_IN:                  // Read Inputs
        {
            readInputs();
            //m_arena->action = 0;
        } break;
        case ESP_IOMUX:
        {
            // Sleep bits 0-6
            // PD bit 7
            // PU bit 8
            // IE bit 9
            // Drive bits 10-11
            // function bits 12-14

            int pin = m_arena->data8;
            IoPin* ioPin = m_ioPin.at( pin );
            uint64_t value = m_arena->data32;

            int func = (value>> 12) & 7;
            if( ioPin ) setPinFunction( ioPin, func );

            uint64_t pinMask = 1LL<<pin;

            uint64_t oldPd = m_pullDown & pinMask;
            uint64_t puld = ((value >> 7) & 1) << pin;

            if( puld != oldPd ){
                m_pullDown &= ~pinMask;
                m_pullDown |= puld;
            }
            uint64_t oldPu = m_pullUps  & pinMask;
            uint64_t pulu = ((value >> 8) & 1) << pin;

            if( pulu != oldPu ){
                //printf("iomuxPu %i %i %lu\n", pin, func, pulu);fflush( stdout );
                m_pullUps  &= ~pinMask;
                m_pullUps  |= pulu;
                double pullup = pulu ? 1e5 : 0;
                if( ioPin ) ioPin->setPullup( pullup );
            }
            uint64_t oldIe = m_inputEn  & pinMask;
            uint64_t inEn  = ((value >> 9) & 1) << pin;

            if( inEn != oldIe ){
                //printf("iomuxIe %i %i %lu\n", pin, func, inEn);fflush( stdout );
                m_inputEn  &= ~pinMask;
                m_inputEn  |= inEn;
            }
        } break;
        case ESP_MATRIX_IN:
        {
            uint32_t value = m_arena->data32;
            int pin  = value & 0x3F;
            IoPin* ioPin = m_ioPin.at( pin );

            uint8_t function = m_arena->data8;

            switch( function ) {
            case 9: //m_spi[0].cipo_pin = io2pin(gpio); // HSPIQ
                break;

            case 14: //m_uart[0].rx_pin = io2pin(gpio); // U0RXD
                break;
            case 17: //m_uart[1].rx_pin = io2pin(gpio); // U1RXD
                break;

            case 29: m_i2c[0].setSclPin( ioPin ); /*qDebug()<<"SCL"<<pin;*/ break; // I2CEXT0_SCL
            case 30: m_i2c[0].setSdaPin( ioPin ); /*qDebug()<<"SDA"<<pin;*/ break; // I2CEXT0_SDA

            case 64: //m_spi[1].cipo_pin = io2pin(gpio); // VSPIQ
                break;

            case 95: m_i2c[1].setSclPin( ioPin ); break; // I2CEXT1_SCL
            case 96: m_i2c[1].setSdaPin( ioPin ); break; // I2CEXT1_SDA

            case 198: //m_uart[2].rx_pin = io2pin(gpio); // U2RXD
                break;
            }
        } break;
        case ESP_MATRIX_OUT:
        {
            uint32_t value = m_arena->data32;
            int pin  = value & 0x3F;
            IoPin* ioPin = m_ioPin.at( pin );

            uint8_t function = m_arena->data8;

            switch( function ) {
            /*case 8:  // HSPICLK
                master_spi[0].sck_pin = io2pin(gpio);
                master_spi[0].ctrl_on = 1;
                break;
            case 9:  // HSPIQ
                master_spi[0].cipo_pin = io2pin(gpio);
                master_spi[0].ctrl_on = 1;
                break;
            case 10:  // HSPID
                master_spi[0].copi_pin = io2pin(gpio);
                master_spi[0].ctrl_on = 1;
                break;
            case 11:  // HSPICS0
                master_spi[0].cs_pin[0] = io2pin(gpio);
                master_spi[0].ctrl_on = 1;
                break;

            case 14:  // U0TXD
                master_uart[0].tx_pin = io2pin(gpio);
                master_uart[0].ctrl_on = 1;
                break;
            case 17:  // U1TXD
                master_uart[1].tx_pin = io2pin(gpio);
                master_uart[1].ctrl_on = 1;
                break;

            case 29:  // I2CEXT0_SCL
                master_i2c[0].scl_pin = io2pin(gpio);
                master_i2c[0].ctrl_on = 1;
                break;
            case 30:  // I2CEXT0_SDA
                master_i2c[0].sda_pin = io2pin(gpio);
                master_i2c[0].ctrl_on = 1;
                break;

            case 61:  // HSPICS1
                master_spi[0].cs_pin[1] = io2pin(gpio);
                master_spi[0].ctrl_on = 1;
                break;
            case 62:  // HSPICS2
                master_spi[0].cs_pin[2] = io2pin(gpio);
                master_spi[0].ctrl_on = 1;
                break;

            case 63:  // VSPICLK
                master_spi[1].sck_pin = io2pin(gpio);
                master_spi[1].ctrl_on = 1;
                break;
            case 64:  // VSPIQ
                master_spi[1].cipo_pin = io2pin(gpio);
                master_spi[1].ctrl_on = 1;
                break;
            case 65:  // VSPID
                master_spi[1].copi_pin = io2pin(gpio);
                master_spi[1].ctrl_on = 1;
                break;
            case 68:  // VSPICS0
                master_spi[1].cs_pin[0] = io2pin(gpio);
                master_spi[1].ctrl_on = 1;
                break;
            case 69:  // VSPICS1
                master_spi[1].cs_pin[1] = io2pin(gpio);
                master_spi[1].ctrl_on = 1;
                break;
            case 70:  // VSPICS2
                master_spi[1].cs_pin[2] = io2pin(gpio);
                master_spi[1].ctrl_on = 1;
                break;
            case 71:  // ledc_hs_sig_out0
            case 72:  // ledc_hs_sig_out1
            case 73:  // ledc_hs_sig_out2
            case 74:  // ledc_hs_sig_out3
            case 75:  // ledc_hs_sig_out4
            case 76:  // ledc_hs_sig_out5
            case 77:  // ledc_hs_sig_out6
            case 78:  // ledc_hs_sig_out7
            case 79:  // ledc_ls_sig_out0
            case 80:  // ledc_ls_sig_out1
            case 81:  // ledc_ls_sig_out2
            case 82:  // ledc_ls_sig_out3
            case 83:  // ledc_ls_sig_out4
            case 84:  // ledc_ls_sig_out5
            case 85:  // ledc_ls_sig_out6
            case 86:  // ledc_ls_sig_out7
                // printf("LEDC channel %i in GPIO %i\n",function - 71, gpio);
                bitbang_pwm_set_pin(&pwm_out, function - 71, io2pin(gpio));
                bitbang_pwm_set_enable(&pwm_out, function - 71, 1);
                break;
            case 87:  // rmt_sig_out0
                // case 88:  // rmt_sig_out1  //FIXME only channel 0 enabled
                // case 89:  // rmt_sig_out2
                // case 90:  // rmt_sig_out3
                // case 91:  // rmt_sig_out4
                // case 92:  // rmt_sig_out5
                // case 93:  // rmt_sig_out6
                // case 94:  // rmt_sig_out7
                //  printf("RMT channel %i in GPIO %i\n", function - 87, gpio);
                rmt_out.pins[function - 87] = io2pin(gpio);
                break;
            case 95:  // I2CEXT1_SCL
                master_i2c[1].scl_pin = io2pin(gpio);
                master_i2c[1].ctrl_on = 1;
                break;
            case 96:  // I2CEXT1_SDA
                master_i2c[1].sda_pin = io2pin(gpio);
                master_i2c[1].ctrl_on = 1;
                break;

            case 198:  // U2RTXD
                master_uart[2].tx_pin = io2pin(gpio);
                master_uart[2].ctrl_on = 1;
                break;*/
            }

        } break;
        case SIM_I2C:
        {
            //qDebug() << "---";
            //qDebug() << m_arena->time/1000000 << "Esp32::doAction";
            int         id = m_arena->data16;
            uint8_t   data = m_arena->data8;
            uint32_t event = m_arena->data32;

            if( id < 2 ) m_i2c[id].doAction( event, data );
        } break;
    }
}

Pin* Esp32::addPin( QString id, QString type, QString label,
                        int n, int x, int y, int angle, int length, int space )
{
    IoPin* pin = new IoPin( angle, QPoint(x, y), m_id+"-"+id, n-1, this, input );

    if( type.contains("rst") ) m_rstPin = pin;
    else{
        int n = id.right(2).toInt();
        m_ioPin.at(n) = pin;
    }
    QColor color = Qt::black;
    if( !m_isLS ) color = QColor( 250, 250, 200 );

    if( type.startsWith("inv") ) pin->setInverted( true );

    pin->setLength( length );
    pin->setSpace( space );
    pin->setLabelText( label );
    pin->setLabelColor( color );
    pin->setFlag( QGraphicsItem::ItemStacksBehindParent, true );
    return pin;
}
