/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "linkercomponent.h"
#include "e-resistor.h"
#include "watched.h"

class LibraryItem;

class DcMotor : public LinkerComponent, public eResistor, public Watched
{
    public:
        DcMotor( QString type, QString id );
        ~DcMotor();

        QString getPropStr( QString prop ) override;

 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

        void initialize() override;
        void stamp() override;
        void voltChanged() override;
        void updateStep() override;

        int rpm() { return m_rpm; }
        void setRpm( int rpm );

        double volt() { return m_voltNom; }
        void setVolt( double v ) { m_voltNom = v; }

        void openWatcher();

        double getDblReg( QString reg ) override;

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    protected:
        void contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu ) override;

        void updatePos();

        int m_rpm;

        double m_LastVolt;
        double m_voltNom;
        double m_ang;
        double m_motStPs;
        double m_speed;
        double m_delta;

        uint64_t m_lastTime;
        uint64_t m_updtTime;
};
