/*******************************************************************************
 *
 * File Name    : SPi_flash_Reg.c
 *
 * Author       : Gaurav Choubey
 *
 * IC           : AT45DB081-SPI flash # 1MB size # Max Speed (Up to 85Mhz)#
 *
 * Description  : Library for SPi flash AT45DB081E using register level.
 *
 *
 *******************************************************************************/



/********************************************************************************
 *     Include file
 */
#include "header.h"

/********************************************************************************
 *     Extern variable
 */

/********************************************************************************
 *     Global variable for This file
 */


const char SPI_Powerof2[] = {0x3D,0x2A,0x80,0xA6};
const char SPI_DisableProtection[]   =  {0x3D,0x2A,0x76,0x9A};
const char SPI_Manf_ID[] = {0xD7};
//char Manu_ID[10]={0};

char delay = 0;
bool buffer_sel = false;
char stat[2]={0};
//unsigned long i = 0;

char readBackBuffer[257]={0};
/********************************************************************************/

#define eUSCI_TYPE_B_SPI

void SPI_Init(char spi_Clk_sourc,unsigned long spi_Clk_sourc_freq,unsigned long spi_clk_freq)
{


#ifdef  eUSCI_TYPE_A_SPI
    //eUSCI_TYPE_A_SPI selected
       // MOSI port  2.0
       // MISO port  2.1
        //CLK port  1.5




#else
  //eUSCI_TYPE_B_SPI selected
   // MOSI port  1.6
   // MISO port   1.7
//    //CLK port    2.2
   P1SEL1 |= BIT6|BIT7;
   P2SEL1 |= BIT2;

   SPI_CS_PORT_CONFIG;

   SPI_CS_HIGH;


   UCB0CTLW0 |=  UCSWRST;

   UCB0CTLW0 |=UCMSB|UCMST|UCCKPL|UCMODE_0;

   switch(spi_Clk_sourc)
   {
   case SPI_CLK_ACLK:   UCB0CTLW0 |=  UCSSEL__ACLK   ;   break;
   case SPI_CLK_SMCKL:  UCB0CTLW0 |=  UCSSEL__SMCLK  ; break;
   }

   UCB0BRW =  (unsigned int)(spi_Clk_sourc_freq / spi_clk_freq);


   UCB0CTLW0 &= ~UCSWRST;

   SPI_Send_SingleCmd(SPI_Powerof2, 4, 0, 0);

   SPI_Send_SingleCmd(SPI_DisableProtection , 4 , 0 , 0 );

   SPI_Send_SingleCmd(SPI_Manf_ID, 1, stat, 2);

#endif

}



 unsigned char SPI(unsigned char val)
 {

     while(UCB0STATW & UCBUSY);
     UCB0TXBUF = val;
     while(UCB0STATW & UCBUSY);

     return (UCB0RXBUF);

 }
 unsigned char SPI_SerialFlash_WriteBusy( )
 {
     unsigned char temp1;
     //CSLow
         SPI_CS_LOW;
         SPI(SFM_RDSR);
        temp1 = SPI(0xff);

         SPI_CS_HIGH;

     return(!(temp1 & 0x80));
 }


