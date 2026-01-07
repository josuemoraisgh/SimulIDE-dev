/***************************************************************************
 *   Copyright (C) 2026 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <math.h>

#include "stm32ocunit.h"

Stm32OcUnit::Stm32OcUnit( Stm32Timer* timer, QString name, int number )
           : eElement( name ) // QemuOcUnit( mcu, name, number, nullptr, 0, 0 )
{
    m_timer  = timer;
    m_number = number;
}
Stm32OcUnit::~Stm32OcUnit() {}

void Stm32OcUnit::reset()
{

}

void Stm32OcUnit::writeCCMR( uint8_t ccmr )
{
    if( m_CCMR == ccmr ) return;
    m_CCMR = ccmr;

    uint8_t ccs = ccmr & 0b11;
    if( m_CCS != ccs )
    {
        m_CCS = ccs;

        switch( ccs ) {
        case 0: break; // Output
        case 1: break; // Input TI1
        case 2: break; // Input TI2
        case 3: break; // Input TRC
        default: break;
        }
    }

    if( ccs ) // Input
    {
        uint8_t icpsc = (ccmr & 0b00001100) >> 2;
        m_ICpresc = pow( 2, icpsc );

        //uint8_t icf   = (ccmr & 0b11110000) >> 4;  // Filter not implemented
    }
    else      // Output
    {
        //uint8_t ocfe = (ccmr & 0b00000100) >> 2; // Output compare fast enable
        //uint8_t ocpe = (ccmr & 0b00001000) >> 3; // Output compare preload enable
        uint8_t ocm  = (ccmr & 0b01110000) >> 4;
        if( m_OCM != ocm )
        {
            /// FIXME: should we initialize pin states in all cases?

            m_OCM = ocm;
            switch( ocm ) {                        // OCxREF: signal from which OCx and OCxN are derived
            case 0:                       break;   // Frozen: No output
            case 1:                       break;   // OCxREF High at match
            case 2:                       break;   // OCxREF Low  at match
            case 3:                       break;   // OCxREF Toggle at match
            case 4: setPinState( false ); break;   // OCxREF is forced low
            case 5: setPinState( true );  break;   // OCxREF is forced high
            case 6:                       break;   // PWM mode 1, Birirectional: High below math
            case 7:                       break;   // PWM mode 2, Birirectional: Low below math
            default: break;
            }
        }
        //uint8_t occe = (ccmr & 0b10000000) >> 7;
    }

    //printf("Stm32_channel_write_CCMR ch: %i %i %i\n", ch->number, ch->CCS, ch->OCM);fflush( stdout );
}

void Stm32OcUnit::writeCCER( uint16_t ccer )
{
    uint8_t cce = ccer & 0b01; // Input/Output Enable
    if( m_CCE != cce )
    {
        m_CCE = cce;
        /// Set actual pin out enabled
    }
    m_inverted = ccer & 0b10; // Polarity
}

void Stm32OcUnit::setPinState( bool s )
{

}
