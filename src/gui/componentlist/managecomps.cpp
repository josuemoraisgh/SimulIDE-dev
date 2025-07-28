/***************************************************************************
 *   Copyright (C) 2023 by santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QSettings>
#include <QCloseEvent>
#include <QPushButton>
#include <QNetworkReply>

#include "managecomps.h"
#include "componentlist.h"
#include "installitem.h"
#include "treeitem.h"
#include "qzipreader.h"
#include "mainwindow.h"

manCompDialog::manCompDialog( QWidget* parent )
             : QDialog( parent )
{
    setupUi(this);

    m_initialized = false;
    m_checkUpdates = true; //false;
    m_updated = false;

    m_installItem = nullptr;

    table->verticalHeader()->hide(); //setVisible(False)
    table->setHorizontalHeaderLabels( QStringList()<<tr("Name")<<tr("ShortCut") );
    table->setColumnWidth( 0, 300 );
    table->setColumnWidth( 1, 90 );

    installTable->horizontalHeader()->setStretchLastSection( true );

    m_compsUrl = "https://simulide.com/p/direct_downloads/components/";
    //m_compsUrl = "http://localhost/simulide/components/";

    m_compsDir = MainWindow::self()->getConfigPath("components");
    if( !m_compsDir.exists() ) m_compsDir.mkpath(".");

    QSettings* settings = MainWindow::self()->settings();
    QStringList installedList = settings->value("library/installed").toString().split(",");

    for( QString item : installedList ) if( !item.isEmpty() ) m_installed.insert( item, nullptr );

    //checkForUpdates();

    connect( table, &QTableWidget::itemChanged,
              this, &manCompDialog::slotItemChanged, Qt::UniqueConnection );
}

void manCompDialog::loadInstalled()
{
    for( QString item : m_installed.keys() )
    {
        QDir compSetDir = m_compsDir;
        compSetDir.cd( item );
        ComponentList::self()->LoadCompSetAt( compSetDir );
    }
}

void manCompDialog::addItem( TreeItem* treeItem )
{
    QTableWidgetItem* listItem = new QTableWidgetItem();
    listItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable );
    listItem->setText( treeItem->nameTr() );

    if( treeItem->isHidden() ) listItem->setCheckState( Qt::Unchecked );
    else                       listItem->setCheckState( Qt::Checked );

    QTableWidgetItem* shortItem = new QTableWidgetItem();
    shortItem->setText( treeItem->shortcut() );

    int row = table->rowCount();
    table->insertRow( row );
    table->setItem( row, 0, listItem );
    table->setItem( row, 1, shortItem );

    m_treeToList[ listItem ]   = treeItem;
    m_treeToShort[ shortItem ] = treeItem;

    int childCount = treeItem->childCount();
    if( childCount > 0 )
    {
        listItem->setBackground( QColor(240, 235, 245) );
        listItem->setForeground( QBrush( QColor( 110, 95, 50 )));

        for( int i=0; i<childCount; i++ ) addItem( (TreeItem*)treeItem->child( i ) );
    }
    else listItem->setIcon( QIcon(":/blanc.png") );
}

void manCompDialog::initialize()
{
    if( m_initialized ) return;

    QList<QTreeWidgetItem*> itemList = ComponentList::self()->findItems("",Qt::MatchStartsWith);

    for( QTreeWidgetItem* item : itemList ) addItem( (TreeItem*)item );

    //addInstallItem("PIC; 14 bit microcontrollers.; PIC.zip; 2507102250", 0 );
    checkForUpdates();

    m_initialized = true;
}

void manCompDialog::slotItemChanged( QTableWidgetItem* item )
{
    if( !m_initialized ) return;

    if( item->column() == 0 )  // Show/Hide
    {
        TreeItem* treeItem = m_treeToList[ item ];

        bool visible = item->checkState();
        treeItem->setItemHidden( !visible );

        for( int i=0; i<treeItem->childCount(); ++i )
        {
            TreeItem*         childItem = (TreeItem*)treeItem->child( i );
            QTableWidgetItem* listItem  = m_treeToList.keys( childItem ).at(0);

            if( visible ) listItem->setCheckState( Qt::Checked );
            else          listItem->setCheckState( Qt::Unchecked );
        }
    }else                      // Shortcut
    {
        TreeItem* treeItem = m_treeToShort[ item ];
        QString text = item->text().left(1);
        item->setText( text );
        treeItem->setShortCut( text );
        ComponentList::self()->setShortcut( text, treeItem->name() );
    }
}

/*bool manCompDialog::install( QString zipFile )
{
    QString compFolder = MainWindow::self()->getConfigPath("data/components");
    QDir dataDir( compFolder );
    if( !dataDir.exists() ) dataDir.mkpath( compFolder );


    QZipReader qZip( zipFile );
    bool isExtracted = qZip.extractAll( compFolder );

    return isExtracted;
}*/

