/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "cpu8bits.h"
#include "e-element.h"

class Mcs65Interface : public Cpu8bits, public eElement
{
    public:
        Mcs65Interface( eMcu* mcu );
        ~Mcs65Interface();

    protected:
        QString getStrInst( uint8_t IR );
};
