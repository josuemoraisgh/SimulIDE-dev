/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "qemudevice.h"
#include "ioport.h"

class LibraryItem;

class Stm32 : public QemuDevice
{
    public:
        Stm32( QString type, QString id );
        ~Stm32();

        void stamp() override;

 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

    protected:
        Pin* addPin( QString id, QString type, QString label,
                    int n, int x, int y, int angle , int length=8, int space=0 ) override;

        void createPins();
        bool createArgs() override;
        void doAction() override;
        //void readInputs();

        uint64_t m_frequency;

        IoPort m_portA;
        IoPort m_portB;
        IoPort m_portC;
        IoPort m_portD;
};
