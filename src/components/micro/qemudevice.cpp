/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <qtconcurrentrun.h>
#include <QLibrary>
#include <QFileInfo>
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
#include "circuitwidget.h"
#include "iopin.h"

#include "circuit.h"
#include "simulator.h"
#include "utils.h"
#include "stringprop.h"


#define tr(str) simulideTr("QemuDevice",str)

QemuDevice::QemuDevice( QString type, QString id )
          : Chip( type, id )
{
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
        qDebug() << "Shared Mem created" << shMemSize << "bytes";

        Simulator::self()->m_qemuDevice = this;
    }else{
        m_arena = nullptr;
        m_shMemId = -1;
        qDebug() << "Error creating arena";
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
}

void QemuDevice::initialize()
{
    if( m_shMemId == -1 ) return;

    m_qemuProcess.waitForFinished( 500 );
    if( m_qemuProcess.state() != QProcess::NotRunning )
    {
        m_qemuProcess.kill();
        qDebug() << "QemuDevice: Qemu proccess killed";
    }
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
        while( m_arena->simuAction == 0 )   // Wait for Qemu running
        {
            if( timeout++ > 5e9 ) // Don't wait forever
            {
                qDebug() << "Error: QemuDevice::stamp timeout";
                m_qemuProcess.kill();
                return;
            }
        }
        m_arena->simuAction = 0;
        qDebug() << "QemuDevice::stamp started";
        //Simulator::self()->addEvent( 10, this );
        //updateStep();
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

    //m_arena->resetHard = reset;

    if( reset )
    {
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
    if( m_arena->qemuTime ) return; // Our event still not executed

    //qDebug() << "\nQemuDevice::runToTime"<< time/1000;

    m_arena->qemuTime = time; // Tell Qemu to run up to time

    while( true ) //Simulator::self()->simState() == SIM_RUNNING )
    {
        if( m_arena->simuTime )
        {
            uint64_t actionTime = m_arena->simuTime;
            uint64_t eventTime = actionTime - Simulator::self()->circTime();
            //if( m_arena->action < SIM_EVENT )
                Simulator::self()->addEvent( eventTime, this );

            //qDebug() << "QemuDevice::runToTime action:"<< m_arena->simuAction <<"at time"<< actionTime/1000;
            return;
        }
    }
}

void QemuDevice::runEvent()
{
    //qDebug() << "QemuDevice::runEvent"<< m_arena->action<< Simulator::self()->circTime()/1000;
    if( m_arena->simuAction < SIM_EVENT ) doAction();
    m_arena->simuAction = 0;
    m_arena->simuTime = 0;
    m_arena->qemuTime = 0;       // Qemu will wait for next time
    //m_arena->qemuTime = time; // Tell Qemu to run up to time
}

/*void QemuDevice::runEvent()
{
    //qDebug() << "runEvent"<<m_arena->action<<eventEnter;
    if( m_arena->action )
    {
        doAction();
        m_arena->action = 0;
    }

    uint64_t nextTime = 0;
    uint64_t time = 0;
    m_arena->time = 0;

    while( time == 0 )   // Wait for next event from Qemu
    {
        if( Simulator::self()->simState() < SIM_RUNNING ) break;// runCircuit loop finished
        time = m_arena->time;
    }
    if( time )
    {
        uint64_t eventEnter = Simulator::self()->circTime();
        Q_ASSERT( time >= eventEnter );
        nextTime = time-eventEnter;
    }
    Simulator::self()->addEvent( nextTime, this );
    //qDebug() << "exitEvent"<<m_arena->action<<m_arena->time;
}*/

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
