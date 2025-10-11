/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QPlainTextEdit>

#include "updatable.h"

class Watched;

class Console : public QPlainTextEdit, public Updatable
{
    public:
        Console( Watched* cpu, QWidget* parent=nullptr );

        virtual void updateStep() override;

        void appendText( QString text );
        void appendLine( QString line );

    protected:
        void keyPressEvent(QKeyEvent* e) override;
        void mousePressEvent(QMouseEvent* e) override;
        void mouseDoubleClickEvent(QMouseEvent* e) override;
        void contextMenuEvent(QContextMenuEvent* e) override;

    private:
        Watched* m_watched;

        bool m_sendCommand;

        QString m_command;
        QString m_buffer;
};
