/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <qtconcurrentrun.h>
#include <QLibrary>
#include <QFileInfo>
#include <QDir>

#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

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

    //int shMemSize = 10*8;

    //m_sharedMemOk = false;

    /*if( !m_sharedMemory.create( shMemSize ) ) // Create shared memory segment
    {
        if( m_sharedMemory.error() == QSharedMemory::AlreadyExists )
        {
            if( m_sharedMemory.attach() )
            {
                m_sharedMemOk = true;
                qDebug() << "Attached to existing sharedMemory";
            }
            else
            {
                //m_sharedMemory.detach();
                qDebug() << "Could not attach to shared memory segment:" << m_sharedMemory.errorString();
            }
        }
        else qDebug() << "Could not create shared memory segment:" << m_sharedMemory.errorString();
    }else{
        m_sharedMemOk = true;
        qDebug() << "Created new sharedMemory";
    }*/

    uint64_t pid = QCoreApplication::applicationPid();
    m_shMemKey = QString::number( pid )+id;

    // create the shared memory object
    m_shMemId = shm_open( m_shMemKey.toLocal8Bit().data(), O_CREAT | O_RDWR, 0666);

    if( m_shMemId == -1 )
    {
        qDebug() << "Error creating shared Mem";
    }
    else
    {
        qDebug() << "Shared Mem created" << sizeof( qemuArena_t ) << "bytes";

        ftruncate( m_shMemId, sizeof( qemuArena_t ) );
        void* arena = mmap( 0, sizeof( qemuArena_t ), PROT_WRITE, MAP_SHARED, m_shMemId, 0 );

        if( arena )
        {
            m_arena = (qemuArena_t*)arena;
            m_arena->state = 0;
        }
        else qDebug() << "Error creating arena";
    }

    m_qemuProcess.setProcessChannelMode( QProcess::MergedChannels ); // Merge stdout and stderr

    Simulator::self()->addToUpdateList( this );

    addPropGroup( { tr("Main"),{
        new StrProp<QemuDevice>("Program", tr("Firmware"),""
                         , this, &QemuDevice::firmware, &QemuDevice::setFirmware ),
    }, 0 } );
}
QemuDevice::~QemuDevice()
{
    initialize();

    if( m_shMemId != -1 ) shm_unlink( m_id.toLocal8Bit().data() );
}

void QemuDevice::initialize()
{
    if( m_shMemId == -1 ) return;

    m_arena->state = 0;

    m_qemuProcess.kill();

    m_qemuProcess.waitForFinished( 1000 );
    updateStep();
}

void QemuDevice::stamp()
{
    if( m_shMemId == -1 ) return;

    m_arena->time = 0;
    m_arena->data32 = 0;
    m_arena->mask32 = 0;
    m_arena->data16 = 0;
    m_arena->mask16 = 0;
    m_arena->data8  = 0;
    m_arena->mask8  = 0;
    m_arena->state  = 0;
    m_arena->action = 0;

    for( IoPin* pin : m_ioPin ) // Qemu calls us to read input
    {
        if( !pin ) continue;
        pin->setOutState( false );
        pin->setPinMode( input );
    }
    if( m_rstPin ) m_rstPin->changeCallBack( this );

    if( createArgs() )
    {
        m_qemuProcess.start( m_executable, m_arguments );

        uint64_t timeout = 0;
        while( !m_arena->state )   // Wait for Qemu running
        {
            if( timeout++ > 5e9 ) // Don't wait forever
            {
                qDebug() << "Error: QemuDevice::stamp timeout";
                m_qemuProcess.kill();
                return;
            }
        }
        Simulator::self()->addEvent( 10, this );
        updateStep();
    }
}

void QemuDevice::updateStep()
{
    QString output = m_qemuProcess.readAllStandardOutput();
    if( !output.isEmpty() )
    {
        QStringList lines = output.split("\n");
        for( QString line : lines ) qDebug() << line.remove("\"");
    }
}

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

void QemuDevice::runEvent()
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
}

void QemuDevice::setFirmware( QString file )
{
    if( Simulator::self()->isRunning() ) CircuitWidget::self()->powerCircOff();

    QDir    circuitDir   = QFileInfo( Circuit::self()->getFilePath() ).absoluteDir();
    QString fileNameAbs  = circuitDir.absoluteFilePath( file );
    QString cleanPathAbs = circuitDir.cleanPath( fileNameAbs );
    m_firmware = cleanPathAbs;
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

Pin* QemuDevice::addPin( QString id, QString type, QString label,
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
