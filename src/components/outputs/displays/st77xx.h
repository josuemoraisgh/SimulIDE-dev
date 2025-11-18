/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "tftcontroller.h"
#include "iopin.h"

class LibraryItem;

class St77xx : public TftController
{
    public:
        St77xx( QString type, QString id );
        ~St77xx();

        int width() { return m_width; }
        void setWidth( int w );

        int height() { return m_height; }
        void setHeight( int h );

    protected:
        /// void displayReset() override;
        virtual void updateSize();
        void setPixelMode() override;
        void writeRam() override;

        uint16_t m_rows;         // 8 pixel rows (Circuit grid)
        uint16_t m_maxWidth;
        uint16_t m_maxHeight;

        uint8_t m_pixelMode;
};
