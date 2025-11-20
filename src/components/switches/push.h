/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "push_base.h"

class LibraryItem;

class Push : public PushBase
{
    public:
        Push( QString type, QString id );
        ~Push();

 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

        void paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget ) override;
};
