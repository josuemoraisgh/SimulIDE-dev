/***************************************************************************
 *   Copyright (C) 2019 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "component.h"
#include "e-clocked_device.h"
#include "tftcontroller.h"
#include "iopin.h"

class LibraryItem;

class Ili9341 : public Component, public eClockedDevice, public TftController
{
    public:
        Ili9341( QString type, QString id );
        ~Ili9341();
        
        static Component* construct( QString type, QString id );
        static LibraryItem* libraryItem();

        virtual void stamp() override;
        virtual void initialize() override;
        virtual void voltChanged() override;
        virtual void updateStep() override;
        
        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    protected:
        void displayReset() override;
        void setPixelMode() override;
        void writeRam() override;

        int m_inBit;        //How many bits have we read since last byte
        int m_inByte;

        //Inputs
        IoPin m_pinCS;
        IoPin m_pinRst;
        IoPin m_pinDC;
        IoPin m_pinMosi;
        IoPin m_pinSck;
        //Pin m_pinMiso;

        QImage m_img;
};
