/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "mcuwdt.h"
#include "e_mcu.h"
//#include "cpu8bits.h"
#include "mcuinterrupts.h"
#include "simulator.h"

McuWdt::McuWdt( eMcu* mcu, QString name )
      : McuPrescaled( mcu, name )
      , eElement( mcu->getId()+"-"+name )
{
    m_wdtFuse  = false;
}
McuWdt::~McuWdt(){}
