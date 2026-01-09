/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QStringList>

#include "mcutypes32.h"

//class DataSpace;
//
//uint32_t getBitMask( QStringList bitList, DataSpace* mcu ); // Get mask for a group of bits in a Register
//
//regBits32_t getRegBits( QString bitNames, DataSpace* mcu ); // Get a set of consecutive bits in a Register
//
//static inline uint32_t overrideBits( uint32_t val, regBits32_t bits ) // Replace bits in val with current value in register bits.reg
//{
//    return (val & ~bits.mask) | (*(bits.reg) | bits.mask);
//}

static inline void replaceBits( uint32_t val, regBits32_t bits ) // Replace bits in register with value
{
    *bits.reg &= ~bits.mask;
    *bits.reg |= val & bits.mask;
}

static inline uint32_t getRegBits( uint32_t val, regBits32_t rb )
{
    return (val & rb.mask);
}

static inline uint32_t getRegBitsVal( regBits32_t rb )
{
    return (*rb.reg & rb.mask)>>rb.bit0;
}

static inline uint32_t getRegBitsVal( uint32_t val, regBits32_t rb )
{
    return (val & rb.mask)>>rb.bit0;
}

static inline uint32_t getRegBitsBool( regBits32_t rb )
{
    return (*rb.reg & rb.mask) > 0;
}

static inline uint32_t getRegBitsBool( uint32_t val, regBits32_t rb )
{
    return (val & rb.mask) > 0;
}

static inline void setRegBits( regBits32_t bits )
{
    *(bits.reg) |= bits.mask;
}

static inline void clearRegBits( regBits32_t bits ) // Clear bits in a Register
{
    *(bits.reg) &= ~bits.mask;
}

static inline void writeRegBits( regBits32_t bits, bool value ) // Set/Clear bits in a Register
{
    if( value ) setRegBits( bits );
    else        clearRegBits( bits );
}

//void writeBitsToReg( regBits32_t bits, bool value, DataSpace* mcu ); // Write bits in a Register calling watchers

