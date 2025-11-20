/***************************************************************************
 *   Copyright (C) 2019 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "tftcontroller.h"
#include "e-clocked_device.h"
#include "iopin.h"

class LibraryItem;

class Ili9341 : public TftController, public eClockedDevice
{
    public:
        Ili9341( QString type, QString id );
        ~Ili9341();
        
 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

        void stamp() override;
        void initialize() override;
        void voltChanged() override;
        void updateStep() override;

    protected:
        void displayReset() override;
        void setPixelMode() override;
        void writeRam() override;

        int m_inBit;        //How many bits have we read since last byte

        IoPin m_pinCS;
        IoPin m_pinRst;
        IoPin m_pinDC;
        IoPin m_pinMosi;
        IoPin m_pinSck;
};
