/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "st77xx.h"

class LibraryItem;

class St7735 : public St77xx
{
    public:
        St7735( QString type, QString id );
        ~St7735();

        void endTransaction() override;

 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

    protected:
        void displayReset() override;
};
