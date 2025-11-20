/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <qtconcurrentrun.h>
#include <QLibrary>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSignalMapper>
#include <QDir>

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#ifdef __linux__
#include <sys/mman.h>
#include <sys/shm.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

#include "qemudevice.h"
#include "itemlibrary.h"
#include "qemuusart.h"
#include "circuitwidget.h"
#include "iopin.h"

#include "stm32.h"
#include "esp32.h"

#include "circuit.h"
#include "simulator.h"
#include "componentlist.h"
#include "utils.h"

#include "stringprop.h"

#define tr(str) simulideTr("QemuDevice",str)

QemuDevice* QemuDevice::m_pSelf = nullptr;

Component* QemuDevice::construct( QString type, QString id )
{
    if( QemuDevice::self() )
    {
        qDebug() << "\nQemuDevice::construct ERROR: only one QemuDevice allowed\n";
        return nullptr;
    }
    QString device = Chip::getDevice( id );

    QemuDevice* qdev = nullptr;

    if     ( device.startsWith("STM32") ) qdev = new Stm32( type, id, device );
    else if( device.startsWith("Esp32") ) qdev = new Esp32( type, id, device );
    return qdev;
}

LibraryItem* QemuDevice::libraryItem()
{
    return new LibraryItem(
        "QemuDevice",
        "",
        "ic2.png",
        "QemuDevice",
        QemuDevice::construct );
}

QemuDevice::QemuDevice( QString type, QString id )
          : Chip( type, id )
{
    m_pSelf = this;
    m_rstPin = nullptr;

    uint64_t pid = QCoreApplication::applicationPid();
    m_shMemKey = QString::number( pid )+id;
    void* arena = nullptr;

    const int shMemSize = sizeof( qemuArena_t );

    // create the shared memory object
#ifdef __linux__
    const char* charMemKey = m_shMemKey.toLocal8Bit().data();
    m_shMemId = shm_open( charMemKey, O_CREAT | O_RDWR, 0666);
    if( m_shMemId != -1 )
    {
        ftruncate( m_shMemId, shMemSize );
        arena = mmap( 0, shMemSize, PROT_WRITE, MAP_SHARED, m_shMemId, 0 );
    }
#elif defined(_WIN32)
    const wchar_t* charMemKey = m_shMemKey.toStdWString().c_str();
    // Create a file mapping object
    HANDLE hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE, // Use paging file
        NULL,                 // Default security
        PAGE_READWRITE,       // Read/write access
        0,                    // Maximum object size (high-order DWORD)
        shMemSize,            // Maximum object size (low-order DWORD)
        charMemKey );         // Name of the mapping object

    if( hMapFile != NULL )
        arena = MapViewOfFile( hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, shMemSize );

    m_wHandle = hMapFile;
#endif
    if( arena )
    {
        m_arena = (qemuArena_t*)arena;
        qDebug() << "QemuDevice::QemuDevice Shared Mem created" << shMemSize << "bytes";
    }else{
        m_arena = nullptr;
        m_shMemId = -1;
        qDebug() << "QemuDevice::QemuDevice Error creating arena";
    }

    m_qemuProcess.setProcessChannelMode( /*QProcess::MergedChannels*/ QProcess::ForwardedChannels ); // Merge stdout and stderr

    //Simulator::self()->addToUpdateList( this );

    addPropGroup( { tr("Main"),{
        new StrProp<QemuDevice>("Program", tr("Firmware"),""
                               , this, &QemuDevice::firmware, &QemuDevice::setFirmware ),

        new StrProp<QemuDevice>("Args", tr("Extra arguments"),""
                               , this, &QemuDevice::extraArgs, &QemuDevice::setExtraArgs )
    }, 0 } );
}
QemuDevice::~QemuDevice()
{
    initialize();
#ifdef __linux__
    if( m_shMemId != -1 ) shm_unlink( m_shMemKey.toLocal8Bit().data() );
#elif defined(_WIN32)
    if( m_wHandle != NULL )
    {
        UnmapViewOfFile( (LPVOID)m_arena );
        CloseHandle( (HANDLE)m_wHandle );
    }
#endif
    m_pSelf = nullptr;
}

void QemuDevice::initialize()
{
    if( m_shMemId == -1 ) return;

    m_arena->running = 0;

    m_qemuProcess.waitForFinished( 500 );
    if( m_qemuProcess.state() != QProcess::NotRunning )
    {
        m_qemuProcess.kill();
        qDebug() << "QemuDevice: Qemu proccess killed";
    }
    else qDebug() << "QemuDevice: Qemu proccess finished";
    //updateStep();
}

void QemuDevice::stamp()
{
    if( m_shMemId == -1 ) return;

    m_arena->simuTime = 0;
    m_arena->qemuTime = 0;
    m_arena->data32 = 0;
    m_arena->mask32 = 0;
    m_arena->data16 = 0;
    m_arena->mask16 = 0;
    m_arena->data8  = 0;
    m_arena->mask8  = 0;
    m_arena->simuAction = 0;
    m_arena->qemuAction = 0;
    m_arena->ps_per_inst = 0;
    m_arena->running = 0;

    for( IoPin* pin : m_ioPin ) // Qemu calls us to read input
    {
        if( !pin ) continue;
        pin->setOutState( false );
        pin->setPinMode( input );
    }
    if( m_rstPin ) m_rstPin->changeCallBack( this );

    if( createArgs() )
    {
        QString executable = m_executable;
#ifdef _WIN32
        executable += ".exe";
#endif
        if( !QFileInfo::exists( executable ) )
        {
            qDebug() << "Error: QemuDevice::stamp executable does not exist:" << endl << executable;
        }
        m_qemuProcess.start( executable, m_arguments );

        uint64_t timeout = 0;
        while( !m_arena->running )   // Wait for Qemu running
        {
            if( timeout++ > 5e9 ) // Don't wait forever
            {
                qDebug() << "Error: QemuDevice::stamp timeout";
                m_qemuProcess.kill();
                return;
            }
        }
        qDebug() << "QemuDevice::stamp started";
    }
}

