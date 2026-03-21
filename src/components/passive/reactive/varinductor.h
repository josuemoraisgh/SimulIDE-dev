/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "dialed.h"
#include "e-reactive.h"

class LibraryItem;

class VarInductor : public Dialed, public eReactive
{
    public:
        VarInductor( QString type, QString id );
        ~VarInductor();

 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

        void initialize() override;
        void stamp() override;
        void updateStep() override;

        double resist() { return m_resistor->resistance(); }
        void setResist( double resist );

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    protected:
        void updateProxy() override;
        double updtRes()  override { return m_inductance/m_tStep; }
        double updtCurr() override { return m_volt*m_admit; }

    private:
        eNode* m_midEnode;
        eResistor* m_resistor;

        double m_inductance;
};
