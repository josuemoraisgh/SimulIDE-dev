/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QWidget>

#include "ui_currentwidget.h"

class CurrentWidget : public QWidget, private Ui::currentWidget
{
    Q_OBJECT

    public:
        CurrentWidget( QWidget* parent=0 );

 static CurrentWidget* self() { return m_pSelf; }

        //void showCurrentSpeed( bool c );
        double speed() { return m_currentSpeed; }

    public slots:
        void on_currSpeedSlider_valueChanged( int speed );

    private:
        double m_currentSpeed;

 static CurrentWidget* m_pSelf;
};
