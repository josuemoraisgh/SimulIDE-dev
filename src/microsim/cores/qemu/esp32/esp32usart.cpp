/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>

#include "esp32usart.h"
#include "qemudevice.h"
#include "usarttx.h"
#include "usartrx.h"
#include "iopin.h"
#include "simulator.h"

#define ETS_UART0_INTR_SOURCE 34  /**< interrupt of UART0, level*/
//#define ETS_UART1_INTR_SOURCE 35/**< interrupt of UART1, level*/
//#define ETS_UART2_INTR_SOURCE 36/**< interrupt of UART2, level*/

#define TXFIFO_EMPTY_INT (1<<1 ); //
#define TX_DONE_INT      (1<<14); //

Esp32Usart::Esp32Usart( QemuDevice* mcu, QString name, int n, uint32_t* clk, uint64_t memStart, uint64_t memEnd )
          : QemuUsart( mcu, name, n, clk, memStart, memEnd )
{
    //m_prescList = {2,4,8,16,32,64,128,256};

}
Esp32Usart::~Esp32Usart(){}

void Esp32Usart::reset()
{
    m_txFifo.clear();
    m_rxFifo.clear();

    m_divider = 0;
    m_baudRate = 115200;

    m_apbClock = 1; /// TODO: implement REF_TICK
}

void Esp32Usart::connected( bool c )
{
    enable( c );
    if( c ) m_sender->getPin()->setPinMode( output );
}

void Esp32Usart::writeRegister()
{
    uint64_t offset = m_eventAddress - m_memStart;
    uint32_t data = m_eventValue;
    //qDebug() << "Esp32Usart::writeRegister" << offset<<data ;

    switch( offset ) {
    case 0x00:{                            // UART_FIFO:
        if( m_txFifo.size() >= 128 )
        {
            qDebug() << "Esp32Usart::writeRegister Tx Fifo overflow";
            return;
        }
        //if( m_baudRate < 9600 )  /// FIXME: data sent at boot
        //{
        //    QemuUsart::frameSent( data );
        //    return;
        //}
        m_txFifo.enqueue( data );
        if( m_txFifo.size() == 1 ) UsartModule::sendByte( m_txFifo.head() );
    }break;
    //case 0x04: break;                                // UART_INT_RAW: RO
    //case 0x08: break;                                // UART_INT_ST:  RO
    //case 0x0C: m_intEn = data;     break;            // UART_INT_ENA:
    //case 0x10: m_intRaw &= ~data;  break;            // UART_INT_CLR:

        ///if (value & R_UART_INT_CLR_RXFIFO_TOUT_MASK) s->rxfifo_tout = false;

    case 0x14:{                                       // UART_CLKDIV:
        //uint32_t clkFra = (data & 0x00F00000) >> 20;
        //uint32_t clkInt = (data & 0x000FFFFF) << 4 ;
        //m_divider = clkInt + clkFra;
        //int baudRate = 115200;
        //if( m_divider ){
        //    uint64_t freq = m_apbClock ? *m_frequency : 1000000; // m_frequency = APB Clock
        //    baudRate = (freq << 4) / m_divider;
        //}
        int br = 1e9/data;
        if( m_baudRate != br )
        {
            m_baudRate = br;
            setPeriod( data*1000 );
        }
        //qDebug() << "Esp32Usart::writeRegister baudRate" << m_baudRate;// << data*1000; //<<m_apbClock<<*m_frequency;
        //freqChanged();
    }break;
    //case 0x18:                                        // UART_AUTOBAUD:
    //    //Autobaud is only used in the ROM bootloader, and it doesn't care if the result is ready immediately.
    //    writeMem( m_memStart+0x30, (data & 1) ? 0x3FF :  0 );

    //    ///if( FIELD_EX32(value, UART_AUTOBAUD, EN) ) s->reg[R_UART_RXD_CNT] = 0x3FF;
    //    ///else                                       s->reg[R_UART_RXD_CNT] = 0;
    //    break;
    //case 0x1C:                break;          // UART_STATUS: RO
    case 0x20: writeCR0();  break;          // UART_CONF0:
    //case 0x24: writeCR1();  break;          // UART_CONF1:
    default: break;
    }
    //updateIrq();
}

