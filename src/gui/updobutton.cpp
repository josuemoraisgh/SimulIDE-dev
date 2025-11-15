/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "updobutton.h"
#include "mainwindow.h"

#include <QPainter>
#include <QColor>
#include <QIcon>

UpDoButton::UpDoButton( bool up, QWidget* parent )
          : QToolButton( parent )
{
    QFont f = this->font();
    f.setFamily( MainWindow::self()->defaultFontName() );
    f.setPixelSize(11);
    setFont( f );

    setMaximumSize( 6, 6 );
    setStyleSheet("border: 0px; background: transparent;");
    setCheckable( false );
    setCursor( Qt::PointingHandCursor );

    m_path = QPainterPath();
    if( up ) {
        m_path.moveTo( 1.5, 4.2 );
        m_path.lineTo( 4.5, 4.2 );
        m_path.lineTo(   3, 1.4 );
        m_path.lineTo( 1.5, 4.2 );

        m_color = QColor( 255,50,20 );
    }else{
        m_path.moveTo( 1.5, 1.8 );
        m_path.lineTo( 4.5, 1.8 );
        m_path.lineTo(   3, 4.6 );
        m_path.lineTo( 1.5, 1.8 );

        m_color = QColor( 50,50,240 );
    }
}

void UpDoButton::paintEvent( QPaintEvent* )
{
    QPainter painter(this);
    painter.setRenderHint( QPainter::Antialiasing ); // Smooth out the circle

    int h = height()-2;
    int w = width();

    QRectF rect = QRectF( 0.5, 0.5,5,5 );

    QLinearGradient linearGrad( QPointF(w/2, 0), QPointF(w/2, h) );

    if( this->isDown() || isChecked() ){
        linearGrad.setColorAt( 0, QColor( 230, 230, 230 ) );
        linearGrad.setColorAt( 1, QColor( 180, 180, 180 ) );
    }else{
        linearGrad.setColorAt(0, Qt::white);
        linearGrad.setColorAt(1, QColor( 200, 200, 200 ) );
    }
    painter.setBrush( linearGrad );

    QPen pen( QColor( 80, 80, 80 ), .5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    painter.setPen( pen );
    painter.drawRoundedRect( rect, 1, 1 );
    painter.setPen( QPen(Qt::NoPen) );

    QPen pen2( Qt::black, .5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    painter.setPen( pen2 );
    painter.setBrush( m_color );
    painter.drawPath( m_path );
}
