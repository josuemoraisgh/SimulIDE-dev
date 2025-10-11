/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QHash>

#include "e-element.h"

class CoreBase;
class Cpu8bits;
class Mcu;
class IoPort;
class IoPin;
class Watcher;

class eIou : public eElement
{
    public:
        eIou( Mcu* comp, QString id );
        ~eIou();

        void reset();

        IoPort* getIoPort( QString name );
        IoPin*  getIoPin( QString pinName );

        Watcher* getWatcher() { return m_watcher; }
        void createWatcher( CoreBase* cpu );

        Mcu* component() { return m_component; }
        Cpu8bits* cpu()   { return m_cpu; }

    protected:
        Mcu* m_component;

        Cpu8bits* m_cpu;

        IoPin*  m_clkPin;

        Watcher* m_watcher;

        QHash<QString, IoPort*> m_ioPorts;  // Access ioPORTS by name
};