unsigned int SPI_Send_SingleCmd(const char Val[],char num_in,char *ptr,char num_ou)
 {
     unsigned int rval = 0xff;
     char i = 0;


     SPI_CS_LOW;

     for(i = 0; i < num_in; i++ )
     {
         SPI(Val[i]);
     }

     if(num_ou)
     {
         for(i = 0; i < num_ou; i++ )
         {
             ptr[i]=SPI(0xff);
         }
     }

     SPI_CS_HIGH;
     return rval;
 }

 unsigned int SPI_Erase(unsigned char TypeofErase,unsigned long address)
 {
     unsigned int rval = 0xff;
     unsigned char Ad0=0,Ad1=0,Ad2= 0;
     Ad0 = address | 0x00;
     Ad1 = (address >> 8) | 0x00;
     Ad2 = (address >> 16) | 0x00;

     SPI_CS_LOW;
     SPI(TypeofErase);
     SPI(Ad2);
     SPI(Ad1);
     SPI(Ad0);
     SPI_CS_HIGH;


     return rval;
 }

 unsigned char SPI_SingleByteRead(unsigned long Address)
 {
     unsigned char Byte = 0xff;
     unsigned char Ad0=0,Ad1=0,Ad2= 0;
     Ad0 = Address | 0x00;
     Ad1 = (Address >> 8) | 0x00;
     Ad2 = (Address >> 16) | 0x00;

     SPI_CS_LOW;
     SPI(SFM_READ);
     SPI(Ad2);
     SPI(Ad1);
     SPI(Ad0);
     Byte = SPI(0xff);
     SPI_CS_HIGH;
     return Byte;
 }

 unsigned char SPI_SingleByteWrite(unsigned long Address,char data)
 {
     unsigned int rval =0xff;

     unsigned char Ad0=0,Ad1=0,Ad2=0;

     Ad0 = Address | 0x00;
     Ad1 = (Address >> 8) | 0x00;
     Ad2 = (Address >> 16) | 0x00;
     //CSLow;
     SPI_CS_LOW;

     SPI(SFM_WRITE);

     SPI(Ad2);
     SPI(Ad1);
     SPI(Ad0);
     SPI(data);
     SPI_CS_HIGH;

    return rval;
 }

 unsigned int SPI_MultipleBBBRead(unsigned char *Buffer,unsigned long Address,unsigned int numByte)
 {

    unsigned int i = 0;
    unsigned char data = 0;

    for(i = 0; i < numByte; i++)
    {
        data = SPI_SingleByteRead(Address+i);
        if((data == '$') || (data == 0xff)) break;
        *Buffer = data;
         Buffer++;
    }

	while(SPI_SerialFlash_WriteBusy());
    return i;
 }

 unsigned int SPI_MultipleBBBWrite(unsigned char *Buffer,unsigned long Address,unsigned int numByte,bool Appned)
 {
    volatile unsigned int i = 0;


     for(i = 0; i < numByte; i++)
     {
         SPI_SingleByteWrite(Address+i,*Buffer++);
         __delay_cycles(160);
     }

    if(Appned)  SPI_SingleByteWrite(Address+i,'$');

     return i;
 }

 unsigned int SPI_MultipleContinuouseRead(unsigned char *Buffer,unsigned long Address,unsigned int numByte)
 {

     unsigned int i =0;
     unsigned char Ad0=0,Ad1=0,Ad2=0;
     unsigned char data = 0;

     Ad0 = Address | 0x00;
     Ad1 = (Address >> 8) | 0x00;
     Ad2 = (Address >> 16) | 0x00;

     SPI_CS_LOW;
//     for(delay = 0; delay < 3; delay++);

     SPI(SFM_READ);
     SPI(Ad2);
     SPI(Ad1);
     SPI(Ad0);

     for(i = 0; i< numByte; i++)
     {
         data = SPI(0xff);
         if((data == '$') || (data == 0xff)) break;
         *Buffer = data;
         Buffer++;
         while(UCB0STATW &UCBUSY);
     }

     SPI_CS_HIGH;
	 
	 while(SPI_SerialFlash_WriteBusy());

   return i;
 }

 unsigned char SPI_Modify_SingleByteWrite(unsigned long Address,char data)
 {
     unsigned int rval =0xff;

     unsigned char Ad0=0,Ad1=0,Ad2=0;

     Ad0 = Address | 0x00;
     Ad1 = (Address >> 8) | 0x00;
     Ad2 = (Address >> 16) | 0x00;
     //CSLow;
     SPI_CS_LOW;

     SPI(SFM_ReadModifyWrite);

     SPI(Ad2);

     SPI(Ad1);

     SPI(Ad0);

     SPI(data);

     SPI_CS_HIGH;
	 
	  while(SPI_SerialFlash_WriteBusy());
     return rval;

 }

 unsigned int SPI_Modify_MultipleContinuouseWrite( char *Buffer,unsigned long Address,unsigned int numByte,bool Append)
 {
     unsigned long i = 0;

     unsigned char Ad0 = 0,Ad1 = 0,Ad2 = 0;

     Ad0 = Address | 0x00;
     Ad1 = (Address >> 8) | 0x00;
     Ad2 = (Address >> 16) | 0x00;
     //CSLow;
     SPI_CS_LOW;
//     for(delay = 0; delay < 3; delay++);


     SPI(SFM_ReadModifyWrite);
     SPI(Ad2);
     SPI(Ad1);
     SPI(Ad0);
     for(i = 0; i < numByte; i++)
     {

       SPI(*Buffer++);
//       Buffer++;
//       __delay_cycles(1600);
     }

     if(Append)  SPI('$');

//     for(delay = 0; delay < 3; delay++);
     SPI_CS_HIGH;

     while(SPI_SerialFlash_WriteBusy());

//     __delay_cycles(1600000);

     return i;
 }


 unsigned int SPI_ModifyWrite_Flash(char *dataToBeWritten, long startAddress, unsigned long numberOfBytes , bool Append_Dollar)
 {
     volatile uint32_t presentAddress, lastAddressToWrite, numberOfBytesToWrite = 0, endAddress = 0;
     unsigned int byte_Written = 0;

     presentAddress = startAddress;
     lastAddressToWrite = startAddress;
     endAddress = startAddress + numberOfBytes;


     loop:
     while(((lastAddressToWrite++ & 0xff) != 0xff) && lastAddressToWrite < endAddress)
         numberOfBytesToWrite++;

     byte_Written += SPI_Modify_MultipleContinuouseWrite(dataToBeWritten, presentAddress, numberOfBytesToWrite + 1,false);
     presentAddress += numberOfBytesToWrite + 1;

     if(lastAddressToWrite < endAddress)
     {
         lastAddressToWrite = presentAddress;
         dataToBeWritten += numberOfBytesToWrite + 1;
         numberOfBytesToWrite = 0;
         goto loop;
     }

     if(Append_Dollar == true) SPI_Modify_MultipleContinuouseWrite("$", presentAddress, 1, false);

     return byte_Written;
 }

 unsigned int SPI_CheckAndWriteIntoFlash(char *dataToBeWritten, uint32_t numberOfBytestoWrite , uint32_t startAddress, uint32_t endAddress , bool Append_Dollar)
 {
     unsigned rval = 0;

     if(numberOfBytestoWrite < 256)
     {
         SPI_MultipleContinuouseRead((unsigned char *)readBackBuffer, startAddress, (endAddress - startAddress));


         if(strlen((char *)readBackBuffer) == numberOfBytestoWrite)
         {
             if(memcmp(readBackBuffer, dataToBeWritten, numberOfBytestoWrite) == 0)
             {
                 //It's a match.  Don't write into flash
                 rval = numberOfBytestoWrite;
             }
             else
             {
                 rval = SPI_ModifyWrite_Flash(( char *)dataToBeWritten, startAddress, numberOfBytestoWrite , Append_Dollar);

             }
         }
         else
         {
             rval = SPI_ModifyWrite_Flash(( char *)dataToBeWritten, startAddress, numberOfBytestoWrite , Append_Dollar);

         }
     }
     else
     {
         rval =  SPI_ModifyWrite_Flash(( char *)dataToBeWritten, startAddress, numberOfBytestoWrite , Append_Dollar);

     }

     return rval;
 }


 /********************************************************************************************
   * Flash memory Queue is implemented
   *
   ******************************************************************************************/
  unsigned int Insert_into_flash(char *data,unsigned int datalen)
  {
      unsigned int rval = 0xff;

      if( ((CloudSendAddress < SECTOR9_ADDRESSS) && (FlashWriteAdrress+1024 >= MAXIMUM_ALLOWABLE_LENGTH_TO_WRITE_IN_FLASH)) )
      {
               flash_memory_full = true;
                       datalen = 0;
               rval = Flash_FULL;

               Tx_Uart0_String("\n***Flash memory Full occured_1******\n");
      }
      else
      {
          if((FlashWriteAdrress+1024)  >= MAXIMUM_ALLOWABLE_LENGTH_TO_WRITE_IN_FLASH)
          {
              if(CloudSendAddress >= SECTOR9_ADDRESSS)
              {
                  //u can change fFlashWriteAdrress to application_Sa and start writing
                  FlashWriteAdrress = Application_Data_SA;
                  rval =   SPI_ModifyWrite_Flash(data, FlashWriteAdrress, datalen, true);
                  FlashWriteAdrress = FlashWriteAdrress + rval+1;
              }
          }
          else
          {
              if((CloudSendAddress >= SECTOR9_ADDRESSS))
              {

                  if(FlashWriteAdrress == CloudSendAddress)
                  {
                      rval=SPI_ModifyWrite_Flash(data, FlashWriteAdrress, datalen, true);
                      FlashWriteAdrress = FlashWriteAdrress + rval+1;
                  }
                  else
                      if((FlashWriteAdrress + 0xffff) > CloudSendAddress)
                      {
                          flash_memory_full = true;
                          datalen = 0;
                          rval = Flash_FULL;

                          Tx_Uart0_String("\n***Flash memory Full occured_2******\n");
                      }
                      else
                      {
                          rval=SPI_ModifyWrite_Flash(data, FlashWriteAdrress, datalen, true);
                          FlashWriteAdrress = FlashWriteAdrress + rval+1;
                      }
              }
              else
              {
                  rval =  SPI_ModifyWrite_Flash(data, FlashWriteAdrress, datalen, true);
                  FlashWriteAdrress = FlashWriteAdrress + rval+1;
              }
          }

      }

      return rval;
  }

  unsigned int Read_from_flash(char *Buffer,unsigned int Bufferlen)
  {
      unsigned int rval = 0;


      if(FlashWriteAdrress == CloudSendAddress)
      {
          rval = Flash_Empty;

          Tx_Uart0_String("\n***Flash memory empty occured_1******\n");
      }
      else
      {
            if(CloudSendAddress+1024 > MAXIMUM_ALLOWABLE_LENGTH_TO_WRITE_IN_FLASH)
            {

                CloudSendAddress = Application_Data_SA;
                Erase_Sector = SECTOR9_ADDRESSS;
                Flash_State = 3;

                Tx_Uart0_String("\n***Flash memory sector erase occured_1******\n");

            }
            rval = SPI_MultipleContinuouseRead((unsigned char *)Buffer, CloudSendAddress, Bufferlen);


      }


      return  rval;
  }

 void Increment_CloudSendAddress( int len)
 {
     unsigned int l = len;
     volatile long  tmpAddr = 0;
     while((((CloudSendAddress+tmpAddr) & 0xffff) != 0xffff) && len)
     {
         tmpAddr++;
         --len;
     }

     if(len != 0)
     {

         tmpAddr = CloudSendAddress;
         tmpAddr = tmpAddr >> 4;
         tmpAddr = tmpAddr & ~(0xfff);
         Erase_Sector = tmpAddr << 4;
         Flash_State = 3;

         Tx_Uart0_String("\n***Flash memory sector erase occured******\n");
     }


     CloudSendAddress +=l+1;

 }

