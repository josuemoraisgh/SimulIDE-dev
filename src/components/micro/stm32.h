/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "qemudevice.h"
#include "ioport.h"
#include "qemutwi.h"

class LibraryItem;
class Stm32Pin;

class Stm32 : public QemuDevice
{
    public:
        Stm32( QString type, QString id );
        ~Stm32();

        void stamp() override;

 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

    protected:
        Pin* addPin( QString id, QString type, QString label,
                    int n, int x, int y, int angle , int length=8, int space=0 ) override;

        void createPort( std::vector<Stm32Pin*>* port, QString pId, uint8_t n );
        void createPins();
        bool createArgs() override;
        void doAction() override;

        void cofigPort( uint8_t port,  uint32_t config, uint8_t shift );

        void setPortState( std::vector<Stm32Pin*>* port, uint16_t state );

        uint16_t readInputs( uint8_t port );

        uint16_t m_state[4]; // Port states

        std::vector<Stm32Pin*> m_portA;
        std::vector<Stm32Pin*> m_portB;
        std::vector<Stm32Pin*> m_portC;
        std::vector<Stm32Pin*> m_portD;

        QemuTwi m_i2c[2];
};
