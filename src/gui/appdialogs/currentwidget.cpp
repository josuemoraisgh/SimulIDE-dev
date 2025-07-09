/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

//#include <QDebug>
#include <math.h>

#include "currentwidget.h"
#include "mainwindow.h"

CurrentWidget* CurrentWidget::m_pSelf = nullptr;

CurrentWidget::CurrentWidget( QWidget* parent )
             : QWidget( parent )
{
    setupUi( this );
    m_pSelf = this;

    QFont font( MainWindow::self()->defaultFontName() , 10, QFont::Bold );
    double fontScale = MainWindow::self()->fontScale();
    font.setPixelSize( 12*fontScale );

    currSpeedLabel->setFont( font );

    currSpeedSlider->setValue( 500 );
    on_currSpeedSlider_valueChanged( 500 );

    //showCurrentWidget( false );
    setVisible( false );
}

/*void CurrentWidget::showCurrentWidget( bool c )
{
    currSpeedLine->setVisible( c );
    currSpeedLabel->setVisible( c );
    currSpeedSlider->setVisible( c );
}*/

void CurrentWidget::on_currSpeedSlider_valueChanged( int speed )
{
    m_currentSpeed = pow( (double)speed/150, 4.5 );
    //qDebug() << m_CurrentWidget;
}
