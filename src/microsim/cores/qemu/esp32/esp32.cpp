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
#include "esp32twi.h"
#include "esp32spi.h"
#include "esp32usart.h"
#include "esp32pin.h"
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



Esp32::Esp32( QString type, QString id, QString device )
     : QemuDevice( type, id )
{
    m_area = QRect( 0, 0, 15*8, 15*8 );
    m_color = QColor( 50, 50, 70 );

    m_executable = "./data/esp32/qemu-system-xtensa";

    m_firmware ="";

    m_dummyPin = new esp32Pin( 0, m_id+"-dummy", this, nullptr );
    m_dummyPin->setVisible( false );

    m_i2cN = 2;
    m_i2cs.resize( m_i2cN );
    for( int i=0; i<m_i2cN; ++i ) m_i2cs[i] = new Esp32Twi( this, "I2C"+QString::number(i), i );

    m_spiN = 4;
    m_spis.resize( m_spiN );
    for( int i=0; i<m_spiN; ++i ) m_spis[i] = new Esp32Spi( this, "I2C"+QString::number(i), i );

    m_usartN = 3;
    m_usarts.resize( m_usartN );
    for( int i=0; i<m_usartN; ++i )
    {
        m_usarts[i] = new Esp32Usart( this, "Usart"+QString::number(i), i );
        m_usarts[i]->setPins({ m_dummyPin, m_dummyPin});
    }

    createPins();
}
Esp32::~Esp32(){}

