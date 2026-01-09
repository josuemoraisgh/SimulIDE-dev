/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "qemudevice.h"

class QemuModule
{
    friend class  QemuDevice;

    public:
        QemuModule(QemuDevice* mcu, QString name, int n, uint32_t* clk=nullptr, uint64_t memStart=0, uint64_t memEnd=0 );
        ~QemuModule();

        virtual void reset();

        virtual void doAction() {;}

        virtual void connected( bool c ) {;}

        virtual void runAction();

        inline void writeMem( uint32_t address, uint32_t value ) { (*m_ioMem)[address] = value; }
        inline void write() { (*m_ioMem)[m_eventAddress] = m_eventValue; }

        inline uint32_t readMem( uint32_t address ) { return (*m_ioMem)[address]; }
        inline uint32_t read() { return (*m_ioMem)[m_eventAddress]; }

        virtual void freqChanged(){;}

        //QemuModule* nextEvent;
        //uint64_t eventTime;

    protected:
        virtual void writeRegister();
        virtual void readRegister();

        void setInterrupt( uint8_t number, uint8_t level );

        QString m_name;
        int m_number;

        uint32_t* m_frequency;

        uint64_t m_memStart;
        uint64_t m_memEnd;

        uint8_t  m_eventAction;
        uint64_t m_eventAddress;
        uint64_t m_eventValue;

        std::vector<uint32_t>* m_ioMem;
        QemuDevice* m_device;

        volatile qemuArena_t* m_arena;
};

