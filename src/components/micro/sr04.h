/***************************************************************************
 *   Copyright (C) 2019 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "e-element.h"
#include "component.h"

class LibraryItem;
class IoPin;
class CustomSlider;

class SR04 : public Component, public eElement
{
    public:
        SR04( QString type, QString id );
        ~SR04();
        
        void stamp() override;
        void initialize() override;
        void updateStep() override;
        void voltChanged() override;
        void runEvent() override;

        bool slider() { return m_useDial; }
        void setSlider( bool s );

        void dialChanged( int );

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

    private:
        bool m_useDial;

        uint64_t m_lastStep;
        bool     m_lastTrig;

        int m_echouS;
        double m_distance;
        
        Pin* m_inpin;
        Pin* m_trigpin;
        
        IoPin* m_echo;

        CustomSlider* m_slider;
};
