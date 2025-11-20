/***************************************************************************
 *   Copyright (C) 2025 by santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QDialog>

#include "ui_installinfo.h"

class InstallItem;

class InstallInfo : public QDialog, private Ui::installInfo
{
    //Q_OBJECT

    public:
        InstallInfo( InstallItem* parent , QString tittle, QString author, QStringList items );
};
