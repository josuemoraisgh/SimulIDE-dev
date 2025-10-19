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
    uint32_t data32;
    uint32_t mask32;
    uint16_t data16;
    uint16_t mask16;
    uint8_t  data8;
    uint8_t  mask8;
    uint8_t  state;
    uint8_t  action;
} qemuArena_t;

enum simuAction{
    SIM_I2C=10,
    SIM_USART,
    SIM_EVENT=1<<7
};


class IoPin;

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

        //void clearData32() { m_arena->data32 = 0; }

        volatile qemuArena_t* getArena() { return m_arena; }

        void runToTime( uint64_t time );

    protected:
        virtual bool createArgs(){ return false;}

        virtual void doAction(){;}

        QString m_firmware;
        QString m_executable;

        QString m_extraArgs;

        volatile qemuArena_t* m_arena;

        uint64_t m_ClkPeriod;

        int m_gpioSize;
        std::vector<IoPin*> m_ioPin;
        IoPin* m_rstPin;

        QString m_shMemKey;
        int64_t m_shMemId;

        void* m_wHandle;

        QProcess m_qemuProcess;
        QStringList m_arguments;
};
