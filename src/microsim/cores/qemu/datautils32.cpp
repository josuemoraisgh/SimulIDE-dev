/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "datautils32.h"
//#include "mcudataspace.h"
//
//uint32_t getBitMask( QStringList bitList, DataSpace* mcu ) // Get mask for a group of bits in a Register
//{
//    uint32_t bitMask = 0;
//    for( QString bitName : bitList ) bitMask |= mcu->bitMasks()->value( bitName.remove(" ") );
//    return bitMask;
//}
//
//regBits32_t getRegBits( QString bitNames, DataSpace* mcu ) // Get a set of bits in a Register
//{
//    regBits32_t regBits;
//    QStringList bitList = bitNames.split(",");
//
//    uint32_t mask = getBitMask( bitList, mcu );
//    regBits.mask = mask;
//
//    for( regBits.bit0=0; regBits.bit0<8; ++regBits.bit0 ) // Rotate mask to get initial bit
//    {
//        if( mask & 1 ) break;
//        mask >>= 1;
//    }
//    regBits.regAddr = mcu->bitRegs()->value( bitList.first() );
//    uint32_t* ram = mcu->getRam();
//    regBits.reg = ram + regBits.regAddr;
//
//    return regBits;
//}
//
//void writeBitsToReg( regBits32_t bits, bool value, DataSpace* mcu ) // Set/Clear bits in a Register
//{
//    if( value ) mcu->writeReg( bits.regAddr, *bits.reg |  bits.mask);
//    else        mcu->writeReg( bits.regAddr, *bits.reg & ~bits.mask);
//}
