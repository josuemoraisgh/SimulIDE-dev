/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "e-element.h"
#include "component.h"

class LibraryItem;
class QToolButton;
class QGraphicsProxyWidget;
class IoPin;

class Dht22 : public Component , public eElement
{
    public:
        Dht22( QString type, QString id );
        ~Dht22();

        static Component* construct( QString type, QString id );
        static LibraryItem* libraryItem();

        void setModel( QString model);
        QString model();

        double temperature() { return m_temp; }
        void setTemperature( double t );

        double tempInc() { return m_tempInc; }
        void setTempInc( double inc ) { m_tempInc = trim( inc ); }

        double humidity() { return m_humi; }
        void setHumidity( double h );

        double humidInc() { return m_humiInc; }
        void setHumidInc( double inc ) { m_humiInc = trim( inc ); }

        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void runEvent() override;

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    public slots:
        void tempUpClicked();
        void tempDoClicked();
        void humidUpClicked();
        void humidDoClicked();

    private:
        void calcData();
        double trim( double data );

        bool m_DHT22;
        bool m_lastIn;

        uint64_t m_lastTime;
        uint64_t m_start;
        uint64_t m_data;
        uint64_t m_bit;

        double m_temp;
        double m_humi;
        double m_tempInc;
        double m_humiInc;

        int m_outStep;
        int m_bitStep;

        QFont m_font;

        IoPin* m_inpin;
};
