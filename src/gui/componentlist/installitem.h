/***************************************************************************
 *   Copyright (C) 2025 by santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QWidget>

#include "ui_installitem.h"

class manCompDialog;

class InstallItem : public QWidget, private Ui::installItem
{
    friend class manCompDialog;

    public:
        InstallItem( manCompDialog* parent , QString item );

        enum buttonState_t{
            bInstall=0,
            bUpdate,
            bUninstall
        };

        void setItem( QString itemStr );

        void setVersion( int64_t v );

        void updated();

    private:
        void buttonClicked();
        void setButtonState( buttonState_t state );

        QString m_name;
        QString m_description;
        QString m_file;
        QString m_depends;
        int64_t m_version;

        QString m_itemStr;

        buttonState_t m_buttonState;

        manCompDialog* m_dialog;
};
