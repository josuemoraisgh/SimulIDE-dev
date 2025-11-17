/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "spimodule.h"
#include "iopin.h"

class Spi5Pins : public SpiModule
{
    public:
        Spi5Pins( QString id, Component* comp );
        ~Spi5Pins();

        void stamp() override;

        virtual void endTransaction() override;

    protected:
        void reset();

        uint8_t m_isData;
        uint8_t m_buffer;

        IoPin m_pinDC;
        IoPin m_pinRS;
        IoPin m_pinCS;
        IoPin m_pinDI;
        IoPin m_pinCK;
        IoPin m_pinDO;
};
