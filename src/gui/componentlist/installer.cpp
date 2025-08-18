/***************************************************************************
 *   Copyright (C) 2025 by santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QSettings>
#include <QPushButton>
#include <QNetworkReply>

#include "installer.h"
#include "componentlist.h"
#include "installitem.h"
#include "mainwindow.h"
#include "qzipreader.h"

Installer::Installer( QWidget* parent )
         : QWidget( parent )
{
    setupUi(this);

    m_checkUpdates = true; //false;
    m_updated = false;

    m_installItem = nullptr;

    installTable->horizontalHeader()->setStretchLastSection( true );

    m_compsUrl = "https://simulide.com/p/direct_downloads/components/";
    //m_compsUrl = "http://localhost/simulide/components/";

    m_compsDir = MainWindow::self()->getConfigPath("components");
    if( !m_compsDir.exists() ) m_compsDir.mkpath(".");

    QSettings* settings = MainWindow::self()->settings();
    QStringList installedList = settings->value("library/installed").toString().split(",");

    for( QString item : installedList ) if( !item.isEmpty() ) m_installed.insert( item, nullptr );
}

void Installer::loadInstalled()
{
    for( QString item : m_installed.keys() )
    {
        QDir compSetDir = m_compsDir;
        compSetDir.cd( item );
        ComponentList::self()->LoadCompSetAt( compSetDir );
    }
}

void Installer::checkForUpdates( QString url )
{
    if( !m_checkUpdates ) return;

    //QString version = MainWindow::self()->settings()->value("library/version").toString();
    //qDebug() << "version" << version;

    if( url.isEmpty() ) url = m_compsUrl+"dloadset.php?file=components.txt";
    QNetworkRequest request( url );

    request.setAttribute( QNetworkRequest::RedirectPolicyAttribute
                        , QNetworkRequest::NoLessSafeRedirectPolicy );

    request.setTransferTimeout( 5000 );

    m_reply = m_manager.get( request );

    QObject::connect( m_reply, &QNetworkReply::finished,
                     [=](){ updtReady(); } );
}

void Installer::updtReady()
{
    if( m_reply->error() == QNetworkReply::NoError )
    {
        QString    replyStr = m_reply->readAll();
        QStringList setList = replyStr.split("\n"); // List of Component Sets

        //if( setList.size() < 2 ) return;
        //QString version = setList.takeFirst();
        //if( version = m_version ) return;
        //m_version = version;
        //MainWindow::self()->settings()->setValue("library/version", version );

        int row = 0;
        for( QString itemStr : setList )
        {
            if( itemStr.isEmpty() ) continue;
            addInstallItem( itemStr, row );
            row++;
            //qDebug() <<"Installer::updtReady"<< itemStr;
        }
        installTable->setRowCount( row );
        m_updated = true;
    }
    else qDebug() << "Installer::updtReady ERROR"; // There was a network error

    m_reply->close();
}

void Installer::addInstallItem( QString itemStr, int row )
{
    InstallItem* item = new InstallItem( this, itemStr );

    if( m_installed.contains( item->m_name ) )
    {
        InstallItem* installed = m_installed.value( item->m_name );

        if( installed ) item->setVersion( installed->m_version );
        else{
            m_installed[item->m_name] = item;
            item->updated();
        }
    }
    m_items.insert( item->m_name, item );

    installTable->insertRow( row );
    installTable->setCellWidget( row, 0, item );

    float scale = MainWindow::self()->fontScale();

    if( item->m_file.isEmpty() ) installTable->setRowHeight( row, 25*scale );
    else                         installTable->setRowHeight( row, 50*scale );
}

void Installer::installItem( QString itemStr )
{
    InstallItem* item = m_items.value( itemStr );

    if( !item->m_depends.isEmpty() && !m_installed.contains( item->m_depends ) )
    {
        m_nextItem = itemStr;
        itemStr = item->m_depends;
        item = m_items.value( itemStr );
    }
    else m_nextItem.clear();

    if( !item ) return;

    qDebug() << "Installing Component Set:" << itemStr;

    QString url = m_compsUrl+"dloadset.php?file="+item->m_file;
    QNetworkRequest request( url );

    request.setAttribute( QNetworkRequest::RedirectPolicyAttribute
                        , QNetworkRequest::NoLessSafeRedirectPolicy );

    request.setTransferTimeout( 5000 );

    m_reply = m_manager.get( request );

    QObject::connect( m_reply, &QNetworkReply::finished,
                     [=](){ itemDataReady(); } );

    m_installItem = item;
}

void Installer::unInstallItem( QString itemStr )
{
    QDir compSetDir = m_compsDir;
    compSetDir.cd( itemStr );
    compSetDir.removeRecursively();
    m_installed.remove( itemStr );

    ComponentList::self()->createList();
}

void Installer::itemDataReady()
{
    bool error = true;
    if( m_reply->error() == QNetworkReply::NoError )
    {
        error = false;
    }
    else qDebug() << "Installer::itemDataReady QNetworkReply ERROR"; // There was a network error

    m_reply->close();

    if( !error )
    {
        QString zipFile = m_compsDir.filePath( m_installItem->m_file );

        QFile file( zipFile );
        if( file.exists() ) QFile::remove( zipFile );

        if( file.open( QIODevice::WriteOnly | QIODevice::Append ))
        {
            file.write( m_reply->readAll() );
            file.close();
        }else{
            qDebug() << "Installer::itemDataReady ERROR: can't write file" << zipFile ;
            m_installItem = nullptr;
            return;
        }

        //QString setDir =m_compsDir.absolutePath()+ m_installItem->m_name;
        qZipReader qZip( zipFile );
        bool isExtracted = qZip.extractAll( m_compsDir.absolutePath() );

        if( isExtracted )
        {
            QDir compSetDir = m_compsDir;
            compSetDir.cd( m_installItem->m_name );
            ComponentList::self()->LoadCompSetAt( compSetDir );

            m_installItem->updated();
            m_installed.insert( m_installItem->m_name, m_installItem );
            qDebug() << m_installItem->m_name <<"Installed";
        }
        else qDebug() << "Installer::itemDataReady ERROR extracting" << zipFile ;

        QFile::remove( zipFile );
    }
    m_installItem = nullptr;
    if( !m_nextItem.isEmpty() ) installItem( m_nextItem );
}

void Installer::writeSettings()
{
    QString installed = m_installed.keys().join(",");
    QSettings* settings = MainWindow::self()->settings();
    settings->setValue("library/installed", installed );
}