//void QemuDevice::updateStep()
//{
//    return;
//
//    QString output = m_qemuProcess.readAllStandardOutput();
//    if( !output.isEmpty() )
//    {
//        QStringList lines = output.split("\n");
//        for( QString line : lines ) qDebug() << line.remove("\"");
//    }
//}

void QemuDevice::voltChanged()
{
    if( m_shMemId == -1 ) return;

    bool reset = !m_rstPin->getInpState();

    if( reset )
    {
        m_arena->running = 0;

        if( m_qemuProcess.state() > QProcess::NotRunning )
        {
            Simulator::self()->cancelEvents( this );
            m_qemuProcess.kill();
        }
    }
    else if( m_qemuProcess.state() == QProcess::NotRunning ) stamp();
}

void QemuDevice::runToTime( uint64_t time )
{
    if( this->eventTime ) return;// Our event still not executed
    //if( m_arena->qemuTime ) return; // Our event still not executed
    //if( m_arena->simuTime ) return; // Our event still not executed

    //qDebug() << "\nQemuDevice::runToTime"<< time;

    m_arena->simuTime = 0;
    m_arena->qemuTime = time; // Tell Qemu to run up to time
    //m_arena->qemuAction = SIM_EVENT;

    while( m_arena->simuTime == 0 ) { ; } // Wait for Qemu action  //   Simulator::self()->simState() == SIM_RUNNING )

    uint64_t eventTime = m_arena->simuTime - Simulator::self()->circTime();
    //if( m_arena->action < SIM_EVENT )
    Simulator::self()->addEvent( eventTime, this );
    //qDebug() << "QemuDevice::runToTime event"<< m_arena->simuTime;
}

void QemuDevice::runEvent()
{
    //qDebug() << "QemuDevice::runEvent"<< m_arena->simuAction<< Simulator::self()->circTime();
    if( m_arena->simuAction < SIM_EVENT ) doAction();
    //m_arena->simuAction = 0;
    //m_arena->qemuTime = 0;       // Qemu will wait for next time
    //m_arena->simuTime = 0;
}

void QemuDevice::slotOpenTerm( int num )
{
    m_usarts.at(num-1)->openMonitor( findIdLabel(), num );
    //m_serialMon = num;
}

void QemuDevice::slotLoad()
{
    QDir dir( m_lastFirmDir );
    if( !dir.exists() ) m_lastFirmDir = Circuit::self()->getFilePath();

    QString fileName = QFileDialog::getOpenFileName( nullptr, tr("Load Firmware"), m_lastFirmDir,
                                                    tr("All files (*.*);;Hex Files (*.hex)"));

    if( fileName.isEmpty() ) return; // User cancels loading

    setFirmware( fileName );
}

void QemuDevice::slotReload()
{
    if( !m_firmware.isEmpty() ) setFirmware( m_firmware );
    else QMessageBox::warning( 0, "QemuDevice::slotReload", tr("No File to reload ") );
}

void QemuDevice::setFirmware( QString file )
{
    if( Simulator::self()->isRunning() ) CircuitWidget::self()->powerCircOff();

    //QDir    circuitDir   = QFileInfo( Circuit::self()->getFilePath() ).absoluteDir();
    //QString fileNameAbs  = circuitDir.absoluteFilePath( file );
    //QString cleanPathAbs = circuitDir.cleanPath( fileNameAbs );
    m_firmware = file;//cleanPathAbs;
}

void QemuDevice::setPackageFile( QString package )
{
    if( !QFile::exists( package ) )
    {
        qDebug() <<"File does not exist:"<< package;
        return;
    }
    QString pkgText = fileToString( package, "QemuDevice::setPackageFile");
    QString pkgStr  = convertPackage( pkgText );
    m_isLS = package.endsWith("_LS.package");
    initPackage( pkgStr );
    setLogicSymbol( m_isLS );

    m_label.setPlainText( m_name );

    Circuit::self()->update();
}

void QemuDevice::contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu )
{
    //if( m_eMcu.flashSize() )
    {
        QAction* loadAction = menu->addAction( QIcon(":/load.svg"),tr("Load firmware") );
        QObject::connect( loadAction, &QAction::triggered, [=](){ slotLoad(); } );

        QAction* reloadAction = menu->addAction( QIcon(":/reload.svg"),tr("Reload firmware") );
        QObject::connect( reloadAction, &QAction::triggered, [=](){ slotReload(); } );

        menu->addSeparator();
    }

    //QAction* openRamTab = menu->addAction( QIcon(":/terminal.svg"),tr("Open Mcu Monitor.") );
    //QObject::connect( openRamTab, &QAction::triggered, [=](){ slotOpenMcuMonitor(); } );

    if( m_usarts.size() )
    {
        QMenu* serMonMenu = menu->addMenu( QIcon(":/serialterm.png"),tr("Open Serial Monitor.") );

        QSignalMapper* sm = new QSignalMapper();
        for( uint i=0; i<m_usarts.size(); ++i )
        {
            QAction* openSerMonAct = serMonMenu->addAction( "USart"+QString::number(i+1) );
            QObject::connect( openSerMonAct, &QAction::triggered, sm, QOverload<>::of(&QSignalMapper::map) );
            sm->setMapping( openSerMonAct, i+1 );
        }
        QObject::connect( sm, QOverload<int>::of(&QSignalMapper::mapped), [=](int n){ slotOpenTerm(n);} );
    }
    menu->addSeparator();
    Component::contextMenu( event, menu );
}
