/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "component.h"
#include "e-element.h"

class LibraryItem;
class QPushButton;
class QGraphicsProxyWidget;

class SwitchDip : public Component, public eElement
{
    public:
        SwitchDip( QString type, QString id );
        ~SwitchDip();

 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

        int  size() { return m_size; }
        void setSize( int size );

        bool exclusive() { return m_exclusive; }
        void setExclusive( bool e );

        bool commonPin() { return m_commonPin; }
        void setCommonPin( bool c );

        int  state() { return m_state; }
        void setState( int state );

        void stamp() override;
        void updateStep() override;
        void remove() override;

        void createSwitches( int c );
        void deleteSwitches( int d );

        void setHidden( bool hid, bool hidArea=false, bool hidLabel=false ) override;

        void setLinkedValue( double v, int i=0 ) override;

        void onbuttonclicked();

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    private:
        QList<QPushButton*> m_buttons;
        QList<QGraphicsProxyWidget*> m_proxys;

        bool m_exclusive;
        bool m_commonPin;

        int m_size;
        int m_state;
};
