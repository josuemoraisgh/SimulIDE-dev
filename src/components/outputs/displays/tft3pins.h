/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "e-clocked_device.h"
#include "iopin.h"

class Tft3Pins : public eClockedDevice
{
    public:
        Tft3Pins( QString id, Component* comp );
        ~Tft3Pins();

        void stamp() override;
        void voltChanged() override;

        virtual void endTransaction() {;}

    protected:
        void reset();

        uint8_t m_inBit;        //How many bits have we read since last byte
        uint8_t m_isData;
        uint8_t m_buffer;

        IoPin m_pinRS;
        IoPin m_pinCS;
        IoPin m_pinDI;
        IoPin m_pinCK;
};
