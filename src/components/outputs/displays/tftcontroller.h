/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <stdint.h>
#include <vector>

#include <QImage>

#include "component.h"

class TftController : public Component
{
    public:
        TftController( QString type, QString id );
        ~TftController();

        void updateStep() override;

        void setDisplaySize( int x, int y );

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    protected:
        virtual void displayReset();
        virtual void setPixelMode(){;}
        virtual void writeRam();

        void commandReceived();
        void dataReceived();
        void clearDDRAM();

        void printImage();

        void setRamSize( int x, int y );
        void setStartX( uint16_t sx );
        void setStartY( uint16_t sy );
        void setEndX( uint16_t ex );
        void setEndY( uint16_t ey );

        uint32_t getPixel( int row , int col );

        uint8_t m_isILI; // ILI9341 weirdness

        uint16_t m_width;
        uint16_t m_height;

        double m_scaledWidth;
        double m_scaledHeight;

        uint8_t m_rxReg;      // Received value
        uint8_t m_lastCommand;

        uint8_t m_dataBytes;  // Determined by Pixel mode
        uint8_t m_dataIndex;

        uint8_t m_dispOn;
        //bool m_dispFull;
        uint8_t m_dispInv;

        //bool m_reset;
        uint8_t m_scroll;
        uint8_t m_scrollR;
        uint8_t m_scrollV;

        uint16_t m_TFA;      // Top Fixed Area
        uint16_t m_VSA;      // Vertical Scrolling Area
        uint16_t m_BFA;      // Bottom Fixed Area
        uint16_t m_VSP;      // Vertical Scrolling Pointer

        uint32_t m_addrX;    // X RAM address
        uint32_t m_addrY;    // Y RAM address
        uint16_t m_startX;
        uint16_t m_endX;
        uint16_t m_startY;
        uint16_t m_endY;
        uint16_t m_maxX;
        uint16_t m_maxY;

        uint8_t m_BGR;
        uint8_t m_mirrorX;
        uint8_t m_mirrorY;
        uint8_t m_swapXY;
        uint8_t m_icEor;
        //uint8_t m_mirrorLine;

        uint32_t m_readBytes;
        uint32_t m_data;
        uint32_t m_colorData;

        uint8_t m_addrBytes;
        std::vector<std::vector<uint32_t>> m_DDRAM;

        QImage m_image;
};
