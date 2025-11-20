/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "mech_contact.h"
#include "inductor.h"

class LibraryItem;
class eNode;

class Relay : public MechContact
{
    public:
        Relay( QString type, QString id );
        ~Relay();

 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

        double iTrig() { return m_trigCurrent; }
        void setITrig( double c ) { if( c > 0.0 ) m_trigCurrent = c; }

        double iRel() { return m_relCurrent; }
        void setIRel( double current ) { m_relCurrent = current; }

        void initialize() override;
        void stamp() override;
        void voltChanged() override;

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    protected:
        Inductor* m_inductor;

        eNode* m_internalEnode;

        double m_trigCurrent;
        double m_relCurrent;

        bool m_relayOn;
};
