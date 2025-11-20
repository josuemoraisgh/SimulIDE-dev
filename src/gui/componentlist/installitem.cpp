/***************************************************************************
 *   Copyright (C) 2025 by santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPushButton>
#include <QDebug>
#include <unistd.h>

#include "installitem.h"
#include "installinfo.h"
#include "installer.h"
#include "mainwindow.h"

InstallItem::InstallItem( Installer* parent, QString item )
           : QWidget( (QWidget*)parent )
{
    m_Installer = parent;
    m_infoWidget = nullptr;

    setupUi( this );
    setItem( item );
}

void InstallItem::setVersion( int64_t v )
{
    if( m_version == v ) setButtonState( bUninstall );
    else                 setButtonState( bUpdate );
}

void InstallItem::setItem( QString itemStr )
{
    m_itemStr = itemStr;

    float scale = MainWindow::self()->fontScale();
    QFont font;
    font.setFamily( MainWindow::self()->defaultFontName() );

    font.setPixelSize( 11*scale );
    textEdit->setFont( font );

    infoButton->setIcon(QIcon(":/help.svg"));
    infoButton->setToolTip( tr("Information") );
    QPalette pi = infoButton->palette();
    pi.setColor( QPalette::Button, "#DDDDDD" );
    infoButton->setPalette( pi );
    //infoButton->setStyleSheet( "background-color:#CCCCCC" );

    updtButton->setIcon(QIcon(":/reset.svg"));
    updtButton->setToolTip( tr("Update") );

    QStringList set = itemStr.split("; ");

    m_name = set.at(0);

    QString header = "#### ";

    QPalette p = setNameEdit->palette();


    if( set.size() < 4 )
    {
        installButton->hide();
        infoButton->hide();
        updtButton->hide();
        gridLayout->setContentsMargins(2,2,2,2);

        font.setPixelSize( 13*scale );
        setNameEdit->setFont( font );

        p.setColor( QPalette::Base, QColor( 220, 235, 240 ) );
        p.setColor( QPalette::Window, QColor( 220, 235, 240 ) );
        p.setColor( QPalette::Text, QColor( 50, 70, 100 ) );
        setNameEdit->setPalette( p );

        this->setAutoFillBackground( true );
        this->setPalette( p );

        textEdit->setVisible( false );

        header = "### ";
    }
    else{
        m_description = set.at(1);
        m_file = set.at(2);
        m_version = set.at(3).toLongLong();

        if( set.size() > 4 ) m_depends = set.at(4);
        if( set.size() > 5 ) m_author  = set.at(5);

        font.setPixelSize( 12*scale );
        setNameEdit->setFont( font );

        p.setColor( QPalette::Text, QColor( 80, 90, 110 ) );
        setNameEdit->setPalette( p );

        textEdit->setMarkdown( m_description );

        setButtonState( bInstall );
    }
    //QString md = header+m_name+"\n"+m_description;
    //md.replace("<br>","\n");

    setNameEdit->setMarkdown( header+m_name );

    QObject::connect( installButton, &QPushButton::clicked, [=](){ installClicked(); } );
    QObject::connect( infoButton   , &QPushButton::clicked, [=](){ infoClicked(); } );
}

void InstallItem::infoClicked()
{
    if( !m_infoWidget )
    {
        QStringList items = m_Installer->getGroupItems( m_name );
        m_infoWidget = new InstallInfo( this , m_description, m_author, items );
        m_infoWidget->setWindowTitle( m_name );
    }
    m_infoWidget->show();
}

void InstallItem::installClicked()
{
    switch( m_buttonState )
    {
        case bInstall:
        case bUpdate:
            /*if( !m_depends.isEmpty() )
            {
                /// TODO: implement multiple dependencies. m_depends = CSV of depends
                m_Installer->installItem( m_depends );
                waitUntillInstalled();
            }*/
            m_Installer->installItem( m_name );
            setButtonState( bUninstall );
            break;
        case bUninstall:
            m_Installer->unInstallItem( m_name );
            setButtonState( bInstall );
            break;
    }
}

void InstallItem::updated()
{
    setButtonState( bUninstall );
}

void InstallItem::setButtonState( buttonState_t state )
{
    m_buttonState = state;

    bool update = (state == bUpdate);
    updtButton->setEnabled( update );

    QPalette pb = updtButton->palette();
    if( update ) pb.setColor( QPalette::Button, "#EEEE00" );
    else         pb.setColor( QPalette::Button, "#EEEEEE" );
    updtButton->setPalette( pb );

    QString color = "#CCCCCC";

    pb = installButton->palette();

    bool installed =(state != bInstall);
    if( installed )
    {
        installButton->setToolTip( tr("Uninstall") );
        installButton->setIcon( QIcon(":/remove.svg") );
        pb.setColor( QPalette::Button, "#FF8070" );
        color = "#DDFFDD";
    }else{
        installButton->setToolTip( tr("Install") );
        installButton->setIcon( QIcon(":/load.svg") );
        pb.setColor( QPalette::Button, "#30DD40" );
        color = "#F8F8FF";
    }
    installButton->setPalette( pb );
    //this->setStyleSheet( "background-color:"+color );
    QPalette p = setNameEdit->palette();
    p.setColor( QPalette::Base, color );
    p.setColor( QPalette::Window, color );
    this->setPalette( p );
    setNameEdit->setPalette( p );
    textEdit->setPalette( p );
}
