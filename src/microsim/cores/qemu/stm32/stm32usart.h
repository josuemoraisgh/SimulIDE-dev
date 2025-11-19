/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "qemuusart.h"


class Stm32Usart : public QemuUsart
{
    public:
        Stm32Usart( QemuDevice* mcu, QString name, int number );
        ~Stm32Usart();

        enum stm32UsartAction_t{
            STM32_USART_CR1=1,
            STM32_USART_CR2,
//            STM32_USART_READ,
            STM32_USART_WRITE,
            STM32_USART_BAUD,
            STM32_USART_ENABLE,
        };

        void doAction() override;

        //void endTransaction() override;

    private:
        void writeCR1( uint16_t data );
        void writeCR2( uint16_t data );

        uint8_t m_enabled;
};

