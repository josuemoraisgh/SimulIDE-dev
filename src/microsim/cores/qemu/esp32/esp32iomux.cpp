/***************************************************************************
 *   Copyright (C) 2026 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "esp32iomux.h"
#include "esp32gpio.h"

Esp32IoMux::Esp32IoMux( QemuDevice* mcu, QString name, int n, uint32_t* clk, uint64_t memStart, uint64_t memEnd)
          : QemuModule( mcu, name, n, clk, memStart, memEnd )
{
    m_gpio = nullptr;
}
Esp32IoMux::~Esp32IoMux() {}

void Esp32IoMux::reset()
{
    for (int i = 0; i < 40; ++i) {
        m_iomuxReg[i] = 0;
    }
}

void Esp32IoMux::readRegister()
{
    uint64_t offset = m_eventAddress - m_memStart;
    uint16_t val = 0;

    uint32_t index = offset/4;
    if( index < 40 ) val = m_iomuxReg[index];

    m_arena->regData = val;
    m_arena->qemuAction = SIM_READ;
}

void Esp32IoMux::writeRegister()
{
    uint64_t offset = m_eventAddress - m_memStart;

    int pin = getMuxGpio( offset );
    if( pin < 0 ) return;

    uint32_t index = offset/4;
    if( m_iomuxReg[index] == m_eventValue ) return;
    m_iomuxReg[index] = m_eventValue;

    m_gpio->writeIoMuxReg( pin, m_eventValue );
}

int Esp32IoMux::getMuxGpio( uint64_t addr )
{
    switch (addr) {
    case 0x04: return 36;
    case 0x08: return 37;
    case 0x0C: return 38;
    case 0x10: return 39;
    case 0x1C: return 32;
    case 0x14: return 34;
    case 0x18: return 35;
    case 0x20: return 33;
    case 0x24: return 25;
    case 0x28: return 26;
    case 0x2C: return 27;
    case 0x30: return 14;  //MTMS
    case 0x34: return 12;  //MTDI
    case 0x38: return 13;  //MTCK
    case 0x3C: return 15;  //MTDO
    case 0x40: return  2;
    case 0x44: return  0;
    case 0x48: return  4;
    case 0x4C: return 16;
    case 0x50: return 17;
    case 0x54: return  9;  //SD_DATA2
    case 0x58: return 10;  //SD_DATA3
    case 0x5C: return 11;  //SD_CMD
    case 0x60: return  6;  //SD_CLK
    case 0x64: return  7;  //SD_DATA0
    case 0x68: return  8;  //SD_DATA1
    case 0x6C: return  5;
    case 0x70: return 18;
    case 0x74: return 19;
    case 0x78: return 20;
    case 0x7C: return 21;
    case 0x80: return 22;
    case 0x84: return  3;  //U0RXD
    case 0x88: return  1;  //U0TXD
    case 0x8C: return 23;
    case 0x90: return 24;
    default: break;
    }
    return -1;
}
