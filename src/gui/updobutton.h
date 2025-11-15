/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QPainterPath>
#include <QToolButton>

class UpDoButton : public QToolButton
{
    Q_OBJECT
    public:
        UpDoButton( bool up, QWidget* parent=nullptr );

    private:
        virtual void paintEvent( QPaintEvent* ) override;

        QPainterPath m_path;
        QColor m_color;
};
