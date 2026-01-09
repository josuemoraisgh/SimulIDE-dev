/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QDial>

class CustomDial : public QDial
{
    Q_OBJECT
    public:
        CustomDial( QWidget* parent=nullptr );

    private:
        virtual void paintEvent( QPaintEvent* e ) override;
        bool eventFilter(QObject *object, QEvent *event) override;

        double m_arrowX;
        double m_arrowY;
};
