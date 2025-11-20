/***************************************************************************
 *   Copyright (C) 2020 by Jan K. S.                                       *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified 2020 by Santiago González                                    *
 *   santigoro@gmail.com                                                   *
 *                                                                         */

#include <QVariant>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>

#include "joystickwidget.h"

#define STICK_PERCENTAGE_SIZE 0.6

JoystickWidget::JoystickWidget()
{
    m_changed = true;
}
JoystickWidget::~JoystickWidget() {}

void JoystickWidget::setupWidget()
{
    setObjectName( QString::fromUtf8("joystick") );
    
    m_xValue = 500;
    m_yValue = 500;
    
    m_grabCenter = false;
    
    setAttribute( Qt::WA_NoSystemBackground) ;
    
    m_movingOffset = center();
    updateOutputValues();
}

void JoystickWidget::updateOutputValues()
{
    m_xValue = m_movingOffset.x() * 1000 / width();
    m_yValue = m_movingOffset.y() * 1000 / height();

    m_changed = true;
    update();
}
QRectF JoystickWidget::centerEllipse()
{
    int radius = (width()-16)*STICK_PERCENTAGE_SIZE/2;
    QRectF rect = QRectF(-radius, -radius, 2*radius, 2*radius );

    if( m_grabCenter ) return rect.translated( m_movingOffset );
    return rect.translated(center());
}

QPointF JoystickWidget::center()
{
    return QPointF( width()/2, height()/2 );
}

void JoystickWidget::mousePressEvent( QMouseEvent* event )
{
    if( event->button() == Qt::LeftButton )
        m_grabCenter = centerEllipse().contains( event->pos() );
    else QWidget::mousePressEvent( event );
}
 
void JoystickWidget::mouseMoveEvent( QMouseEvent* event )
{
    if( !m_grabCenter ) return;

    QLineF limitLine = QLineF(center(), event->pos() );
    int maxDistance = (width()-24)/2;
    if( limitLine.length() > maxDistance ) limitLine.setLength( maxDistance );

    m_movingOffset = limitLine.p2();
    updateOutputValues();
}

void JoystickWidget::mouseReleaseEvent( QMouseEvent* event )
{
    m_grabCenter = false;
    m_movingOffset = center();
    updateOutputValues();
}

void JoystickWidget::paintEvent( QPaintEvent* )
{
    QPainter painter(this);
    painter.setRenderHint( QPainter::Antialiasing );
    
    int size = width()-16;
    int x0 = width()/2;
    int y0 = height()/2;
    QRectF bounds = QRectF(-size/2,-size/2, size, size ).translated( center() );
    QRadialGradient lg0( QPointF( x0-3, y0-3 ), 20, QPointF( x0-30, y0-30 ) );
    lg0.setColorAt( 0, QColor( 80, 80, 80 ) );
    lg0.setColorAt( 1, QColor( 0, 0, 0 ) );
    painter.setBrush( lg0 );
    painter.drawEllipse( bounds );

    x0 = centerEllipse().x();
    y0 = centerEllipse().y();

    QRadialGradient lg( QPointF(x0+10, y0 ), 17, QPointF( x0-5, y0 ) );
    lg.setColorAt( 0, QColor( 180, 180, 180 ) );
    lg.setColorAt( 1, QColor( 50, 50, 50 ) );
    painter.setBrush( lg );
    painter.drawEllipse( centerEllipse() );
}
