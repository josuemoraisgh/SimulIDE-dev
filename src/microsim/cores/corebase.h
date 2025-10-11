/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QString>

class Display;

class CoreBase
{
    public:
        CoreBase();
        virtual ~CoreBase();

        virtual void reset(){;}
        virtual void runStep(){;}
        virtual void extClock( bool clkState ){;}
        virtual void updateStep(){;}

        virtual void command( QString c ){;}

        virtual int getCpuReg( QString reg ) { return -1; }
        virtual QString getStrReg( QString ){ return ""; }

    protected:

        Display* m_display; // Find a place for this
};
