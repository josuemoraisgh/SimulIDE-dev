/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "e-resistor.h"
#include "dialed.h"
#include "dialwidget.h"

class VarResBase : public Dialed, public eResistor
{
    public:
        VarResBase( QString type, QString id );
        ~VarResBase();

        void initialize() override;

    protected:
        void updateProxy() override;
};
