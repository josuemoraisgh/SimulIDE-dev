/***************************************************************************
 *   Copyright (C) 2025 by santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QWidget>
#include <QDir>
#include <QNetworkAccessManager>

#include "ui_installer.h"

class InstallItem;
class QNetworkReply;

class Installer : public QWidget, private Ui::installer
{
    public:
        Installer( QWidget* parent );

        void checkForUpdates( QString url="" );

        void loadInstalled();

        void installItem( QString itemStr );
        void unInstallItem( QString itemStr );

        void writeSettings();

    private:
        void addInstallItem( QString itemStr, int row );
        void updtReady();
        void itemDataReady();

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
