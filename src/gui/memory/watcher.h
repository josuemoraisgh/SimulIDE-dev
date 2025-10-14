/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QWidget>

#include "ui_watcher.h"

class Watched;
class ValueWidget;
class ValLabelWidget;
class QStandardItemModel;
class ScriptCpu;
class Console;

class Watcher : public QWidget, private Ui::Watcher
{
    Q_OBJECT

    public:
        Watcher( QWidget* parent=0, Watched* cpu=nullptr, bool proxy=false );

        void updateValues();

        void setRegisters( QStringList regs );
        void addRegister( QString name, QString type, QString unit="" );

        void setVariables( QStringList vars );
        void addVariable( QString name, QString type, QString unit="" );

        void loadVarSet( QStringList varSet );
        QStringList getVarSet();

        void addConsole();
        Console* console() { return m_console; }

        void addWidget( QWidget* widget );

        void setProxy( QGraphicsProxyWidget* p );

        void paintEvent( QPaintEvent *event ) override;

    public slots:
        void RegDoubleClick( const QModelIndex& index );
        void VarDoubleClick( const QModelIndex& index );

    protected:
        void mousePressEvent( QMouseEvent* event );
        void mouseMoveEvent( QMouseEvent* event );
        void mouseReleaseEvent( QMouseEvent* event );

        void addHeader();
        void insertValue( QString name );

        bool m_header;

        Watched* m_core;
        Console* m_console;

        QStandardItemModel* m_registerModel;
        QStandardItemModel* m_variableModel;

        QMap<QString, QString> m_typeTable;
        QMap<QString, QString> m_unitTable;
        QMap<QString, ValueWidget*> m_values;
        QMap<QString, ValLabelWidget*> m_valLabels;

        QBoxLayout* m_valuesLayout;

        QPointF m_mousePos;
        QGraphicsProxyWidget* m_proxy;
};

