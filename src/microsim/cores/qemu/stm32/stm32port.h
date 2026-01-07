/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "stm32pin.h"
#include "qemumodule.h"
#include "e-element.h"

class Stm32Port : public QemuModule, public eElement
{
    public:
        Stm32Port(  QemuDevice* mcu, QString n, int number, uint32_t* clk=nullptr, uint64_t memStart=0, uint64_t memEnd=0 );
        ~Stm32Port();

        void reset() override;

        //void doAction() override;
        //void runAction() override;

        uint32_t readPort();

        Stm32Pin* getPin( int i ) { return m_pins.at(i); }

        Stm32Pin* createPin( int i, QString id , QemuDevice* mcu );

        uint size(){ return m_pins.size(); }

        //void addEvent( uint64_t address, uint64_t value ) override;

    protected:
        void writeRegister() override;
        void readRegister()  override;

        void cofigPort( uint32_t config, uint8_t shift );
        void setPortState( uint16_t state );

        std::vector<Stm32Pin*> m_pins;

        uint16_t m_pinState;
};
