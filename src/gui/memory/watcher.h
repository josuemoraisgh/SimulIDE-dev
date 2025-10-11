/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QWidget>

#include "ui_watcher.h"

class Watched;
class ValueWidget;
class QStandardItemModel;
class ScriptCpu;
class Console;

class Watcher : public QWidget, private Ui::Watcher
{
    Q_OBJECT

    public:
        Watcher( QWidget* parent=0, Watched* cpu=nullptr );

        void updateValues();

        void setRegisters( QStringList regs );
        void addRegister( QString name, QString type );

        void setVariables( QStringList vars );
        void addVariable( QString name, QString type );

        void loadVarSet( QStringList varSet );
        QStringList getVarSet();

        void addConsole();
        Console* console() { return m_console; }

        void addWidget( QWidget* widget );

    public slots:
        void RegDoubleClick( const QModelIndex& index );
        void VarDoubleClick( const QModelIndex& index );

    private:
        void addHeader();
        void insertValue( QString name );

        bool m_header;

        Watched* m_core;
        Console* m_console;

        QStandardItemModel* m_registerModel;
        QStandardItemModel* m_variableModel;

        QHash<QString, QString> m_typeTable;
        QHash<QString, ValueWidget*> m_values;

        QBoxLayout* m_valuesLayout;
};
