/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef PIC12CORE_H
#define PIC12CORE_H

#include "picmrcore.h"

class Pic12Core : public PicMrCore
{
    public:
        Pic12Core( eMcu* mcu );
        ~Pic12Core();

        virtual void runStep() override;

    protected:
        virtual void setBank( uint8_t bank ) override { PicMrCore::setBank( bank ); }
        virtual void decode( uint16_t instr ) override;

        void TRIS();

        uint16_t m_TRISaddr;
};

#endif
