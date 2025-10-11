/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "cpu8bits.h"

Cpu8bits::Cpu8bits( eMcu* mcu )
        : CoreBase()
{
    m_mcu = mcu;

    m_retCycles = 2;

    m_spl = nullptr;
    m_sph = nullptr;
    m_STATUS = nullptr;
}
Cpu8bits::~Cpu8bits() {}

void Cpu8bits::reset()
{
    m_PC = 0;
    m_RET_ADDR = 0;
}
