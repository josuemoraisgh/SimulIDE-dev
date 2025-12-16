/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QProcess>

#include "chip.h"

typedef struct qemuArena{
    uint64_t simuTime;    // in ps
    uint64_t qemuTime;    // in ps
    uint64_t qemuEvent;
    uint32_t data32;
    uint32_t mask32;
    uint16_t data16;
    uint16_t mask16;
    uint8_t  data8;
    uint8_t  mask8;
    uint8_t  simuAction;
    uint8_t  qemuAction;
    uint8_t  running;
    double   ps_per_inst;
} qemuArena_t;

enum simuAction{
    SIM_I2C=10,
    SIM_SPI,
    SIM_USART,
    SIM_TIMER,
    SIM_GPIO_IN,
    SIM_EVENT=1<<7
};

class IoPin;
class QemuModule;
class QemuUsart;
class QemuTimer;
class QemuTwi;
class QemuSpi;
class LibraryItem;

class QemuDevice : public Chip
{
    public:
        QemuDevice( QString type, QString id );
        ~QemuDevice();

        void initialize() override;
        void stamp() override;
        //void updateStep() override;
        void voltChanged() override;
        void runEvent() override;

        QString firmware() { return m_firmware; }
        void setFirmware( QString file );

        QString extraArgs()  { return m_extraArgs; }
        void setExtraArgs( QString a ){ m_extraArgs = a; }

        void setPackageFile( QString package );

        volatile qemuArena_t* getArena() { return m_arena; }

        void runToTime( uint64_t time );
        //void setNexTEvent( uint64_t e ) { m_nextEvent = e; }

        void slotLoad();
        void slotReload();
        void slotOpenTerm( int num );

        //void addEvent( uint64_t time, QemuModule* el );
        //void cancelEvents( QemuModule* el );
        void addModule( QemuModule* m ) { m_modules.append( m ); }

 static QemuDevice* self() { return m_pSelf; }
 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

    protected:
 static QemuDevice* m_pSelf;

        virtual bool createArgs(){ return false;}

        virtual void doAction(){;}

        void contextMenu( QGraphicsSceneContextMenuEvent* e, QMenu* m ) override;

        QString m_lastFirmDir;  // Last firmware folder used
        QString m_firmware;
        QString m_executable;
        QString m_packageFile;

        QString m_extraArgs;

        //QemuModule* m_firstEvent;

        volatile qemuArena_t* m_arena;

        //bool m_fullSynch;
        //uint64_t m_lastTime;

        int m_gpioSize;
        std::vector<IoPin*> m_ioPin;
        IoPin* m_rstPin;

        QString m_shMemKey;
        int64_t m_shMemId;

        void* m_wHandle;

        QProcess m_qemuProcess;
        QStringList m_arguments;

        uint8_t m_portN;
        uint8_t m_usartN;
        //uint8_t m_timerN;
        uint8_t m_i2cN;
        uint8_t m_spiN;

        std::vector<QemuTwi*> m_i2cs;
        std::vector<QemuSpi*> m_spis;
        std::vector<QemuUsart*> m_usarts;

        QList<QemuModule*> m_modules;
};
