/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "component.h"
#include "e-element.h"

class IoPin;
class LibraryItem;

class Probe : public Component, public eElement
{
    public:
        Probe( QString type, QString id );
        ~Probe();

 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

        void stamp() override;
        void updateStep() override;
        void voltChanged() override;

        void setVolt( double volt );

        void setSmall( bool s );
        bool isSmall() { return m_small; }

        double threshold() { return m_voltTrig; }
        void setThreshold( double t ) { m_voltTrig = t; }

        bool pauseState() { return m_pauseState; }
        void setPauseState( bool s ) { m_pauseState = s; }

        void rotateCW() override;
        void rotateCCW() override;
        void setAngle( double angle ) override;

        void rotateAngle( double a ) override;

        QPainterPath shape() const override;
        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    protected:
        void contextMenu( QGraphicsSceneContextMenuEvent* e, QMenu* m ) override;
        void slotBreakpoint();

    private:
        void updtPinSize();

        double m_voltIn;
        double m_voltTrig;

        bool m_small;
        bool m_pauseState;
        bool m_state;

        IoPin* m_inputPin;
};