void Esp32Usart::readRegister()
{
    uint64_t offset = m_eventAddress - m_memStart;
    uint32_t value = 0;

    switch( offset ) {
    case 0x00:                    // UART_FIFO:
        if( m_rxFifo.size() ) value = m_rxFifo.dequeue();
        break;

    //case 0x04: value = m_intRaw;  break; // UART_INT_RAW: RO
    //case 0x08: value = m_intSt;   break; // UART_INT_ST:  RO
    //case 0x0C: value = m_intEn;   break; // UART_INT_ENA:
    //case 0x10:                    break; // UART_INT_CLR:
  ////case 0x14:                    break; // UART_CLKDIV:
  ////case 0x18:                    break; // UART_AUTOBAUD:
    //case 0x1C:                           // UART_STATUS: RO
    //{
    //    value  = m_rxFifo.size();
    //    value |= m_txFifo.size() << 16;
    //    //qDebug() << "Esp32Usart::readRegister" << m_txFifo.size();
    //} break;
  ////case 0x20:                    break; // UART_CONF0:
  ////case 0x24:                    break; // UART_CONF1:
    //case 0x28:                           // UART_LOWPULSE
    //case 0x2C: value = 337;       break; // UART_HIGHPULSE  /* FIXME: this should depend on the APB frequency */
    //case 0x58:                           // UART_MEM_CONF
    //{
    //    value = 1<<3 | 1<<7;
    //    //r = FIELD_DP32(r, UART_MEM_CONF, RX_SIZE, (unsigned char)(UART_FIFO_LENGTH/128));
    //    //r = FIELD_DP32(r, UART_MEM_CONF, TX_SIZE,  (unsigned char)(UART_FIFO_LENGTH/128));
    //}break;
    //case 0x60:
    //{
    //    //uint32_t fifo_size = fifo8_num_used(&s->rx_fifo);
    //    /* The software only cares about the differene between WR_ADDR and RD_ADDR;
    //     * to keep things simpler, set RD_ADDR to 0 and WR_ADDR to the number of bytes
    //     * in the FIFO. 128 is a special case — write and read pointers should be
    //     * the same in this case.
    //     */
    //    //r = FIELD_DP32(0, UART_MEM_RX_STATUS, WR_ADDR, (fifo_size == 128) ? 0 : fifo_size);
    //    value = m_rxFifo.size() & 0x7F;
    //    value <<= 13;
    //}break;
    //case 0x78: value = 0x15122500; break; // UART_DATE:
    default:   value = read();    break;
    }
    m_arena->regData = value;
    m_arena->qemuAction = SIM_READ;
    //qDebug() << "\nEsp32Usart::readRegister"<<QString::number( offset )<<value ;
}

void Esp32Usart::writeCR0()
{
    uint32_t data = m_eventValue;

    uint8_t parityOdd = (data & 1<<0) ? 1 : 0;
    uint8_t parityEn  = (data & 1<<1) ? 1 : 0;

    if( parityEn ) m_parity = parityOdd ? parODD : parEVEN;
    else           m_parity = parNONE;

    uint8_t dataBits = (data & 0b001100) >> 2;
    setDataBits( 5 + dataBits );

    uint8_t stopBits = (data & 0b110000) >> 4;
    switch( stopBits ) {
    case 0: break;
    case 1: m_stopBits = 1; break;
    case 2: m_stopBits = 1; break;
    case 3: m_stopBits = 2; break;
    }
    //qDebug() << "writeCR0"<< data << 5 + dataBits << m_stopBits;
    uint8_t txFifoRst = data & 1<<18;
    if( txFifoRst ) m_txFifo.clear();

    uint8_t rxFifoRst = data & 1<<17;
    if( rxFifoRst ) m_rxFifo.clear();

    //m_apbClock = (data & 1<<27) ? 1 : 0;
}

//void Esp32Usart::writeCR1()
//{
//    uint32_t data = m_eventValue;
//
//    m_rxFullThrhd  = data & 0b01111111;
//    m_txEmptyThrhd = (data >> 8) & 0b01111111;
//
//    /// //On the ESP32, rx_tout_thres is in units of (bit_time * 8).
//    /// //Note this is different on later chips.
//    /// s->rx_tout_thres = 8 * FIELD_EX32(s->reg[R_UART_CONF1], UART_CONF1, TOUT_THRD);
//    /// s->rx_tout_ena = FIELD_EX32(s->reg[R_UART_CONF1], UART_CONF1, TOUT_EN) != 0;
//    /// esp32_uart_set_rx_timeout(s);
//    /// esp32_uart_update_irq(s);
//}

void Esp32Usart::frameSent( uint8_t data )
{
    QemuUsart::frameSent( data );

    m_txFifo.dequeue();
    if( m_txFifo.size() ) UsartModule::sendByte( m_txFifo.head() );

    //qDebug() << "Esp32Usart::frameSent"<< m_number<< m_sender->m_framesize<<m_txFifo.size()<< Simulator::self()->circTime();
}

//void Esp32Usart::updateIrq()
//{
//    m_intSt = m_intRaw & m_intEn;
//
//    uint8_t irqLevel = m_intSt ? 1 : 0;
//
//    //if( m_irqLevel != irqLevel ){ // Only trigger an interrupt if the IRQ level changes.
//    //    m_irqLevel = irqLevel;
//    //    setInterrupt( ETS_UART0_INTR_SOURCE+m_number, irqLevel );
//    //    //qDebug() << "Esp32Usart::updateIrq"<< ETS_UART0_INTR_SOURCE+m_number<<irqLevel;
//    //}
//
//    // uint32_t rxfifo_tout_raw = (s->rxfifo_tout) ? 1 : 0;
//    // int_raw = FIELD_DP32(int_raw, UART_INT_RAW, RXFIFO_TOUT, rxfifo_tout_raw);
//}

void Esp32Usart::freqChanged()
{
    //if( !m_divider ) return;

    //uint64_t freq = m_apbClock ? *m_frequency : 1000000; // m_frequency = APB Clock
    //int baudRate = (freq << 4) / m_divider;

    //if( m_baudRate == baudRate ) return;
    //setBaudRate( baudRate );
    ////qDebug() << "Esp32Usart::freqChanged baudRate" << baudRate<<m_apbClock<<*m_frequency;
}
