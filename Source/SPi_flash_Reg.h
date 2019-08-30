/*
 * SPi_flash_Reg.h
 *
 *  Created on: Nov 12, 2018
 *      Author: gaurav
 */

#ifndef SOURCE_SPI_FLASH_REG_H_
#define SOURCE_SPI_FLASH_REG_H_


/*****************************************************************************
 * Micros Required in Uart Pheripheral
 *****************************************************************************/
bool flash_memory_full;
unsigned long Erase_Sector;

//flash_State = 1 => chip erase
//flash_State = 2 => Application erase sector 8 - 16
//flash_State = 3 => Sector_erase
//Flash_State = 0 => ideal condition
char Flash_State;


#define Flash_FULL          0x8000
#define Flash_Empty         0x8001




#define SPI_CS_PORT             P1OUT
#define SPI_CS_PORT_Config      P1DIR
#define SPI_CS_PIN              BIT3


#define SPI_CS_HIGH             SPI_CS_PORT |= SPI_CS_PIN
#define SPI_CS_LOW              SPI_CS_PORT &= ~SPI_CS_PIN
#define SPI_CLK_SMCKL           0xA0
#define SPI_CLK_ACLK            0xA1
#define SPI_CS_PORT_CONFIG      SPI_CS_PORT_Config |= SPI_CS_PIN


#define SFM_WRSR                0x01    /*  SFM instruction for Write to Status Register    */
#define SFM_WRITE               0x02    /*  SFM instruction for main memory byte or page program through buffer1 without built-in erase */
#define SFM_READ                0x03    /*  SFM instruction for continous array read(Low frequency) */
#define SFM_WRDI                0x04    /*  SFM instruction for Write Disable */
#define SFM_RDSR                0xD7    /*  SFM instruction for Read from Status Register */
#define SFM_WREN                0x06    /*  SFM instruction for Write Enable */
#define SFM_RDID                0x9F    /*  SFM instruction for Read Device ID */
#define SFM_SectorErase         0x7C    /*  SFM instruction for chip sector Erase */
#define SFM_PageErase           0x81    /*  SFM op-code for Page Erase*/
#define SFM_BlockErase          0x50    // SFM  OpCode for Block Erase
#define SFM_ReadModifyWrite     0x58    // SFM Op-code for Read Modify and Write
#define SFM_PageWrite_erase     0x82    //  SFM Op-Code for Main memory page program through buffer 1 with built in erase
#define SFM_Chip_erase          0xC7
#define SFM_Chip_erase_1        0x94809A

#define SECTOR1_ADDRESSS        0x000000
#define SECTOR2_ADDRESSS        0x010000
#define SECTOR3_ADDRESSS        0x020000
#define SECTOR4_ADDRESSS        0x030000
#define SECTOR5_ADDRESSS        0x040000
#define SECTOR6_ADDRESSS        0x050000
#define SECTOR7_ADDRESSS        0x060000
#define SECTOR8_ADDRESSS        0x070000
#define SECTOR9_ADDRESSS        0x080000
#define SECTOR10_ADDRESSS       0x090000
#define SECTOR11_ADDRESSS       0x0A0000
#define SECTOR12_ADDRESSS       0x0B0000
#define SECTOR13_ADDRESSS       0x0C0000
#define SECTOR14_ADDRESSS       0x0D0000
#define SECTOR15_ADDRESSS       0x0E0000
#define SECTOR16_ADDRESSS       0x0F0000



/*****************************************************************************
 * Function Declaration Required in Uart Pheripheral
 *****************************************************************************/
void          SPI_Init(char spi_Clk_sourc,unsigned long spi_Clk_sourc_freq,unsigned long spi_clk_freq);
unsigned int  SPI_Send_SingleCmd(const char Val[],char num_in,char *ptr,char num_ou);
unsigned int  SPI_Erase(unsigned char TypeofErase,unsigned long address);
unsigned char SPI_SingleByteRead(unsigned long Address);
unsigned char SPI_SingleByteWrite(unsigned long Address,char data);
unsigned int  SPI_MultipleBBBRead(unsigned char *Buffer,unsigned long Address,unsigned int numByte);
unsigned int  SPI_MultipleBBBWrite(unsigned char *Buffer,unsigned long Address,unsigned int numByte,bool Append);
unsigned int  SPI_MultipleContinuouseRead(unsigned char *Buffer,unsigned long Address,unsigned int numByte);
unsigned char SPI_Modify_SingleByteWrite(unsigned long Address,char data);
unsigned int  SPI_Modify_MultipleContinuouseWrite( char *Buffer,unsigned long Address,unsigned int numByte,bool Append);
unsigned char SPI_SerialFlash_WriteBusy( );
unsigned int SPI_CheckAndWriteIntoFlash(char *dataToBeWritten, uint32_t numberOfBytestoWrite , uint32_t startAddress, uint32_t endAddress , bool Append_Dollar);
unsigned int SPI_ModifyWrite_Flash(char *dataToBeWritten, long startAddress, unsigned long numberOfBytes , bool Append_Dollar);

unsigned int Insert_into_flash(char *data,unsigned int datalen);
unsigned int Read_from_flash(char *Buffer,unsigned int Bufferlen);
void Increment_CloudSendAddress(int len);

#endif /* SOURCE_SPI_FLASH_REG_H_ */
