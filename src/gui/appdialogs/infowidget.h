/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QWidget>

#include "ui_infowidget.h"

class InfoWidget : public QWidget, private Ui::infoWidget
{
    Q_OBJECT

    public:
        InfoWidget( QWidget* parent=0 );

 static InfoWidget* self() { return m_pSelf; }

        void setRate( double rate=0, double simLoad=0, double guiLoad=0, int fps=0 );
        void setCircTime( uint64_t tStep );
        void setTargetSpeed( double s );
        void updtMcu();
        void showCurrentSpeed( bool c );
        double currentSpeed() { return m_currentSpeed; }

    public slots:
        void on_currSpeedSlider_valueChanged( int speed );

    private:
        double m_currentSpeed;

 static InfoWidget* m_pSelf;
};
