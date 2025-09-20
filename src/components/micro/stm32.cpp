/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>
#include <QPainter>
#include <QFileInfo>

#include "stm32.h"
#include "itemlibrary.h"
#include "iopin.h"

#define tr(str) simulideTr("Stm32",str)

enum actions{
    GPIO_OUT = 1,
    GPIO_DIR,
    GPIO_IN,
};


#define PORTA 0x1000
#define PORTB 0x2000
#define PORTC 0x3000
#define PORTD 0x4000

/*static const short int pinmap[65] = {
        64,      // number of pins
        -1,      // 1 VBAT
    PORTC | 13,  // 2 C13
    PORTC | 14,  // 3 C14
    PORTC | 15,  // 4 C15
    PORTD | 0,   // 5 D0
    PORTD | 1,   // 6 D1
        -1,      // 7 RST
    PORTC | 0,   // 8 C0
    PORTC | 1,   // 9 C1
    PORTC | 2,   // 10 C2
    PORTC | 3,   // 11 C3
        -1,      // 12 VSSA
        -1,      // 13 VDDA
    PORTA | 0,   // 14 A0
    PORTA | 1,   // 15 A1
    PORTA | 2,   // 16 A2
    PORTA | 3,   // 17 A3
        -1,      // 18 VSS
        -1,      // 19 VDD
    PORTA | 4,   // 20 A4
    PORTA | 5,   // 21 A5
    PORTA | 6,   // 22 A6
    PORTA | 7,   // 23 A7
    PORTC | 4,   // 24 C4
    PORTC | 5,   // 25 C5
    PORTB | 0,   // 26 B0
    PORTB | 1,   // 27 B1
    PORTB | 2,   // 28 B2
    PORTB | 10,  // 29 B10
    PORTB | 11,  // 30 B11
        -1,      // 31 VSS
        -1,      // 32 VDD
    PORTB | 12,  // 33 B12
    PORTB | 13,  // 34 B13
    PORTB | 14,  // 35 B14
    PORTB | 15,  // 36 B15
    PORTC | 6,   // 37 C6
    PORTC | 7,   // 38 C7
    PORTC | 8,   // 39 C8
    PORTC | 9,   // 40 C9
    PORTA | 8,   // 41 A8
    PORTA | 9,   // 42 A9
    PORTA | 10,  // 43 A10
    PORTA | 11,  // 44 A11
    PORTA | 12,  // 45 A12
    PORTA | 13,  // 46 A13
        -1,      // 47 VSS
        -1,      // 48 VDD
    PORTA | 14,  // 49 A14
    PORTA | 15,  // 50 A15
    PORTC | 10,  // 51 C10
    PORTC | 11,  // 52 C11
    PORTC | 12,  // 53 C12
    PORTD | 2,   // 54 D2
    PORTB | 3,   // 55 B3
    PORTB | 4,   // 56 B4
    PORTB | 5,   // 57 B5
    PORTB | 6,   // 58 B6
    PORTB | 7,   // 59 B7
        -1,      // 60 BOOT0
    PORTB | 8,   // 61 B8
    PORTB | 9,   // 62 B9
        -1,      // 63 VSS
        -1       // 64 VDD
};*/

Component* Stm32::construct( QString type, QString id )
{ return new Stm32( type, id ); }

LibraryItem* Stm32::libraryItem()
{
    return new LibraryItem(
        "Stm32",
        "Micro",
        "ic2.png",
        "Stm32",
        Stm32::construct );
}

Stm32::Stm32( QString type, QString id )
     : QemuDevice( type, id )
{
    m_area = QRect(-32,-32, 64, 64 );

    m_ClkPeriod = 10240000; //6400000; // 6.4 ms

    m_executable = "./data/stm32/qemu-system-arm";

    m_firmware ="";

    createPins();


    /*m_argc = 0;

    strcpy( m_argv[m_argc++], "qemu-stm32" );

    strcpy(m_argv[m_argc++], "-M");
    strcpy(m_argv[m_argc++], "stm32-p103-picsimlab-new");

    strcpy( m_argv[m_argc++], "-drive");
    sprintf( m_argv[m_argc++], "file=%s,if=pflash,format=raw", "/home/user/.picsimlab/mdump_STM32_H103_stm32f103rbt6.bin" );

    // strcpy(m_argv[m_argc++], "-d");
    // strcpy(m_argv[m_argc++], "unimp");

    strcpy( m_argv[m_argc++], "-rtc");
    strcpy( m_argv[m_argc++], "clock=vm");


    //if( icount >= 0 && icount < 11 )
    //{
    int icount = 10;                      // instruction per ns ??
    strcpy(m_argv[m_argc++], "-icount");
    sprintf(m_argv[m_argc++], "shift=%i,align=off,sleep=on", icount );
    //}
    //strcpy(m_argv[m_argc++], "-icount");
    //sprintf(m_argv[m_argc++], "shift=auto,align=off,sleep=off");*/
}
Stm32::~Stm32(){}

void Stm32::createPins()
{
    m_gpioSize = 64;
    m_ioPin.resize( m_gpioSize, nullptr ); // =NULL

    setPackageFile("./data/stm32/stm32.package");

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
}

bool Stm32::createArgs()
{
    QFileInfo fi = QFileInfo( m_firmware );

    if( fi.size() != 1048576 )
    {
        qDebug() << "Error firmware file size:" << fi.size() << "must be 1048576";
        return false;
    }

    m_arguments.clear();

    m_arguments << m_shMemKey;          // Shared Memory key

    m_arguments << "qemu-system-arm";

    m_arguments << "-M";
    m_arguments << "stm32l4x5xg-soc";

    m_arguments << "-drive";
    m_arguments << "file="+m_firmware+",if=flash,format=raw";

    return true;
}

void Stm32::doAction()
{
    switch( m_arena->action )
    {
        case GPIO_OUT:       // Set Output
        {
            qDebug() << "Stm32::doAction GPIO_OUT"<< m_arena->data32;
        } break;
        case GPIO_DIR:       // Set Direction
        {
        } break;
        case GPIO_IN:                  // Read Inputs
        {
        } break;
    }
}

void Stm32::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );

    p->drawRoundedRect( m_area, 2, 2 );
    Component::paintSelected( p );
}
