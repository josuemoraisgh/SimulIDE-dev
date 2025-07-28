/***************************************************************************
 *   Copyright (C) 2023 by santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QDialog>
#include <QNetworkAccessManager>
#include <QDir>

#include "ui_managecomps.h"

class TreeItem;
class InstallItem;
class QNetworkReply;

class manCompDialog : public QDialog, private Ui::manCompDialog
{
    public:
        manCompDialog( QWidget* parent );

        void initialize();

        //bool install( QString zipFile );

        void checkForUpdates( QString url="" );

        void loadInstalled();

        void installItem( QString itemStr );
        void unInstallItem( QString itemStr );

        void writeSettings();

    private slots:
        void slotItemChanged( QTableWidgetItem* item );

    private:
        //void reject();
        void addItem( TreeItem* treeItem );
        void addInstallItem( QString itemStr, int row );
        void updtReady();
        void itemDataReady();

        QMap<QTableWidgetItem*, TreeItem*> m_treeToList;
        QMap<QTableWidgetItem*, TreeItem*> m_treeToShort;

        bool m_initialized;
        bool m_checkUpdates;
        bool m_updated;

        QDir m_compsDir;
        QString m_compsUrl;

        QString m_nextItem;

        QMap<QString, InstallItem*> m_items;
        QMap<QString, InstallItem*> m_installed;

        InstallItem* m_installItem;

        QNetworkReply* m_reply;
        QNetworkAccessManager m_manager;
};