void manCompDialog::checkForUpdates( QString url )
{
    if( !m_checkUpdates ) return;

    if( url.isEmpty() ) url = m_compsUrl+"dloadset.php?file=components.txt";
    QNetworkRequest request( url );

    request.setAttribute( QNetworkRequest::RedirectPolicyAttribute
                        , QNetworkRequest::NoLessSafeRedirectPolicy );

    request.setTransferTimeout( 5000 );

    m_reply = m_manager.get( request );

    QObject::connect( m_reply, &QNetworkReply::finished,
                               [=](){ updtReady(); } );
}

void manCompDialog::updtReady()
{
    if( m_reply->error() == QNetworkReply::NoError )
    {
        QString    replyStr = m_reply->readAll();
        QStringList setList = replyStr.split("\n"); // List of Component Sets

        int row = 0;
        for( QString itemStr : setList )
        {
            if( itemStr.isEmpty() ) continue;
            addInstallItem( itemStr, row );
            row++;
            //qDebug() <<"manCompDialog::updtReady"<< itemStr;
        }
        installTable->setRowCount( row );
        m_updated = true;
    }
    else qDebug() << "manCompDialog::updtReady ERROR"; // There was a network error

    m_reply->close();
}

void manCompDialog::addInstallItem( QString itemStr, int row )
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
    if( item->m_file.isEmpty() ) installTable->setRowHeight( row, 30 );
    else                         installTable->setRowHeight( row, 60 );
}

void manCompDialog::installItem( QString itemStr )
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

void manCompDialog::unInstallItem( QString itemStr )
{
    QDir compSetDir = m_compsDir;
    compSetDir.cd( itemStr );
    compSetDir.removeRecursively();
    m_installed.remove( itemStr );
}

void manCompDialog::itemDataReady()
{
    bool error = true;
    if( m_reply->error() == QNetworkReply::NoError )
    {
        error = false;
    }
    else qDebug() << "manCompDialog::itemDataReady QNetworkReply ERROR"; // There was a network error

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
            qDebug() << "manCompDialog::itemDataReady ERROR: can't write file" << zipFile ;
            m_installItem = nullptr;
            return;
        }

        //QString setDir =m_compsDir.absolutePath()+ m_installItem->m_name;
        QZipReader qZip( zipFile );
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
        else qDebug() << "manCompDialog::itemDataReady ERROR extracting" << zipFile ;

        QFile::remove( zipFile );
    }
    m_installItem = nullptr;
    if( !m_nextItem.isEmpty() ) installItem( m_nextItem );
}

void manCompDialog::writeSettings()
{
    QString installed = m_installed.keys().join(",");
    QSettings* settings = MainWindow::self()->settings();
    settings->setValue("library/installed", installed );
}

/*void manCompDialog::reject()
{
    this->setVisible( false );

    QHash<QString, QString> shortCuts;

    for( int row=0; row<table->rowCount(); row++ )
    {
        QTableWidgetItem* listItem = table->item( row, 0 );
        bool hidden = ( listItem->checkState() == Qt::Unchecked );
        QString name = m_treeItems.value( listItem )->name();
        MainWindow::self()->compSettings()->setValue( name+"/hidden", hidden );

        QString shortCut = table->item( row, 1 )->text();
        if( !shortCut.isEmpty() )
        {
            shortCuts[name] = shortCut;
            MainWindow::self()->compSettings()->setValue( name+"/shortcut", shortCut );
        }
    }
    ComponentList::self()->setShortCuts( shortCuts );
}*/
