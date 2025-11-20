/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "component.h"
#include "spimodule.h"
#include "iopin.h"

class LibraryItem;
class QFile;

class SdCard : public Component, public SpiModule
{
    public:
        SdCard( QString type, QString id );
        ~SdCard();
        
 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

        void stamp() override;
        void voltChanged() override;

        void endTransaction() override;

        QString getFile() { return m_fileName; }
        void setFile( QString fileName );

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )override;
        void contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu ) override;

    protected:
        void appCommand();
        void command();
        void readData();
        void writeData();

        void readDataBlock();
        void eraseBlocks();

        void resetCard();
        void removeCard();
        void LoadFile();

        QString m_fileName;

        QFile* m_diskImage;

        uint8_t* m_repply;

        uint8_t  m_buffer[512];
        uint8_t  m_reply[72];
        uint8_t  m_rxReg;
        uint32_t m_rxBytes;
        uint32_t m_diskKb;
        uint32_t m_start;
        uint32_t m_end;
        uint32_t m_arg;        // Argument
        uint8_t  m_command;
        uint8_t  m_appCom;
        uint8_t  m_multi;
        uint8_t  m_R1;
        uint8_t  m_doCRC;
        uint8_t  m_boot;
        uint8_t  m_cmdBuff[5];
        uint16_t m_replyBytes;
        uint16_t m_bytesToRead;
        uint16_t m_bytesToWrite;
        uint16_t m_crc16;

        uint8_t m_replyIndex;

        IoPin m_pinCS;
        IoPin m_pinDI;
        IoPin m_pinCK;
        IoPin m_pinDO;

        QPainterPath m_path;

        uint8_t d_Packet[2] ={
            2,
            0,
        };

        uint8_t d_CRC[3] ={
            3,
            0,
            0,
        };

        uint8_t d_CSD[20] ={
            20,    // size
            0xFE,  // start block
            0x40,  // csd_ver:2 reserved1:6
            0X0E,  // taac:8 fixed 0x0E
            0x00,  // nsac:8 fixed 0
            0x5A,  // tran_speed:8
            0x5B,  // ccc_high:8
            0x59,  // ccc_low:4  read_bl_len:4   fixed to 0x09
            0x00,  // read_bl_partial:1 write_blk_misalign:1  read_blk_misalign:1
            0x00,  // reserved3:2 c_size_high:6
            0x00,  // c_size_mid:8
            0x00,  // c_size_low:8
            0x7F,  // reserved4:1 erase_blk_en:1 sector_size_high:6
            0x80,  // sector_size_low:1 wp_grp_size:7
            0x0A,  // wp_grp_enable:1 reserved5:2  r2w_factor:3 write_bl_len_high:2
            0x40,  // write_bl_len_low:2  write_partial:1 reserved6:5
            0x00,  // file_format_grp:1  perm_write_protect:1 copy:1
            0x00,  // crc:7 always1:1
            0x00,  //m_reply[18] = crc16 >> 8;
            0x00   //m_reply[19] = crc16 & 0xFF;  // qDebug() << "sdcard send CSD\n";
        };

        uint8_t d_CID[20] ={
            20,
            0xFE,  // start block
            0x01,  // Manufacturer ID
            0X34,  // OEM/Application high
            0x32,  // OEM/Application low
            'S' ,  // Product name
            'D' ,  // Product name
            'S' ,  // Product name
            'I' ,  // Product name
            'M' ,  // Product name
            0x01,  //  prv_n:4 prv_m:4
            0x12,  // psn
            0x34,  // psn
            0x56,  // psn
            0x78,  // psn
            0x00,  // reserved:4 mdt_year_high:4
            0x04,  // mdt_year_low:4 mdt_month:4
            0x03,  // crc:7 always1:1
            0xFB,  // crc16
            0x8B
        };

        uint8_t d_SCR[12] ={
            12,
            0xFE,  // start block
            0x02,  // SCR_STRUCTURE:4 SD_SPEC :4
            0X25,  // DATA_STAT_AFTER_ERASE:1 SD_SECURITY:3  SD_BUS_WIDTHS:4
            0x80,  // reserved
            0x00,  // reserved
            0x00,  // reserved for manufacturer usage
            0x00,  // reserved for manufacturer usage
            0x00,  // reserved for manufacturer usage
            0x00,  // reserved for manufacturer usage
            0x4c,  // crc16
            0xd7
        };

        uint8_t d_OCR[5] ={
            5,
            0x00,  // Busy:1 CCS:1 resrved:5  S18A:1
            0xFF,  // voltage window
            0x80,  // voltage window:1 reseved:7
            0x00,  // reseved:8
        };

        uint8_t d_COND[5] ={
            5,
            0x00,
            0x00,
            0x01,  // voltage
            0x00,
        };

        uint8_t d_STATUS[68] ={
            68,
            0xFE,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x28,
            0x01,
            0x10,
            0x90,
            0x00,
            0x0B,
            0x05,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0xBC,
            0x65
        };

 static uint8_t CRC7( uint8_t* buffer, uint32_t size )// Generator polynomial: G(x) = x7 + x3 + 1
        {
            uint8_t crc = 0;

            for( uint32_t i=0; i<size; i++) {
                crc ^= buffer[i];
                for( int j=0; j<8; j++) crc = (crc & 1<<7) ? ((crc ^ 0x89) << 1) : (crc << 1);
            }
            return crc >> 1;
        }

 static uint16_t CRC16(  uint8_t* buffer, uint32_t size ) // Generator polynomial: x^16 + x^12 + x^5 + 1
        {
            uint8_t x;
            uint16_t crc = 0;

            for( uint32_t i=0; i<size; i++ ) {
                x = crc >> 8 ^ buffer[i];
                x ^= x >> 4;
                crc = (crc << 8) ^ (uint16_t)(x << 12) ^ (uint16_t)(x << 5) ^ (uint16_t)x;
            }
            return crc;
        }
};
