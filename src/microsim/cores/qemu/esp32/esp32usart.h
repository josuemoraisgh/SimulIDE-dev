/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "qemuusart.h"


class Esp32Usart : public QemuUsart
{
    public:
        Esp32Usart( QemuDevice* mcu, QString name, int n, uint32_t* clk, uint64_t memStart, uint64_t memEnd );
        ~Esp32Usart();

        //void enable( bool e ) override;
        void connected( bool c ) override;

        //void doAction() override;

        void frameSent( uint8_t data ) override;

        //void endTransaction() override;

    private:
        void writeRegister() override;
        void readRegister()  override;

        void writeCR0( uint32_t data );
        void writeCR1( uint32_t data );
};

