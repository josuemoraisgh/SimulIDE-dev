/***************************************************************************
 *   Copyright (C) 2025 by santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "installinfo.h"

#include <QLabel>
InstallInfo::InstallInfo( InstallItem* parent , QString tittle, QString author, QStringList items )
           : QDialog( (QWidget*)parent )
{
    setupUi( this );

    tittleLabel->setText("**"+tittle+"**");
    authorLabel->setText("**By:** "+author);

    QString list;
    for( QString item : items )
    {
        list += item + "\n";
    }
    listEdit->setText( list );
}
