/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef JFET_H
#define JFET_H

#include "e-jfet.h"
#include "component.h"

class LibraryItem;

class Jfet : public Component, public eJfet
{
    public:
        Jfet( QString type, QString id );
        ~Jfet();

 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

        virtual void updateStep() override;

        // virtual void setIdss( double Idss ) override;
        // virtual void setVp( double Vp ) override;

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;
};

#endif
