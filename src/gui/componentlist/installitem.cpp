/***************************************************************************
 *   Copyright (C) 2025 by santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPushButton>
#include <QDebug>
#include <unistd.h>

#include "installitem.h"
#include "installer.h"

InstallItem::InstallItem( Installer* parent, QString item )
           : QWidget( (QWidget*)parent )
{
    m_Installer = parent;
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

    QStringList set = itemStr.split("; ");

    m_name = set.at(0);

    QString header = "#### ";

    if( set.size() < 4 )
    {
        installButton->hide();

        QPalette p = setNameEdit->palette();
        p.setColor( QPalette::Base, QColor( 220, 220, 200 ) );
        setNameEdit->setPalette( p );

        p.setColor( QPalette::Window, QColor( 220, 220, 200 ) );
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

        textEdit->setMarkdown( m_description );
    }
    //QString md = header+m_name+"\n"+m_description;
    //md.replace("<br>","\n");

    setNameEdit->setMarkdown( header+m_name );

    setButtonState( bInstall );

    QObject::connect( installButton, &QPushButton::clicked, [=](){ buttonClicked(); } );
}

void InstallItem::buttonClicked()
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
    QString buttonText;

    switch( state )
    {
        case bInstall:   buttonText = tr("Install");   break;
        case bUpdate:    buttonText = tr("Update");    break;
        case bUninstall: buttonText = tr("Uninstall"); break;
    }
    installButton->setText( buttonText );
}
