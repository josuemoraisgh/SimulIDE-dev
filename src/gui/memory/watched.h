/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QString>

class Display;
class Watcher;

class Watched
{
    public:
        Watched();
        virtual ~Watched();

        virtual void command( QString c ){;}

        virtual double  getDblReg( QString reg ) { return 0; }
        virtual int     getIntReg( QString reg ) { return -1; }
        virtual QString getStrReg( QString reg ) { return ""; }

        Watcher* getWatcher() { return m_watcher; }
        void createWatcher( bool proxy=false );

    protected:

        Display* m_display; // Find a place for this

        Watcher* m_watcher;
};
