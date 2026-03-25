/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "component.h"
#include "e-element.h"
#include "dialwidget.h"

class Dialed : public Component
{
    public:
        Dialed( QString type, QString id );
        ~Dialed();

        QList<ComProperty*> dialProps();

        bool slider() { return m_slider; }
        void setSlider( bool s );

        double scale() { return m_dialW.scale(); }
        void setScale( double s );

        double maxVal()  { return m_maxVal; }
        virtual void setMaxVal( double max );

        double minVal() { return m_minVal; }
        virtual void setMinVal( double min );

        double getVal() { return m_value; }
        void setVal( double val );

        double getStep() { return m_step; }
        void setStep( double step ) { if( step<0 ) step=0; m_step = step ; }

        virtual void setHidden( bool hid, bool hidArea=false, bool hidLabel=false ) override;

        virtual void setLinkedValue( double v, int i=0 ) override;

        virtual void dialChanged( int val );

    protected:
        void updtValue();
        virtual void updateProxy(){;}

        double m_value;
        double m_minVal;
        double m_maxVal;
        double m_step;

        bool m_needUpdate;
        bool m_slider;

        bool m_blocked;

        QRectF m_areaDial;
        QRectF m_areaComp;

        DialWidget m_dialW;
        QGraphicsProxyWidget* m_proxy;
};
