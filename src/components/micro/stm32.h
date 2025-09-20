/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "qemudevice.h"

class LibraryItem;

class Stm32 : public QemuDevice
{
    public:
        Stm32( QString type, QString id );
        ~Stm32();

 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w) override;

    protected:
        void createPins();
        bool createArgs() override;
        void doAction() override;
        //void readInputs();

};
