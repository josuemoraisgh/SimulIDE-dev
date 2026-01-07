/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <inttypes.h>
#include "mcusignal.h"

struct regInfo32_t{
    uint64_t address;
    uint32_t resetVal;
};

struct regBits32_t{
    uint32_t bit0=0;
    uint32_t mask=0;
    uint32_t* reg=0;
    uint64_t regAddr=0;
};
