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

        int width() { return m_width; }
        void setWidth( int w );

        int height() { return m_height; }
        virtual void setHeight( int h );

        double scale() { return m_scale; }
        void setScale( double s );

    protected:
        /// void displayReset() override;
        virtual void updateSize();
        void setPixelMode() override;
        void writeRam() override;

        //uint16_t m_rows;         // 8 pixel rows (Circuit grid)
        uint16_t m_maxWidth;
        uint16_t m_maxHeight;

        uint8_t m_pixelMode;
};
