/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "tftcontroller.h"
#include "spi5pins.h"
//#include "iopin.h"

class LibraryItem;

class St77xx : public TftController, public Spi5Pins
{
    public:
        St77xx( QString type, QString id );
        ~St77xx();

        void initialize() override;

        int width() { return m_width; }
        void setWidth( int w );

        int height() { return m_height; }
        virtual void setHeight( int h );

        double scale() { return m_scale; }
        void setScale( double s );

        void endTransaction() override;

    protected:
        void displayReset() override;
        virtual void updateSize();
        virtual void setPixelMode() override;
        virtual void writeRam() override;

        //uint16_t m_rows;         // 8 pixel rows (Circuit grid)

        uint8_t m_pixelMode;
};
