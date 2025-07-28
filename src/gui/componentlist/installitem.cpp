/***************************************************************************
 *   Copyright (C) 2025 by santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPushButton>
#include <QDebug>
#include <unistd.h>

#include "installitem.h"
#include "managecomps.h"

InstallItem::InstallItem( manCompDialog* parent, QString item )
           : QWidget( (QWidget*)parent )
{
    m_dialog = parent;
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

        QPalette p = textEdit->palette();
        p.setColor( QPalette::Base, QColor( 220, 220, 200 ) );
        textEdit->setPalette( p );

        p.setColor( QPalette::Window, QColor( 220, 220, 200 ) );
        this->setAutoFillBackground( true );
        this->setPalette( p );

        header = "### ";
    }
    else{
        m_description = set.at(1);
        m_file = set.at(2);
        m_version = set.at(3).toLongLong();

        if( set.size() > 4 ) m_depends = set.at(4);
    }
    QString md = header+m_name+"\n"+m_description;
    //md.replace("<br>","\n");
    textEdit->setMarkdown( md );

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
                m_dialog->installItem( m_depends );
                waitUntillInstalled();
            }*/
            m_dialog->installItem( m_name );
            setButtonState( bUninstall );
            break;
        case bUninstall:
            m_dialog->unInstallItem( m_name );
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
