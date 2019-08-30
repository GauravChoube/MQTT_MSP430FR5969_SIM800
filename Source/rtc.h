/*
 * rtc.h
 *
 *  Created on: Oct 26, 2017
 *      Author: gaurav
 */




void Init_RTC(uint8_t sec,uint8_t min,uint8_t hr,uint8_t day,uint8_t month,uint16_t yr);
unsigned char AsciiToBcd(unsigned char *ptr,unsigned char len);
char RTC_Counter;
