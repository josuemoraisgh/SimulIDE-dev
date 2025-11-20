/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "bcdbase.h"

class LibraryItem;

class BcdTo7S : public BcdBase
{
    public:
        BcdTo7S( QString type, QString id );
        ~BcdTo7S();

 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

        bool propNotFound( QString prop, QString val ) override;

        void stamp() override;
        void updateStep() override;
        void voltChanged() override;
        void runEvent() override { IoComponent::runOutputs(); }

        bool pinReset() { return m_useReset; }
        void setPinReset( bool r );

    private:
        bool m_useReset;

        IoPin* m_resetPin;
};
