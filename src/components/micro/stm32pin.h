/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "iopin.h"

class IoPort;

class Stm32Pin : public IoPin
{
    //friend class Stm32;

    public:
        Stm32Pin( /*IoPort* port,*/ int i, QString id, Component* mcu );
        ~Stm32Pin();

        void initialize() override;
        void stamp() override;

        void setOutState( bool high ) override;
        void scheduleState( bool high, uint64_t time ) override;

        void setPortState( bool high );

        void setPull( bool p );
        bool setAlternate( bool a );
        void setAnalog( bool a );

    protected:
        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

        void setPinState( bool high );
        //QString m_id;

        //IoPort*   m_port;

        bool m_pull;
        bool m_analog;
        bool m_alternate;

        double m_pullAdmit;

        uint8_t m_pinMask;
};
