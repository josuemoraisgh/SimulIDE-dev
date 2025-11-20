/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "st77xx.h"
#include "spi5pins.h"

class LibraryItem;

class St7789 : public St77xx, public Spi5Pins
{
    public:
        St7789( QString type, QString id );
        ~St7789();

        void endTransaction() override;

 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

    protected:
        void displayReset() override;
        void updateSize() override;

};
