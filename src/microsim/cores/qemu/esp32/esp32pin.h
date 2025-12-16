/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "iopin.h"
#include "qemumodule.h"

struct funcPin
{
    QemuModule* module;
    IoPin** pinPointer;
    QString label;
};

class esp32Pin : public IoPin, public QemuModule
{
    friend class Esp32;

    public:
        esp32Pin( int i, QString id, QemuDevice* mcu, IoPin* dummyPin );
        ~esp32Pin();

        void initialize() override;
        void stamp() override;
        void updateStep() override;
        void voltChanged() override;

        void setPinMode( pinMode_t mode );

        void setOutState( bool high ) override;
        void scheduleState( bool high, uint64_t time ) override;

        void setPortState( bool high );

        //void setPull( bool p );
        //bool setAlternate( bool a );
        //void setAnalog( bool a );

        void setIoMuxFunc( uint64_t func );

        void setMatrixFunc( uint16_t val, funcPin func );

        void setIoMuxPins( QList<funcPin> iomuxPin );

    protected:
        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

        void selectIoMuxFunc( uint8_t func );

        void setPinState( bool high );
        QString m_pinLabel;

        //bool m_analog;
        //bool m_alternate;

        double m_pullAdmit;

        uint64_t m_pinMask;
        uint8_t m_pullUp;
        uint8_t m_pullDown;
        uint8_t m_inputEn;

        uint8_t m_iomuxIndex;

        IoPin* m_dummyPin;
        funcPin m_iomuxPin[6];
};
