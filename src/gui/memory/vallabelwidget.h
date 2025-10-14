/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QWidget>

#include "ui_vallabelwidget.h"

class Watched;

class ValLabelWidget : public QWidget, private Ui::ValLabelWidget
{
    Q_OBJECT

    public:
        ValLabelWidget( QString name, QString type, QString unit, Watched* core, QWidget* parent=0 );

        void updateValue();

        void setValueDbl( double val );
        void setValueInt( int val );
        void setValueStr( QString str );

    private:
        QString m_name;
        QString m_type;

        QString m_strVal;
        int m_intVal;

        Watched* m_watched;
};