bool Esp32::createArgs()
{
    QFileInfo fi = QFileInfo( m_firmware );

    if( fi.size() != 4194304 )
    {
        qDebug() << "Error firmware file size:" << fi.size() << "must be 4194304";
        qDebug() << m_firmware;
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

    m_arguments << "-icount";
    m_arguments <<"shift=0,align=off,sleep=off";

    /*m_arguments << "-gdb");
    sprintf( m_argv[m_argc++], "tcp::%i", 1234 );*/

    //m_arguments << "-rtc";
    //m_arguments << "clock=vm";

    //int icount = 10;                      // instruction count "prescaler"
    //m_arguments << "-icount";
    //m_arguments << "shift=10,align=off,sleep=on";
    //m_arguments <<  "shift=auto,align=off,sleep=off";

    return true;
}

void Esp32::stamp()
{
    QemuDevice::stamp();

    m_direc = 0;
    m_state = 0;
}

void Esp32::readInputs()
{
    ////qDebug("readInputs");
    //int n = m_arena->data8;
    //int start = n ? 32 : 0;
    //int end   = n ? 40 : 32;

    //int i = 0;
    //uint32_t inputVal = 0;
    //for( int pin=start; pin<end; ++pin )
    //{
    //    IoPin* ioPin =  m_espPad[pin];

    //    if( ioPin )
    //    {
    //        uint32_t mask = 1<<i;
    //        bool state = ioPin->getInpState();
    //        if( state ) inputVal |= mask;
    //    }
    //    i++;
    //    //if( state != ((*m_nextInput & mask)!=0) ) *m_maskInput |= mask; // Pin changed
    //}
    //m_arena->mask32 = inputVal ^ m_arena->data32;
    //m_arena->data32 = inputVal;
}

void Esp32::matrixFunc( uint8_t out )
{
    //int pin  = m_arena->data8 & 0x3F;
    //if( pin >= m_gpioSize ) return;
    //esp32Pin* espPin = m_espPad[pin];
    //if( !espPin )
    //    return;

    //uint16_t value = m_arena->data16;
    //uint16_t function = value & 0x1FF;

    //funcPin fp = { nullptr, nullptr, "---" };;

    //if      ( function < 256 ) fp = out ? m_matrixOut[function] : m_matrixIn[function];
    //else if( function == 256 ) fp = { nullptr, &m_ioPin[pin], ""};

    //if( fp.label == "---") return;

    ////qDebug() << "Esp32::matrixFunc"<< pin << function;
    //espPin->setMatrixFunc( value, fp );
}

void Esp32::doAction()
{
    ////qDebug() << "Esp32::doAction";
    //switch( m_arena->simuAction)
    //{
    //    case ESP_GPIO_OUT:       // Set Output
    //    {
    //        //qDebug() << "i"<<m_arena->nextState<<eventEnter;
    //        uint32_t state = m_arena->data32;
    //        for( uint i=0; i<32; ++i )
    //        {
    //            IoPin* pin = m_ioPin[i];
    //            if( !pin ) continue;
    //            uint32_t mask = 1<<i;
    //            uint32_t oldState = m_state & mask;
    //            uint32_t newState = state & mask;
    //            if( oldState == newState ) continue;
    //            //qDebug() << "outChanged" << i << newState;
    //            pin->setOutState( newState );
    //        }
    //        m_state = m_arena->data32;
    //        //qDebug() << "o"<<m_state<<eventEnter;
    //    } break;
    //    case ESP_GPIO_DIR:       // Set Direction
    //    {
    //        for( uint i=0; i<32; ++i ) // GPIO pads 34-39 are input-only.
    //        {
    //            IoPin* pin =  m_espPad[i];
    //            if( !pin ) continue;
    //            uint32_t mask =1<<i;
    //            //if( m_arena->inputEn & mask ) continue;

    //            uint32_t oldDirec = m_direc & mask;
    //            uint32_t newDirec = m_arena->data32 & mask;
    //            if( oldDirec == newDirec ) continue;

    //            if( newDirec ) pin->setPinMode( output );
    //            else           pin->setPinMode( input );
    //            //qDebug() << "dirChanged" << i << newDirec;
    //            //pin->changeCallBack( this, !newDirec ); // CallBack only on Inputs
    //        }
    //        m_direc = m_arena->data32;
    //    } break;
    //    case ESP_GPIO_IN:                  // Read Inputs
    //    {
    //        readInputs();
    //        //m_arena->simuAction= 0;
    //    } break;
    //    case ESP_IOMUX:
    //    {
    //        // Sleep bits 0-6
    //        // PD bit 7
    //        // PU bit 8
    //        // IE bit 9
    //        // Drive bits 10-11
    //        // function bits 12-14

    //        int pin = m_arena->data8;
    //        esp32Pin* ioPin =  m_espPad[pin];
    //        if( !ioPin ) return;

    //        uint64_t value = m_arena->data32;

    //        ioPin->setIoMuxFunc( value );

    //    } break;
    //    case ESP_MATRIX_IN:  matrixFunc( 0 );
    //        break;
    //    case ESP_MATRIX_OUT: matrixFunc( 1 );
    //        break;
    //    case SIM_I2C:
    //    {
    //        uint16_t id = m_arena->data16;
    //        //qDebug() << "Stm32::doAction SIM_I2C:"<< id ;
    //        if( id < m_i2cN ) m_i2cs[id]->doAction();
    //    } break;
    //    case SIM_SPI:
    //    {
    //        uint16_t id = m_arena->data16;
    //        //qDebug() << "Stm32::doAction SIM_SPI:"<< id ;
    //        if( id < m_spiN ) m_spis[id]->doAction();
    //    }break;
    //    case SIM_USART:
    //    {
    //        uint16_t id = m_arena->data16;
    //        //qDebug() << "Stm32::doAction SIM_USART:"<< id ;
    //        if( id < m_usartN ) m_usarts[id]->doAction();
    //    }break;
    //}
}

Pin* Esp32::addPin( QString id, QString type, QString label,
                        int n, int x, int y, int angle, int length, int space )
{
    IoPin* pin = nullptr;

    if( type.contains("rst") )
    {
        pin = new IoPin( angle, QPoint(x, y), m_id+"-"+id, n-1, this, input );
        m_rstPin = pin;
        m_rstPin->setOutHighV( 3.3 );
        m_rstPin->setPullup( 1e5 );
        m_rstPin->setInputHighV( 0.65 );
        m_rstPin->setInputLowV( 0.65 );
    }else{
        n = id.right(2).toInt();
        m_espPad[n] = new esp32Pin( n, id, this, m_dummyPin ); // m_ioPin.at(n)
        pin = m_espPad[n];
    }
    //qDebug() << n << id << label << type;
    //QColor color = Qt::black;
    //if( !m_isLS ) color = QColor( 250, 250, 200 );

    pin->setPos( x, y );
    pin->setPinAngle( angle );
    pin->setLength( length );
    pin->setSpace( space );
    pin->setLabelText( label );
    //pin->setLabelColor( color );
    pin->setFlag( QGraphicsItem::ItemStacksBehindParent, true );
    return pin;
}

void Esp32::createPins()
{
    m_gpioSize = 40;
    m_ioPin.resize( 40, m_dummyPin ); // 20,24,28,29,30,31=NULL

    for( int i=0; i<40; ++i ) m_espPad[i] = nullptr;

    setPackageFile("./data/esp32/esp32.package");

    createMatrix();
    createIoMux();
}

void Esp32::createMatrix()
{
    // IN
    m_matrixIn[0]   = { nullptr, nullptr                       , "Ck0"  }; // SPICLK
    m_matrixIn[1]   = { nullptr, nullptr                       , "Mi0"  }; // SPIQ
    m_matrixIn[2]   = { nullptr, nullptr                       , "Mo0" };  // SPID

    m_matrixIn[3]   = { nullptr, nullptr, "SPIHD" }; // SPIHD
    m_matrixIn[4]   = { nullptr, nullptr, "SPIWP" }; // SPIWP

    m_matrixIn[5]   = { nullptr, nullptr                       , "Ss0" }; // SPICS0

    m_matrixIn[6]   = { nullptr, nullptr, "SPICS1" }; // SPICS1
    m_matrixIn[7]   = { nullptr, nullptr, "SPICS2" }; // SPICS2

    m_matrixIn[8]   = { m_spis[0], m_spis[0]->getCkPinPtr()  , "Ck2" }; // HSPICLK
    m_matrixIn[9]   = { m_spis[0], m_spis[0]->getMiPinPtr()  , "Mi2" }; // HSPIQ
    m_matrixIn[10]  = { m_spis[0], m_spis[0]->getMoPinPtr()  , "Mo2" }; // HSPID
    m_matrixIn[11]  = { m_spis[0], m_spis[0]->getSsPinPtr()  , "Ss2" }; // HSPICS0

    m_matrixIn[12]  = { nullptr, nullptr, "HSPIHD" }; // HSPIHD
    m_matrixIn[13]  = { nullptr, nullptr, "HSPIWP" }; // HSPIWP

    m_matrixIn[14]  = { m_usarts[0], m_usarts[0]->getRxPinPointer(), "Rx0" }; // U0RXD

    m_matrixIn[15]  = { nullptr, nullptr, "U0CTS" }; // U0CTS
    m_matrixIn[16]  = { nullptr, nullptr, "U0DSR" }; // U0DSR

    m_matrixIn[17]  = { m_usarts[1], m_usarts[1]->getRxPinPointer(), "Rx1" }; // U1RXD

    m_matrixIn[18]  = { nullptr, nullptr, "U1CTS" }; // U1CTS
    m_matrixIn[19]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[20]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[21]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[22]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[23]  = { nullptr, nullptr, "I2S0O_BCK" }; // I2S0O_BCK
    m_matrixIn[24]  = { nullptr, nullptr, "I2S1O_BCK" }; // I2S1O_BCK
    m_matrixIn[25]  = { nullptr, nullptr, "I2S0O_WS" }; // I2S0O_WS
    m_matrixIn[26]  = { nullptr, nullptr, "I2S1O_WS" }; // I2S1O_WS
    m_matrixIn[27]  = { nullptr, nullptr, "I2S0I_BCK" }; // I2S0I_BCK
    m_matrixIn[28]  = { nullptr, nullptr, "I2S0I_WS" }; // I2S0I_WS

    m_matrixIn[29]  = { m_i2cs[0], m_i2cs[0]->getSclPinPtr() , "Scl0"}; // I2CEXT0_SCL
    m_matrixIn[30]  = { m_i2cs[0], m_i2cs[0]->getSdaPinPtr() , "Sda0"}; // I2CEXT0_SDA

    m_matrixIn[31]  = { nullptr, nullptr, "PWM0_SYNC0" }; // PWM0_SYNC0
    m_matrixIn[32]  = { nullptr, nullptr, "PWM0_SYNC1" }; // PWM0_SYNC1
    m_matrixIn[33]  = { nullptr, nullptr, "PWM0_SYNC2" }; // PWM0_SYNC2
    m_matrixIn[34]  = { nullptr, nullptr, "PWM0_F0" }; // PWM0_F0
    m_matrixIn[35]  = { nullptr, nullptr, "PWM0_F1" }; // PWM0_F1
    m_matrixIn[36]  = { nullptr, nullptr, "PWM0_F2" }; // PWM0_F2
    m_matrixIn[37]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[38]  = { nullptr, nullptr, "GPIO_BT_PRIORITY" }; // GPIO_BT_PRIORITY
    m_matrixIn[39]  = { nullptr, nullptr, "PCNT_SIG_CH0_IN0" }; // PCNT_SIG_CH0_IN0
    m_matrixIn[40]  = { nullptr, nullptr, "PCNT_SIG_CH1_IN0" }; // PCNT_SIG_CH1_IN0
    m_matrixIn[41]  = { nullptr, nullptr, "PCNT_CTRL_CH0_IN0"}; // PCNT_CTRL_CH0_IN0
    m_matrixIn[42]  = { nullptr, nullptr, "PCNT_CTRL_CH1_IN0"}; // PCNT_CTRL_CH1_IN0
    m_matrixIn[43]  = { nullptr, nullptr, "PCNT_SIG_CH0_IN1" }; // PCNT_SIG_CH0_IN1
    m_matrixIn[44]  = { nullptr, nullptr, "PCNT_SIG_CH1_IN1" }; // PCNT_SIG_CH1_IN1
    m_matrixIn[45]  = { nullptr, nullptr, "PCNT_CTRL_CH0_IN1"}; // PCNT_CTRL_CH0_IN1
    m_matrixIn[46]  = { nullptr, nullptr, "PCNT_CTRL_CH1_IN1"}; // PCNT_CTRL_CH1_IN1
    m_matrixIn[47]  = { nullptr, nullptr, "PCNT_SIG_CH0_IN2" }; // PCNT_SIG_CH0_IN2
    m_matrixIn[48]  = { nullptr, nullptr, "PCNT_SIG_CH1_IN2" }; // PCNT_SIG_CH1_IN2
    m_matrixIn[49]  = { nullptr, nullptr, "PCNT_CTRL_CH0_IN2"}; // PCNT_CTRL_CH0_IN2
    m_matrixIn[50]  = { nullptr, nullptr, "PCNT_CTRL_CH1_IN2"}; // PCNT_CTRL_CH1_IN2
    m_matrixIn[51]  = { nullptr, nullptr, "PCNT_SIG_CH0_IN3" }; // PCNT_SIG_CH0_IN3
    m_matrixIn[52]  = { nullptr, nullptr, "PCNT_SIG_CH1_IN3" }; // PCNT_SIG_CH1_IN3
    m_matrixIn[53]  = { nullptr, nullptr, "PCNT_CTRL_CH0_IN3"}; // PCNT_CTRL_CH0_IN3
    m_matrixIn[54]  = { nullptr, nullptr, "PCNT_CTRL_CH1_IN3"}; // PCNT_CTRL_CH1_IN3
    m_matrixIn[55]  = { nullptr, nullptr, "PCNT_SIG_CH0_IN4" }; // PCNT_SIG_CH0_IN4
    m_matrixIn[56]  = { nullptr, nullptr, "PCNT_SIG_CH1_IN4" }; // PCNT_SIG_CH1_IN4
    m_matrixIn[57]  = { nullptr, nullptr, "PCNT_CTRL_CH0_IN4"}; // PCNT_CTRL_CH0_IN4
    m_matrixIn[58]  = { nullptr, nullptr, "PCNT_CTRL_CH1_IN4"}; // PCNT_CTRL_CH1_IN4
    m_matrixIn[59]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[60]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[61]  = { nullptr, nullptr, "HSPICS1" }; // HSPICS1
    m_matrixIn[62]  = { nullptr, nullptr, "HSPICS2" }; // HSPICS2
    m_matrixIn[63]  = { nullptr, nullptr, "VSPICLK" }; // VSPICLK

    m_matrixIn[64]  = { m_spis[1], m_spis[1]->getMiPinPtr()  , "Mi3" };  // VSPIQ

    m_matrixIn[65]  = { nullptr, nullptr, "VSPID" }; // VSPID
    m_matrixIn[66]  = { nullptr, nullptr, "VSPIHD" }; // VSPIHD
    m_matrixIn[67]  = { nullptr, nullptr, "VSPIWP" }; // VSPIWP
    m_matrixIn[68]  = { nullptr, nullptr, "VSPICS0" }; // VSPICS0
    m_matrixIn[69]  = { nullptr, nullptr, "VSPICS1" }; // VSPICS1
    m_matrixIn[70]  = { nullptr, nullptr, "VSPICS2" }; // VSPICS2
    m_matrixIn[71]  = { nullptr, nullptr, "PCNT_SIG_CH0_5" }; // PCNT_SIG_CH0_IN5
    m_matrixIn[72]  = { nullptr, nullptr, "PCNT_SIG_CH1_5" }; // PCNT_SIG_CH1_IN5
    m_matrixIn[73]  = { nullptr, nullptr, "PCNT_CTRL_CH0_5"}; // PCNT_CTRL_CH0_IN5
    m_matrixIn[74]  = { nullptr, nullptr, "PCNT_CTRL_CH1_5"}; // PCNT_CTRL_CH1_IN5
    m_matrixIn[75]  = { nullptr, nullptr, "PCNT_SIG_CH0_6" }; // PCNT_SIG_CH0_IN6
    m_matrixIn[76]  = { nullptr, nullptr, "PCNT_SIG_CH1_6" }; // PCNT_SIG_CH1_IN6
    m_matrixIn[77]  = { nullptr, nullptr, "PCNT_CTRL_CH0_6"}; // PCNT_CTRL_CH0_IN6
    m_matrixIn[78]  = { nullptr, nullptr, "PCNT_CTRL_CH1_6"}; // PCNT_CTRL_CH1_IN6
    m_matrixIn[79]  = { nullptr, nullptr, "PCNT_SIG_CH0_7" }; // PCNT_SIG_CH0_IN7
    m_matrixIn[80]  = { nullptr, nullptr, "PCNT_SIG_CH1_7" }; // PCNT_SIG_CH1_IN7
    m_matrixIn[81]  = { nullptr, nullptr, "PCNT_CTRL_CH0_7"}; // PCNT_CTRL_CH0_IN7
    m_matrixIn[82]  = { nullptr, nullptr, "PCNT_CTRL_CH1_7"}; // PCNT_CTRL_CH1_IN7
    m_matrixIn[83]  = { nullptr, nullptr, "RMT_0" }; // RMT_SIG_IN0
    m_matrixIn[84]  = { nullptr, nullptr, "RMT_1" }; // RMT_SIG_IN1
    m_matrixIn[85]  = { nullptr, nullptr, "RMT_2" }; // RMT_SIG_IN2
    m_matrixIn[86]  = { nullptr, nullptr, "RMT_3" }; // RMT_SIG_IN3
    m_matrixIn[87]  = { nullptr, nullptr, "RMT_4" }; // RMT_SIG_IN4
    m_matrixIn[88]  = { nullptr, nullptr, "RMT_5" }; // RMT_SIG_IN5
    m_matrixIn[89]  = { nullptr, nullptr, "RMT_6" }; // RMT_SIG_IN6
    m_matrixIn[90]  = { nullptr, nullptr, "RMT_7" }; // RMT_SIG_IN7
    m_matrixIn[91]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[92]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[93]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[94]  = { nullptr, nullptr, "TWAI_RX" }; // TWAI_RX

    m_matrixIn[95]  = { m_i2cs[1], m_i2cs[1]->getSclPinPtr() , "Scl1"}; // I2CEXT1_SCL
    m_matrixIn[96]  = { m_i2cs[1], m_i2cs[1]->getSdaPinPtr() , "Sda1"}; // I2CEXT1_SDA

    m_matrixIn[97]  = { nullptr, nullptr, "HOST_CARD_DETECT_N_1" }; // HOST_CARD_DETECT_N_1
    m_matrixIn[98]  = { nullptr, nullptr, "HOST_CARD_DETECT_N_2" }; // HOST_CARD_DETECT_N_2
    m_matrixIn[99]  = { nullptr, nullptr, "HOST_CARD_WRITE_PRT_1"}; // HOST_CARD_WRITE_PRT_1
    m_matrixIn[100] = { nullptr, nullptr, "HOST_CARD_WRITE_PRT_2"}; // HOST_CARD_WRITE_PRT_2
    m_matrixIn[101] = { nullptr, nullptr, "HOST_CARD_INT_N_1" }; // HOST_CARD_INT_N_1
    m_matrixIn[102] = { nullptr, nullptr, "HOST_CARD_INT_N_2" }; // HOST_CARD_INT_N_2
    m_matrixIn[103] = { nullptr, nullptr, "PWM1_SYNC0" }; // PWM1_SYNC0_IN
    m_matrixIn[104] = { nullptr, nullptr, "PWM1_SYNC1" }; // PWM1_SYNC1_IN
    m_matrixIn[105] = { nullptr, nullptr, "PWM1_SYNC2" }; // PWM1_SYNC2_IN
    m_matrixIn[106] = { nullptr, nullptr, "PWM1_F0" }; // PWM1_F0_IN
    m_matrixIn[107] = { nullptr, nullptr, "PWM1_F1" }; // PWM1_F1_IN
    m_matrixIn[108] = { nullptr, nullptr, "PWM1_F2" }; // PWM1_F2_IN
    m_matrixIn[109] = { nullptr, nullptr, "PWM0_CAP0" }; // PWM0_CAP0_IN
    m_matrixIn[110] = { nullptr, nullptr, "PWM0_CAP1" }; // PWM0_CAP1_IN
    m_matrixIn[111] = { nullptr, nullptr, "PWM0_CAP2" }; // PWM0_CAP2_IN
    m_matrixIn[112] = { nullptr, nullptr, "PWM1_CAP0" }; // PWM1_CAP0_IN
    m_matrixIn[113] = { nullptr, nullptr, "PWM1_CAP1" }; // PWM1_CAP1_IN
    m_matrixIn[114] = { nullptr, nullptr, "PWM1_CAP2" }; // PWM1_CAP2_IN
    m_matrixIn[115] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[116] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[117] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[118] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[119] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[120] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[121] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[122] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[123] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[124] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[125] = { nullptr, nullptr, "---" }; // (not assigned)

    m_matrixIn[126] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[127] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[128] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[129] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[130] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[131] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[132] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[133] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[134] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[135] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[136] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[137] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[138] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[139] = { nullptr, nullptr, "---" }; // (not assigned)

    m_matrixIn[140] = { nullptr, nullptr, "I2S0I_D0" }; // I2S0I_DATA_IN0
    m_matrixIn[141] = { nullptr, nullptr, "I2S0I_D1" }; // I2S0I_DATA_IN1
    m_matrixIn[142] = { nullptr, nullptr, "I2S0I_D2" }; // I2S0I_DATA_IN2
    m_matrixIn[143] = { nullptr, nullptr, "I2S0I_D3" }; // I2S0I_DATA_IN3
    m_matrixIn[144] = { nullptr, nullptr, "I2S0I_D4" }; // I2S0I_DATA_IN4
    m_matrixIn[145] = { nullptr, nullptr, "I2S0I_D5" }; // I2S0I_DATA_IN5
    m_matrixIn[146] = { nullptr, nullptr, "I2S0I_D6" }; // I2S0I_DATA_IN6
    m_matrixIn[147] = { nullptr, nullptr, "I2S0I_D7" }; // I2S0I_DATA_IN7
    m_matrixIn[148] = { nullptr, nullptr, "I2S0I_D8" }; // I2S0I_DATA_IN8
    m_matrixIn[149] = { nullptr, nullptr, "I2S0I_D9" }; // I2S0I_DATA_IN9
    m_matrixIn[150] = { nullptr, nullptr, "I2S0I_D10"}; // I2S0I_DATA_IN10
    m_matrixIn[151] = { nullptr, nullptr, "I2S0I_D11"}; // I2S0I_DATA_IN11
    m_matrixIn[152] = { nullptr, nullptr, "I2S0I_D12"}; // I2S0I_DATA_IN12
    m_matrixIn[153] = { nullptr, nullptr, "I2S0I_D13"}; // I2S0I_DATA_IN13
    m_matrixIn[154] = { nullptr, nullptr, "I2S0I_D14"}; // I2S0I_DATA_IN14
    m_matrixIn[155] = { nullptr, nullptr, "I2S0I_D15"}; // I2S0I_DATA_IN15
    m_matrixIn[156] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[157] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[158] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[159] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[160] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[161] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[162] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[163] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[164] = { nullptr, nullptr, "I2S1I_BCK"}; // I2S1I_BCK_IN
    m_matrixIn[165] = { nullptr, nullptr, "I2S1I_WS" }; // I2S1I_WS_IN
    m_matrixIn[166] = { nullptr, nullptr, "I2S1I_D0" }; // I2S1I_DATA_IN0
    m_matrixIn[167] = { nullptr, nullptr, "I2S1I_D1" }; // I2S1I_DATA_IN1
    m_matrixIn[168] = { nullptr, nullptr, "I2S1I_D2" }; // I2S1I_DATA_IN2
    m_matrixIn[169] = { nullptr, nullptr, "I2S1I_D3" }; // I2S1I_DATA_IN3
    m_matrixIn[170] = { nullptr, nullptr, "I2S1I_D4" }; // I2S1I_DATA_IN4
    m_matrixIn[171] = { nullptr, nullptr, "I2S1I_D5" }; // I2S1I_DATA_IN5
    m_matrixIn[172] = { nullptr, nullptr, "I2S1I_D6" }; // I2S1I_DATA_IN6
    m_matrixIn[173] = { nullptr, nullptr, "I2S1I_D7" }; // I2S1I_DATA_IN7
    m_matrixIn[174] = { nullptr, nullptr, "I2S1I_D8" }; // I2S1I_DATA_IN8
    m_matrixIn[175] = { nullptr, nullptr, "I2S1I_D9" }; // I2S1I_DATA_IN9
    m_matrixIn[176] = { nullptr, nullptr, "I2S1I_D10"}; // I2S1I_DATA_IN10
    m_matrixIn[177] = { nullptr, nullptr, "I2S1I_D11"}; // I2S1I_DATA_IN11
    m_matrixIn[178] = { nullptr, nullptr, "I2S1I_D12"}; // I2S1I_DATA_IN12
    m_matrixIn[179] = { nullptr, nullptr, "I2S1I_D13"}; // I2S1I_DATA_IN13
    m_matrixIn[180] = { nullptr, nullptr, "I2S1I_D14"}; // I2S1I_DATA_IN14
    m_matrixIn[181] = { nullptr, nullptr, "I2S1I_D15"}; // I2S1I_DATA_IN15
    m_matrixIn[182] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[183] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[184] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[185] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[186] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[187] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[188] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[189] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[190] = { nullptr, nullptr, "I2S0I_H_SYNC"  }; // I2S0I_H_SYNC
    m_matrixIn[191] = { nullptr, nullptr, "I2S0I_V_SYNC"  }; // I2S0I_V_SYNC
    m_matrixIn[192] = { nullptr, nullptr, "I2S0I_H_ENABLE"}; // I2S0I_H_ENABLE
    m_matrixIn[193] = { nullptr, nullptr, "I2S1I_H_SYNC"  }; // I2S1I_H_SYNC
    m_matrixIn[194] = { nullptr, nullptr, "I2S1I_V_SYNC"  }; // I2S1I_V_SYNC
    m_matrixIn[195] = { nullptr, nullptr, "I2S1I_H_ENABLE"}; // I2S1I_H_ENABLE
    m_matrixIn[196] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[197] = { nullptr, nullptr, "---" }; // (not assigned)

    m_matrixIn[198] = { m_usarts[2], m_usarts[2]->getRxPinPointer(), "Rx2" }; // U2RXD

    m_matrixIn[199] = { nullptr, nullptr, "U2CTS" }; // U2CTS_IN
    m_matrixIn[200] = { nullptr, nullptr, "EMAC_MDC_I" }; // EMAC_MDC_I
    m_matrixIn[201] = { nullptr, nullptr, "EMAC_MDI_I" }; // EMAC_MDI_I
    m_matrixIn[202] = { nullptr, nullptr, "EMAC_CRS_I" }; // EMAC_CRS_I
    m_matrixIn[203] = { nullptr, nullptr, "EMAC_COL_I" }; // EMAC_COL_I
    m_matrixIn[204] = { nullptr, nullptr, "PCMFSYNC" }; // PCMFSYNC_IN
    m_matrixIn[205] = { nullptr, nullptr, "PCMCLK" }; // PCMCLK_IN
    m_matrixIn[206] = { nullptr, nullptr, "PCMDIN" }; // PCMDIN
    m_matrixIn[207] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[208] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[209] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[210] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[211] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[212] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[213] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[214] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[215] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[216] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[217] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[218] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[219] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[220] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[221] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[222] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[223] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[224] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[225] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[226] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[227] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[228] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[229] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[230] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[231] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[232] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[233] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[234] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[235] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[236] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[237] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[238] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[239] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[240] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[241] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[242] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[243] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[244] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[245] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[246] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[247] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[248] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[249] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[250] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[251] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[252] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[253] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[254] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixIn[255] = { nullptr, nullptr, "---" }; // (not assigned)


    // OUT
    m_matrixOut[0]   = { nullptr, nullptr                      , "Ck0"  }; // SPICLK
    m_matrixOut[1]   = { nullptr, nullptr                      , "Mi0"  }; // SPIQ
    m_matrixOut[2]   = { nullptr, nullptr                      , "Mo0"  };  // SPID

    m_matrixOut[3]   = { nullptr, nullptr, "SPIHD" }; // SPIHD
    m_matrixOut[4]   = { nullptr, nullptr, "SPIWP" }; // SPIWP

    m_matrixOut[5]   = { nullptr, nullptr                      , "Ss0" }; // SPICS0

    m_matrixOut[6]   = { nullptr, nullptr, "SPICS1" }; // SPICS1
    m_matrixOut[7]   = { nullptr, nullptr, "SPICS2" }; // SPICS2

    m_matrixOut[8]  = { m_spis[0], m_spis[0]->getCkPinPtr()  , "Ck2" }; // HSPICLK
    m_matrixOut[9]  = { m_spis[0], m_spis[0]->getMiPinPtr()  , "Mi2" }; // HSPIQ
    m_matrixOut[10] = { m_spis[0], m_spis[0]->getMoPinPtr()  , "Mo2" }; // HSPID
    m_matrixOut[11] = { m_spis[0], m_spis[0]->getSsPinPtr()  , "Ss2" }; // HSPICS0

    m_matrixOut[12]  = { nullptr, nullptr, "HSPIHD" }; // HSPIHD
    m_matrixOut[13]  = { nullptr, nullptr, "HSPIWP" }; // HSPIWP

    m_matrixOut[14] = { m_usarts[0], m_usarts[0]->getTxPinPointer(), "Tx0" }; // U0TXD

    m_matrixOut[15]  = { nullptr, nullptr, "U0RTS" }; // U0RTS
    m_matrixOut[16]  = { nullptr, nullptr, "U0DTR" }; // U0DTR

    m_matrixOut[17] = { m_usarts[1], m_usarts[1]->getTxPinPointer(), "Tx1" }; // U1TXD

    m_matrixOut[18]  = { nullptr, nullptr, "U1RTS" }; // U1RTS
    m_matrixOut[19]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[20]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[21]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[22]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[23]  = { nullptr, nullptr, "I2S0O_BCK" }; // I2S0O_BCK
    m_matrixOut[24]  = { nullptr, nullptr, "I2S1O_BCK" }; // I2S1O_BCK
    m_matrixOut[25]  = { nullptr, nullptr, "I2S0O_WS" }; // I2S0O_WS
    m_matrixOut[26]  = { nullptr, nullptr, "I2S1O_WS" }; // I2S1O_WS
    m_matrixOut[27]  = { nullptr, nullptr, "I2S0I_BCK" }; // I2S0I_BCK
    m_matrixOut[28]  = { nullptr, nullptr, "I2S0I_WS" }; // I2S0I_WS

    m_matrixOut[29] = { m_i2cs[0], m_i2cs[0]->getSclPinPtr() , "Scl0"}; // I2CEXT0_SCL
    m_matrixOut[30] = { m_i2cs[0], m_i2cs[0]->getSdaPinPtr() , "Sda0"}; // I2CEXT0_SDA

    m_matrixOut[31]  = { nullptr, nullptr, "SDIO_TOHOST_INT" }; // SDIO_TOHOST_INT
    m_matrixOut[32]  = { nullptr, nullptr, "PWM0_OUT0A" }; // PWM0_OUT0A
    m_matrixOut[33]  = { nullptr, nullptr, "PWM0_OUT0B" }; // PWM0_OUT0B
    m_matrixOut[34]  = { nullptr, nullptr, "PWM0_OUT1A" }; // PWM0_OUT1A
    m_matrixOut[35]  = { nullptr, nullptr, "PWM0_OUT1B" }; // PWM0_OUT1B
    m_matrixOut[36]  = { nullptr, nullptr, "PWM0_OUT2A" }; // PWM0_OUT2A
    m_matrixOut[37]  = { nullptr, nullptr, "PWM0_OUT2B" }; // PWM0_OUT2B
    m_matrixOut[38]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[39]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[40]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[41]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[42]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[43]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[44]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[45]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[46]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[47]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[48]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[49]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[50]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[51]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[52]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[53]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[54]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[55]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[56]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[57]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[58]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[59]  = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[60]  = { nullptr, nullptr, "---" }; // (not assigned)

    m_matrixOut[61] = { nullptr, nullptr, "Spi2CS1" }; // spis[0]->cs_pin[1] // HSPICS1
    m_matrixOut[62] = { nullptr, nullptr, "Spi2CS2" }; // spis[0]->cs_pin[2] // HSPICS2

    m_matrixOut[63] = { m_spis[1], m_spis[1]->getCkPinPtr()  , "Ck3" }; // VSPICLK
    m_matrixOut[64] = { m_spis[1], m_spis[1]->getMiPinPtr()  , "Mi3" }; // VSPIQ
    m_matrixOut[65] = { m_spis[1], m_spis[1]->getMoPinPtr()  , "Mo3" }; // VSPID

    m_matrixOut[66]  = { nullptr, nullptr, "VSPIHD" }; // VSPIHD
    m_matrixOut[67]  = { nullptr, nullptr, "VSPIWP" }; // VSPIWP

    m_matrixOut[68] = { m_spis[1], m_spis[1]->getSsPinPtr()  , "Ss3" }; // VSPICS0

    m_matrixOut[69] = { nullptr, nullptr, "Spi3CS1" }; // spis[1]->cs_pin[1] // VSPICS1
    m_matrixOut[70] = { nullptr, nullptr, "Spi3CS2" }; // spis[1]->cs_pin[2] // VSPICS2

    m_matrixOut[71] = { nullptr, nullptr                       , "LeL0" }; // ledc_hs_sig_out0
    m_matrixOut[72] = { nullptr, nullptr                       , "LeL1" }; // ledc_hs_sig_out1
    m_matrixOut[73] = { nullptr, nullptr                       , "LeL2" }; // ledc_hs_sig_out2
    m_matrixOut[74] = { nullptr, nullptr                       , "LeL3" }; // ledc_hs_sig_out3
    m_matrixOut[75] = { nullptr, nullptr                       , "LeL4" }; // ledc_hs_sig_out4
    m_matrixOut[76] = { nullptr, nullptr                       , "LeL5" }; // ledc_hs_sig_out5
    m_matrixOut[77] = { nullptr, nullptr                       , "LeL6" }; // ledc_hs_sig_out6
    m_matrixOut[78] = { nullptr, nullptr                       , "LeL7" }; // ledc_hs_sig_out7
    m_matrixOut[79] = { nullptr, nullptr                       , "LeH0" }; // ledc_ls_sig_out0
    m_matrixOut[80] = { nullptr, nullptr                       , "LeH1" }; // ledc_ls_sig_out1
    m_matrixOut[81] = { nullptr, nullptr                       , "LeH2" }; // ledc_ls_sig_out2
    m_matrixOut[82] = { nullptr, nullptr                       , "LeH3" }; // ledc_ls_sig_out3
    m_matrixOut[83] = { nullptr, nullptr                       , "LeH4" }; // ledc_ls_sig_out4
    m_matrixOut[84] = { nullptr, nullptr                       , "LeH5" }; // ledc_ls_sig_out5
    m_matrixOut[85] = { nullptr, nullptr                       , "LeH6" }; // ledc_ls_sig_out6
    m_matrixOut[86] = { nullptr, nullptr                       , "LeH7" }; // ledc_ls_sig_out7
    m_matrixOut[87] = { nullptr, nullptr                       , "Rmt0" }; // rmt_sig_out0
    m_matrixOut[88] = { nullptr, nullptr                       , "Rmt1" }; // rmt_sig_out1
    m_matrixOut[89] = { nullptr, nullptr                       , "Rmt2" }; // rmt_sig_out2
    m_matrixOut[90] = { nullptr, nullptr                       , "Rmt3" }; // rmt_sig_out3
    m_matrixOut[91] = { nullptr, nullptr                       , "Rmt4" }; // rmt_sig_out4
    m_matrixOut[92] = { nullptr, nullptr                       , "Rmt5" }; // rmt_sig_out5
    m_matrixOut[93] = { nullptr, nullptr                       , "Rmt6" }; // rmt_sig_out6
    m_matrixOut[94] = { nullptr, nullptr                       , "Rmt7" }; // rmt_sig_out7
    m_matrixOut[95]  = { m_i2cs[1], m_i2cs[1]->getSclPinPtr()  , "Scl1"}; // I2CEXT1_SCL
    m_matrixOut[96]  = { m_i2cs[1], m_i2cs[1]->getSdaPinPtr()  , "Sda1"}; // I2CEXT1_SDA

    m_matrixOut[97]  = { nullptr, nullptr, "HOST_CCMD_OD_PULLUP_EN_N" }; // HOST_CCMD_OD_PULLUP_EN_N
    m_matrixOut[98]  = { nullptr, nullptr, "HOST_RST_N_1" }; // HOST_RST_N_1
    m_matrixOut[99]  = { nullptr, nullptr, "HOST_RST_N_2" }; // HOST_RST_N_2
    m_matrixOut[100] = { nullptr, nullptr, "GPIO_SD0" }; // GPIO_SD0_OUT
    m_matrixOut[101] = { nullptr, nullptr, "GPIO_SD1" }; // GPIO_SD1_OUT
    m_matrixOut[102] = { nullptr, nullptr, "GPIO_SD2" }; // GPIO_SD2_OUT
    m_matrixOut[103] = { nullptr, nullptr, "GPIO_SD3" }; // GPIO_SD3_OUT
    m_matrixOut[104] = { nullptr, nullptr, "GPIO_SD4" }; // GPIO_SD4_OUT
    m_matrixOut[105] = { nullptr, nullptr, "GPIO_SD5" }; // GPIO_SD5_OUT
    m_matrixOut[106] = { nullptr, nullptr, "GPIO_SD6" }; // GPIO_SD6_OUT
    m_matrixOut[107] = { nullptr, nullptr, "GPIO_SD7" }; // GPIO_SD7_OUT
    m_matrixOut[108] = { nullptr, nullptr, "PWM1_0A" }; // PWM1_OUT0A
    m_matrixOut[109] = { nullptr, nullptr, "PWM1_0B" }; // PWM1_OUT0B
    m_matrixOut[110] = { nullptr, nullptr, "PWM1_1A" }; // PWM1_OUT1A
    m_matrixOut[111] = { nullptr, nullptr, "PWM1_1B" }; // PWM1_OUT1B
    m_matrixOut[112] = { nullptr, nullptr, "PWM1_2A" }; // PWM1_OUT2A
    m_matrixOut[113] = { nullptr, nullptr, "PWM1_2B" }; // PWM1_OUT2B
    m_matrixOut[114] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[115] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[116] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[117] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[118] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[119] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[120] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[121] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[122] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[123] = { nullptr, nullptr, "CAN_TX" }; // CAN_TX
    m_matrixOut[124] = { nullptr, nullptr, "CAN_BUS_OFF_ON" }; // CAN_BUS_OFF_ON
    m_matrixOut[125] = { nullptr, nullptr, "CAN_CLKOUT" }; // CAN_CLKOUT
    m_matrixOut[126] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[127] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[128] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[129] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[130] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[131] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[132] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[133] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[134] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[135] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[136] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[137] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[138] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[139] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[140] = { nullptr, nullptr, "I2S0O_D0" }; // I2S0O_DATA_OUT0
    m_matrixOut[141] = { nullptr, nullptr, "I2S0O_D1" }; // I2S0O_DATA_OUT1
    m_matrixOut[142] = { nullptr, nullptr, "I2S0O_D2" }; // I2S0O_DATA_OUT2
    m_matrixOut[143] = { nullptr, nullptr, "I2S0O_D3" }; // I2S0O_DATA_OUT3
    m_matrixOut[144] = { nullptr, nullptr, "I2S0O_D4" }; // I2S0O_DATA_OUT4
    m_matrixOut[145] = { nullptr, nullptr, "I2S0O_D5" }; // I2S0O_DATA_OUT5
    m_matrixOut[146] = { nullptr, nullptr, "I2S0O_D6" }; // I2S0O_DATA_OUT6
    m_matrixOut[147] = { nullptr, nullptr, "I2S0O_D7" }; // I2S0O_DATA_OUT7
    m_matrixOut[148] = { nullptr, nullptr, "I2S0O_D8" }; // I2S0O_DATA_OUT8
    m_matrixOut[149] = { nullptr, nullptr, "I2S0O_D9" }; // I2S0O_DATA_OUT9
    m_matrixOut[150] = { nullptr, nullptr, "I2S0O_D10" }; // I2S0O_DATA_OUT10
    m_matrixOut[151] = { nullptr, nullptr, "I2S0O_D11" }; // I2S0O_DATA_OUT11
    m_matrixOut[152] = { nullptr, nullptr, "I2S0O_D12" }; // I2S0O_DATA_OUT12
    m_matrixOut[153] = { nullptr, nullptr, "I2S0O_D13" }; // I2S0O_DATA_OUT13
    m_matrixOut[154] = { nullptr, nullptr, "I2S0O_D14" }; // I2S0O_DATA_OUT14
    m_matrixOut[155] = { nullptr, nullptr, "I2S0O_D15" }; // I2S0O_DATA_OUT15
    m_matrixOut[156] = { nullptr, nullptr, "I2S0O_D16" }; // I2S0O_DATA_OUT16
    m_matrixOut[157] = { nullptr, nullptr, "I2S0O_D17" }; // I2S0O_DATA_OUT17
    m_matrixOut[158] = { nullptr, nullptr, "I2S0O_D18" }; // I2S0O_DATA_OUT18
    m_matrixOut[159] = { nullptr, nullptr, "I2S0O_D19" }; // I2S0O_DATA_OUT19
    m_matrixOut[160] = { nullptr, nullptr, "I2S0O_D20" }; // I2S0O_DATA_OUT20
    m_matrixOut[161] = { nullptr, nullptr, "I2S0O_D21" }; // I2S0O_DATA_OUT21
    m_matrixOut[162] = { nullptr, nullptr, "I2S0O_D22" }; // I2S0O_DATA_OUT22
    m_matrixOut[163] = { nullptr, nullptr, "I2S0O_D23" }; // I2S0O_DATA_OUT23
    m_matrixOut[164] = { nullptr, nullptr, "I2S1I_BCK" }; // I2S1I_BCK_OUT
    m_matrixOut[165] = { nullptr, nullptr, "I2S1I_WS" }; // I2S1I_WS_OUT
    m_matrixOut[166] = { nullptr, nullptr, "I2S1O_D0" }; // I2S1O_DATA_OUT0
    m_matrixOut[167] = { nullptr, nullptr, "I2S1O_D1" }; // I2S1O_DATA_OUT1
    m_matrixOut[168] = { nullptr, nullptr, "I2S1O_D2" }; // I2S1O_DATA_OUT2
    m_matrixOut[169] = { nullptr, nullptr, "I2S1O_D3" }; // I2S1O_DATA_OUT3
    m_matrixOut[170] = { nullptr, nullptr, "I2S1O_D4" }; // I2S1O_DATA_OUT4
    m_matrixOut[171] = { nullptr, nullptr, "I2S1O_D5" }; // I2S1O_DATA_OUT5
    m_matrixOut[172] = { nullptr, nullptr, "I2S1O_D6" }; // I2S1O_DATA_OUT6
    m_matrixOut[173] = { nullptr, nullptr, "I2S1O_D7" }; // I2S1O_DATA_OUT7
    m_matrixOut[174] = { nullptr, nullptr, "I2S1O_D8" }; // I2S1O_DATA_OUT8
    m_matrixOut[175] = { nullptr, nullptr, "I2S1O_D9" }; // I2S1O_DATA_OUT9
    m_matrixOut[176] = { nullptr, nullptr, "I2S1O_D10" }; // I2S1O_DATA_OUT10
    m_matrixOut[177] = { nullptr, nullptr, "I2S1O_D11" }; // I2S1O_DATA_OUT11
    m_matrixOut[178] = { nullptr, nullptr, "I2S1O_D12" }; // I2S1O_DATA_OUT12
    m_matrixOut[179] = { nullptr, nullptr, "I2S1O_D13" }; // I2S1O_DATA_OUT13
    m_matrixOut[180] = { nullptr, nullptr, "I2S1O_D14" }; // I2S1O_DATA_OUT14
    m_matrixOut[181] = { nullptr, nullptr, "I2S1O_D15" }; // I2S1O_DATA_OUT15
    m_matrixOut[182] = { nullptr, nullptr, "I2S1O_D16" }; // I2S1O_DATA_OUT16
    m_matrixOut[183] = { nullptr, nullptr, "I2S1O_D17" }; // I2S1O_DATA_OUT17
    m_matrixOut[184] = { nullptr, nullptr, "I2S1O_D18" }; // I2S1O_DATA_OUT18
    m_matrixOut[185] = { nullptr, nullptr, "I2S1O_D19" }; // I2S1O_DATA_OUT19
    m_matrixOut[186] = { nullptr, nullptr, "I2S1O_D20" }; // I2S1O_DATA_OUT20
    m_matrixOut[187] = { nullptr, nullptr, "I2S1O_D21" }; // I2S1O_DATA_OUT21
    m_matrixOut[188] = { nullptr, nullptr, "I2S1O_D22" }; // I2S1O_DATA_OUT22
    m_matrixOut[189] = { nullptr, nullptr, "I2S1O_D23" }; // I2S1O_DATA_OUT23
    m_matrixOut[190] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[191] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[192] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[193] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[194] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[195] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[196] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[197] = { nullptr, nullptr, "---" }; // (not assigned)

    m_matrixOut[198] = { m_usarts[2], m_usarts[2]->getTxPinPointer(), "Tx2" }; // U2RTXD

    m_matrixOut[199] = { nullptr, nullptr, "U2RTS_OUT" }; // U2RTS_OUT
    m_matrixOut[200] = { nullptr, nullptr, "EMAC_MDC_O" }; // EMAC_MDC_O
    m_matrixOut[201] = { nullptr, nullptr, "EMAC_MDO_O" }; // EMAC_MDO_O
    m_matrixOut[202] = { nullptr, nullptr, "EMAC_CRS_O" }; // EMAC_CRS_O
    m_matrixOut[203] = { nullptr, nullptr, "EMAC_COL_O" }; // EMAC_COL_O
    m_matrixOut[204] = { nullptr, nullptr, "BT_AUDIO0_IRQ" }; // BT_AUDIO0_IRQ
    m_matrixOut[205] = { nullptr, nullptr, "BT_AUDIO1_IRQ" }; // BT_AUDIO1_IRQ
    m_matrixOut[206] = { nullptr, nullptr, "BT_AUDIO2_IRQ" }; // BT_AUDIO2_IRQ
    m_matrixOut[207] = { nullptr, nullptr, "BLE_AUDIO0_IRQ" }; // BLE_AUDIO0_IRQ
    m_matrixOut[208] = { nullptr, nullptr, "BLE_AUDIO1_IRQ" }; // BLE_AUDIO1_IRQ
    m_matrixOut[209] = { nullptr, nullptr, "BLE_AUDIO2_IRQ" }; // BLE_AUDIO2_IRQ
    m_matrixOut[210] = { nullptr, nullptr, "PCMFSYNC_OUT" }; // PCMFSYNC_OUT
    m_matrixOut[211] = { nullptr, nullptr, "PCMCLK_OUT" }; // PCMCLK_OUT
    m_matrixOut[212] = { nullptr, nullptr, "PCMDOUT" }; // PCMDOUT
    m_matrixOut[213] = { nullptr, nullptr, "BLE_AUDIO_SYNC0_P" }; // BLE_AUDIO_SYNC0_P
    m_matrixOut[214] = { nullptr, nullptr, "BLE_AUDIO_SYNC1_P" }; // BLE_AUDIO_SYNC1_P
    m_matrixOut[215] = { nullptr, nullptr, "BLE_AUDIO_SYNC2_P" }; // BLE_AUDIO_SYNC2_P
    m_matrixOut[216] = { nullptr, nullptr, "ANT_SEL0" }; // ANT_SEL0
    m_matrixOut[217] = { nullptr, nullptr, "ANT_SEL1" }; // ANT_SEL1
    m_matrixOut[218] = { nullptr, nullptr, "ANT_SEL2" }; // ANT_SEL2
    m_matrixOut[219] = { nullptr, nullptr, "ANT_SEL3" }; // ANT_SEL3
    m_matrixOut[220] = { nullptr, nullptr, "ANT_SEL4" }; // ANT_SEL4
    m_matrixOut[221] = { nullptr, nullptr, "ANT_SEL5" }; // ANT_SEL5
    m_matrixOut[222] = { nullptr, nullptr, "ANT_SEL6" }; // ANT_SEL6
    m_matrixOut[223] = { nullptr, nullptr, "ANT_SEL7" }; // ANT_SEL7
    m_matrixOut[224] = { nullptr, nullptr, "SIG_IN_FUNC224" }; // SIG_IN_FUNC224
    m_matrixOut[225] = { nullptr, nullptr, "SIG_IN_FUNC225" }; // SIG_IN_FUNC225
    m_matrixOut[226] = { nullptr, nullptr, "SIG_IN_FUNC226" }; // SIG_IN_FUNC226
    m_matrixOut[227] = { nullptr, nullptr, "SIG_IN_FUNC227" }; // SIG_IN_FUNC227
    m_matrixOut[228] = { nullptr, nullptr, "SIG_IN_FUNC228" }; // SIG_IN_FUNC228
    m_matrixOut[229] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[230] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[231] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[232] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[233] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[234] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[235] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[236] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[237] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[238] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[239] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[240] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[241] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[242] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[243] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[244] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[245] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[246] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[247] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[248] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[249] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[250] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[251] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[252] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[253] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[254] = { nullptr, nullptr, "---" }; // (not assigned)
    m_matrixOut[255] = { nullptr, nullptr, "---" }; // (not assigned)
}

#define U0RXD       m_matrixIn[14]
#define U1RXD       m_matrixIn[17]
#define U2RXD       m_matrixIn[198]
//#define I2CEXT0_SCL m_matrixIn[29]
//#define I2CEXT0_SDA m_matrixIn[30]
//#define I2CEXT1_SCL m_matrixIn[95]
//#define I2CEXT1_SDA m_matrixIn[96]

#define SPICLK      m_matrixOut[0]
#define SPIQ        m_matrixOut[1]
#define SPID        m_matrixOut[2]
#define SPICS0      m_matrixOut[5]
#define U0TXD       m_matrixOut[14]
#define U1TXD       m_matrixOut[17]
#define U2TXD       m_matrixOut[198]
#define HSPICLK     m_matrixOut[8]
#define HSPIQ       m_matrixOut[9]
#define HSPID       m_matrixOut[10]
#define HSPICS0     m_matrixOut[11]
#define VSPICLK     m_matrixOut[63]
#define VSPIQ       m_matrixOut[64]
#define VSPID       m_matrixOut[65]
#define VSPICS0     m_matrixOut[68]

#define GPIO(x) { nullptr, &m_ioPin[x], ""}

void Esp32::createIoMux()
{
    funcPin DUMMY = { nullptr, nullptr, "---" };

    // 0:  GPIO0    CLK_OUT1 GPIO0  ----      ----      EMAC_TX_CLK
    m_espPad[0]->setIoMuxPins( { GPIO(0), DUMMY, GPIO(0), DUMMY, DUMMY, DUMMY} );

    // 1:  U0TXD    CLK_OUT3 GPIO1  ----      ----      EMAC_RXD2
    m_espPad[1]->setIoMuxPins( { U0TXD, DUMMY, DUMMY, DUMMY, DUMMY, DUMMY} );

    // 2:  GPIO2    HSPIWP   GPIO2  HS2_DATA0 SD_DATA0  ----
    m_espPad[2]->setIoMuxPins( { GPIO(2), DUMMY, GPIO(2), DUMMY, DUMMY, DUMMY} );

    // 3:  U0RXD    CLK_OUT2 GPIO3  ----      ----      ----
    m_espPad[3]->setIoMuxPins( { U0RXD, DUMMY, GPIO(3), DUMMY, DUMMY, DUMMY} );

    // 4:  GPIO4    HSPIHD   GPIO4  HS2_DATA1 SD_DATA1E MAC_TX_ER
    m_espPad[4]->setIoMuxPins( { GPIO(4), DUMMY, GPIO(4), DUMMY, DUMMY, DUMMY} );

    // 5:  GPIO5    VSPICS0  GPIO5  HS1_DATA6 ---- EMAC_RX_CLK
    m_espPad[5]->setIoMuxPins( { GPIO(5), VSPICS0, GPIO(5), DUMMY, DUMMY, DUMMY} );

    // 6:  SD_CLK   SPICLK   GPIO6  HS1_CLK   U1CTS ----
    m_espPad[6]->setIoMuxPins( { DUMMY, SPICLK , GPIO(6), DUMMY, DUMMY, DUMMY} ); //m_spis[0]->getClkPinPointer()

    // 7:  SD_DATA0 SPIQ     GPIO7  HS1_DATA0 U2RTS ----
    m_espPad[7]->setIoMuxPins( { DUMMY, SPIQ, GPIO(7), DUMMY, DUMMY, DUMMY} );

    // 8:  SD_DATA1 SPID     GPIO8  HS1_DATA1 U2CTS    ----
    m_espPad[8]->setIoMuxPins( { DUMMY, SPID, GPIO(8), DUMMY, DUMMY, DUMMY} );

    // 9:  SD_DATA2 SPIHD    GPIO9  HS1_DATA2 U1RXD    ----
    m_espPad[9]->setIoMuxPins( { DUMMY, DUMMY, GPIO(9), DUMMY, U1RXD, DUMMY} );

    // 10: SD_DATA3 SPIWP    GPIO10 HS1_DATA3 U1TXD    ----
    m_espPad[10]->setIoMuxPins( { DUMMY, DUMMY, GPIO(10), DUMMY, U1TXD, DUMMY} );

    // 11: SD_CMD   SPICS0   GPIO11 HS1_CMD   U1RTS    ----
    m_espPad[11]->setIoMuxPins( { DUMMY, SPICS0, GPIO(11), DUMMY, DUMMY, DUMMY} );

    // 12: MTDI     HSPIQ    GPIO12 HS2_DATA2 SD_DATA2 EMAC_TXD3
    m_espPad[12]->setIoMuxPins( { HSPIQ, DUMMY, GPIO(12), DUMMY, DUMMY, DUMMY} );

    // 13: MTCK     HSPID    GPIO13 HS2_DATA3 SD_DATA3 EMAC_RX_ER
    m_espPad[13]->setIoMuxPins( { HSPID, DUMMY, GPIO(13), DUMMY, DUMMY, DUMMY} );

    // 14: MTMS     HSPICLK  GPIO14 HS2_CLK   SD_CLK   EMAC_TXD2
    m_espPad[14]->setIoMuxPins( { DUMMY, HSPICLK, GPIO(14), DUMMY, DUMMY, DUMMY} );

    // 15: MTDO     HSPICS0  GPIO15 HS2_CMD   SD_CMD   EMAC_RXD3
    m_espPad[15]->setIoMuxPins( { HSPICS0, DUMMY, GPIO(15), DUMMY, DUMMY, DUMMY} );

    // 16: GPIO16   ----     GPIO16 HS1_DATA4 U2RXD    EMAC_CLK_OUT
    m_espPad[16]->setIoMuxPins( { GPIO(16), DUMMY, GPIO(16), DUMMY, U2RXD, DUMMY} );

    // 17: GPIO17   ----     GPIO17 HS1_DATA5 U2TXD    EMAC_CLK_180
    m_espPad[17]->setIoMuxPins( { GPIO(17), DUMMY, GPIO(17), DUMMY, U2TXD, DUMMY} );

    // 18: GPIO18   VSPICLK  GPIO18 HS1_DATA7 ----     ----
    m_espPad[18]->setIoMuxPins( { GPIO(18), VSPICLK, GPIO(18), DUMMY, DUMMY, DUMMY} );

    // 19: GPIO19   VSPIQ    GPIO19 U0CTS     ----     EMAC_TXD0
    m_espPad[19]->setIoMuxPins( { GPIO(19), VSPIQ , GPIO(19), DUMMY, DUMMY, DUMMY} );

    // 20:
    // 21: GPIO21   VSPIHD   GPIO21 ----       ----    EMAC_TX_EN
    m_espPad[21]->setIoMuxPins( { GPIO(21), DUMMY, GPIO(21), DUMMY, DUMMY, DUMMY} );

    // 22: GPIO22   VSPIWP   GPIO22 U0RTS      ----    EMAC_TXD1
    m_espPad[22]->setIoMuxPins( { GPIO(22), DUMMY, GPIO(22), DUMMY, DUMMY, DUMMY} );

    // 23: GPIO23   VSPID    GPIO23 HS1_STROBE ----    ----
    m_espPad[23]->setIoMuxPins( { GPIO(23), VSPID, GPIO(23), DUMMY, DUMMY, DUMMY} );

    // 24:
    // 25: GPIO25 ----      GPIO25 ----       ----    EMAC_RXD0
    m_espPad[25]->setIoMuxPins( { GPIO(25), DUMMY, GPIO(25), DUMMY, DUMMY, DUMMY} );

    // 26: GPIO26 ----      GPIO26 ----       ----    EMAC_RXD1
    m_espPad[26]->setIoMuxPins( { GPIO(26), DUMMY, GPIO(26), DUMMY, DUMMY, DUMMY} );

    // 27: GPIO27 ----      GPIO27 ----       ----    EMAC_RX_DV
    m_espPad[27]->setIoMuxPins( { GPIO(27), DUMMY, GPIO(27), DUMMY, DUMMY, DUMMY} );
    // 28:
    // 29:
    // 30:
    // 31:
    // 32: GPIOx ---- GPIOx ---- ---- ----
    m_espPad[32]->setIoMuxPins( { GPIO(32), DUMMY, GPIO(32), DUMMY, DUMMY, DUMMY} );
    // 33: GPIOx ---- GPIOx ---- ---- ----
    m_espPad[33]->setIoMuxPins( { GPIO(33), DUMMY, GPIO(33), DUMMY, DUMMY, DUMMY} );
    // 34: GPIOx ---- GPIOx ---- ---- ----
    m_espPad[34]->setIoMuxPins( { GPIO(34), DUMMY, GPIO(34), DUMMY, DUMMY, DUMMY} );
    // 35: GPIOx ---- GPIOx ---- ---- ----
    m_espPad[35]->setIoMuxPins( { GPIO(35), DUMMY, GPIO(35), DUMMY, DUMMY, DUMMY} );
    // 36: GPIOx ---- GPIOx ---- ---- ----
    m_espPad[36]->setIoMuxPins( { GPIO(36), DUMMY, GPIO(36), DUMMY, DUMMY, DUMMY} );
    // 37: GPIOx ---- GPIOx ---- ---- ----
    m_espPad[37]->setIoMuxPins( { GPIO(37), DUMMY, GPIO(37), DUMMY, DUMMY, DUMMY} );
    // 38: GPIOx ---- GPIOx ---- ---- ----
    m_espPad[38]->setIoMuxPins( { GPIO(38), DUMMY, GPIO(38), DUMMY, DUMMY, DUMMY} );
    // 39: GPIOx ---- GPIOx ---- ---- ----
    m_espPad[39]->setIoMuxPins( { GPIO(39), DUMMY, GPIO(39), DUMMY, DUMMY, DUMMY} );
}
