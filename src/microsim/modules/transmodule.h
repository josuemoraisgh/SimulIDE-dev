/***************************************************************************
 *   Copyright (C) 2026 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QString>

class SerialMonitor;

class TransModule
{
    public:
        TransModule( QString name );
        ~TransModule();

        QString moduleName() { return m_moduleName; }

        void printOut( uint8_t data );
        void printIn( uint8_t data );

        bool monitorOpen() { return m_monOpen; }

        void closeMonitor();
        void openMonitor( QString id, int num=0, bool send=false );
        void setMonitorTittle( QString t );
        virtual void monitorClosed();

    protected:

        QString m_moduleName;

        bool m_monOpen;

        SerialMonitor* m_monitor;
};
