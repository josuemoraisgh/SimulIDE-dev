/***************************************************************************
 *   Copyright (C) 2019 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "st77xx.h"

class LibraryItem;

class Ili9341 : public St77xx
{
    public:
        Ili9341( QString type, QString id );
        ~Ili9341();
        
 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

    protected:
        void setPixelMode() override;
        void writeRam() override;
};
