/***************************************************************************
 *   Copyright (C) 2026 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "stm32afio.h"

#define EVCR_OFFSET    0x00
#define MAPR_OFFSET    0x04
#define EXTICR1_OFFSET 0x08
#define EXTICR2_OFFSET 0x0c
#define EXTICR3_OFFSET 0x10
#define EXTICR4_OFFSET 0x14


Stm32Afio::Stm32Afio( QemuDevice* mcu, QString name, int n, uint32_t* clk, uint64_t memStart, uint64_t memEnd )
         : QemuModule( mcu, name, n, clk, memStart, memEnd )
{
}
Stm32Afio::~Stm32Afio() {}

void Stm32Afio::reset()
{

}

void Stm32Afio::writeRegister()
{
    uint64_t offset = m_eventAddress - m_memStart;

    switch (offset) {
    case EVCR_OFFSET:    write();          break; //STM32_NOT_IMPL_REG(offset, size);
    case MAPR_OFFSET:    writeMAPR();      break;
    case EXTICR1_OFFSET: writeEXTICR( 0 ); break;
    case EXTICR2_OFFSET: writeEXTICR( 1 ); break;
    case EXTICR3_OFFSET: writeEXTICR( 2 ); break;
    case EXTICR4_OFFSET: writeEXTICR( 3 ); break;
    default:             write();          break;// STM32_BAD_REG(offset, size); break;
    }
}

void Stm32Afio::readRegister()
{
    uint64_t offset = m_eventAddress - m_memStart;

    uint64_t val = 0;

    switch( offset ) {
    case EVCR_OFFSET:    val = read();      break; //STM32_NOT_IMPL_REG(offset, size);
    case MAPR_OFFSET:    val = m_MAPR;      break;
    case EXTICR1_OFFSET: val = m_EXTICR[0]; break;
    case EXTICR2_OFFSET: val = m_EXTICR[1]; break;
    case EXTICR3_OFFSET: val = m_EXTICR[2]; break;
    case EXTICR4_OFFSET: val = m_EXTICR[3]; break;
    default:             val = read();      break; //  STM32_BAD_REG(offset, size);
    }
    m_arena->regData = val;
}

void Stm32Afio::writeMAPR()
{
    uint32_t mapr = m_eventValue & 0x71FFFFF;

    if( m_MAPR == mapr ) return;
    m_MAPR = mapr;

    //s->Remap[STM32_SPI1]  = extract32(s->AFIO_MAPR, AFIO_MAPR_SPI1_REMAP_BIT, 1);
    //s->Remap[STM32_I2C1]  = extract32(s->AFIO_MAPR, AFIO_MAPR_I2C1_REMAP_BIT, 1);
    //s->Remap[STM32_UART1] = extract32(s->AFIO_MAPR, AFIO_MAPR_USART1_REMAP_BIT, 1);
    //s->Remap[STM32_UART2] = extract32(s->AFIO_MAPR, AFIO_MAPR_USART2_REMAP_BIT, 1);
    //s->Remap[STM32_UART3] = (s->AFIO_MAPR & AFIO_MAPR_USART3_REMAP_MASK) >> AFIO_MAPR_USART3_REMAP_START;

    //stm32_timer_remap( 1, (s->AFIO_MAPR & AFIO_MAPR_TIM1_REMAP_MASK) >> AFIO_MAPR_TIM1_REMAP_START );
    //stm32_timer_remap( 2, (s->AFIO_MAPR & AFIO_MAPR_TIM2_REMAP_MASK) >> AFIO_MAPR_TIM2_REMAP_START );
    //stm32_timer_remap( 3, (s->AFIO_MAPR & AFIO_MAPR_TIM3_REMAP_MASK) >> AFIO_MAPR_TIM3_REMAP_START );
    //stm32_timer_remap( 4, extract32(s->AFIO_MAPR, AFIO_MAPR_TIM4_REMAP_BIT, 1) );
}

void Stm32Afio::writeEXTICR( uint8_t index )
{
    uint16_t exticr = m_eventValue;

    if( m_EXTICR[index] == exticr ) return;

    //assert( index < AFIO_EXTICR_COUNT );
    //printf("stm32_afio_EXTICR_write %i %i\n", index, init );fflush( stdout );

    for( int i=0; i<4; i++ ) // Loop through the four EXTI lines controlled by this register.
    {
        uint8_t exti_line = (index*4) + i;
        uint8_t start = i*4;

        uint8_t old_gpio_index = (m_EXTICR[index] >> start) & 0xF;
        uint8_t new_gpio_index = (exticr >> start) & 0xF;
        //printf("stm32_afio_EXTICR_write PORT%c EXTI%i\n", 'A'+new_gpio_index, exti_line );fflush( stdout );
        if( old_gpio_index != new_gpio_index ) // For each line that changed, notify the EXTI module.
        {
            //sysbus_connect_irq( SYS_BUS_DEVICE(s->gpio[old_gpio_index]), exti_line, NULL );
            //sysbus_connect_irq( SYS_BUS_DEVICE(s->gpio[new_gpio_index]), exti_line, qdev_get_gpio_in(DEVICE(s->exti), exti_line) );
        }
        // else if( init ) // Initial configuration
        //     sysbus_connect_irq( SYS_BUS_DEVICE(s->gpio[new_gpio_index]), exti_line, qdev_get_gpio_in(DEVICE(s->exti), exti_line) );
    }
    m_EXTICR[index] = exticr;
}
