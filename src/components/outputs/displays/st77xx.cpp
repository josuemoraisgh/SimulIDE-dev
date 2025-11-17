/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "st77xx.h"
#include "circuit.h"

#define tr(str) simulideTr("St77xx",str)

St77xx::St77xx( QString type, QString id )
      : TftController( type, id )
{

}
St77xx::~St77xx(){}

void St77xx::setPixelMode()
{
    switch( m_rxReg & 0b111 )
    {
    case 2: m_dataBytes = 1; break;
    case 3: m_dataBytes = 2; break;
    case 5: m_dataBytes = 3; break;
    }
}

void St77xx::writeRam()
{
    /// TODO

    //m_dataIndex++;
    //if( m_dataIndex > m_dataBytes ) return;

    //uint32_t buffer = m_rxReg;
    //switch( m_dataBytes )
    //{
    //    case 1:  // RRRGGGBB
    //    {
    //        m_data = buffer & 0b11<<(0+6);
    //        buffer >>= 2;
    //        m_data |= (buffer & 0b111)<<(8+5);
    //        buffer >>= 3;
    //        m_data |= (buffer & 0b111)<<(16+5);
    //        TftController::writeRam();
    //    }break;
    //    case 2:  // RRRRRGGG GGGBBBBB
    //    {
    //        switch( m_dataIndex ) {
    //        case 1:{
    //            m_data = (buffer & 0b111)<<(8+3+2);  // GGG---
    //            buffer >>= 3;
    //            m_data |= (buffer & 0b11111)<<(16+3);// RRRRR
    //        }break;
    //        case 2:{
    //            m_data |= (buffer & 0b11111)<<(0+3); // BBBBB
    //            buffer >>= 5;
    //            m_data = (buffer & 0b111)<<(8+2);    // ---GGG
    //            TftController::writeRam();
    //        }break;
    //        }
    //    }break;
    //    case 3:  // RRRRGGGG BBBB-RRRR GGGGBBBB
    //    {
    //        switch( m_dataIndex ) {
    //            case 1:{
    //                m_colorData = (buffer & 0b1111)<<(8+4);   // GGGG
    //                buffer >>= 4;
    //                m_colorData |= (buffer & 0b1111)<<(16+4); // RRRR
    //            }break;
    //            case 2:{
    //                m_data = m_colorData;
    //                m_colorData = (buffer & 0b1111)<<(16+4);  // RRRR Next Pixel
    //                buffer >>= 4;
    //                m_data |= (buffer & 0b1111)<<(0+4);   // BBBB This Pixel
    //                TftController::writeRam();            // First Pixel
    //            }break;
    //            case 3:{
    //                m_data = m_colorData;
    //                m_data |= (buffer & 0b1111)<<(0+4);   // BBBB
    //                buffer >>= 4;
    //                m_data |= (buffer & 0b1111)<<(8+4);   // GGGG
    //                TftController::writeRam();            // Second Pixel
    //            }break;
    //        }
    //    }break;
    //}
}

void St77xx::setWidth( int w )
{
    if     ( w > m_maxWidth ) w = m_maxWidth;
    else if( w <  32        ) w = 32;
    if( m_width == w ) return;

    setDisplaySize( w, m_height );
    updateSize();
}

void St77xx::setHeight( int h )
{
    if( h > m_height ) h += 8;
    if     ( h > m_maxHeight ) h = m_maxHeight;
    else if( h <  16         ) h = 16;

    h = (h/8)*8;
    if( m_height == h ) return;

    m_rows = h/8;
    setDisplaySize( m_width, h );
    updateSize();
}

void St77xx::updateSize()
{
    m_area = QRectF(-m_width/2-6,-m_height/2-6, m_width+12, m_height+12+10);

    Circuit::self()->update();
}
