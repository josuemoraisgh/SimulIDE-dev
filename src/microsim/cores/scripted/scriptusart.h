/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "scriptperif.h"
#include "mcuuart.h"

class ScriptCpu;

class asIScriptFunction;

class ScriptUsart : public McuUsart, public ScriptPerif
{
    public:
        ScriptUsart( eMcu* mcu, QString name, int number );
        ~ScriptUsart();

        void reset() override;
        void byteReceived( uint8_t data ) override;
        void frameSent( uint8_t data ) override;

        QStringList registerScript( ScriptCpu* cpu ) override;
        void startScript() override;

    private:
        //QString m_uartName;

        asIScriptFunction* m_byteReceived;
        asIScriptFunction* m_frameSent;
};
