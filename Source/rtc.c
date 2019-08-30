/*
 * rtc.c
 *
 *  Created on: Oct 26, 2017
 *      Author: gaurav
 */
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

/********************************************************************************
 *    Function Defination Start
 */

void Init_RTC(uint8_t sec,uint8_t min,uint8_t hr,uint8_t day,uint8_t month,uint16_t yr)
{
    RTC_B_holdClock(RTC_B_BASE);
    RTC_B_definePrescaleEvent(RTC_B_BASE, RTC_B_PRESCALE_0, RTC_B_PSEVENTDIVIDER_2);
    RTC_B_definePrescaleEvent(RTC_B_BASE, RTC_B_PRESCALE_1, RTC_B_PSEVENTDIVIDER_2);

    RTC_B_setCalendarEvent(RTC_B_BASE,RTC_B_CALENDAREVENT_MINUTECHANGE);

    RTC_B_enableInterrupt(RTC_B_BASE, RTC_B_TIME_EVENT_INTERRUPT);

    RTC_B_clearInterrupt(RTC_B_BASE, RTC_B_TIME_EVENT_INTERRUPT);

   Calendar RTC_time={0};
   RTC_time.Seconds=sec;
   RTC_time.Minutes=min;
   RTC_time.Hours=hr;
   RTC_time.DayOfWeek=day;
   RTC_time.Month=month;
   RTC_time.Year=yr;

   RTC_B_initCalendar(RTC_B_BASE, &RTC_time, RTC_B_FORMAT_BINARY);

   RTC_B_startClock(RTC_B_BASE);

}


#pragma vector = RTC_VECTOR
__interrupt void RTC_ISR(void)
{
    switch(__even_in_range(RTCIV, RTCIV_RT1PSIFG))
        {
            case RTCIV_NONE:      break;        // No interrupts
            case RTCIV_RTCOFIFG:  break;        // RTCOFIFG
            case RTCIV_RTCRDYIFG:               // RTCRDYIFG
                                // Toggles P1.0 every second
                break;
            case RTCIV_RTCTEVIFG:               // RTCEVIFG
            {

            if(++RTC_Counter >= 1)
            {
                RTC_Counter = 0;
                Sync_Occured = true;
            }


            }

                break;
            case RTCIV_RTCAIFG:   break;        // RTCAIFG
            case RTCIV_RT0PSIFG:  break;        // RT0PSIFG
            case RTCIV_RT1PSIFG:  Tx_Uart0_Char('\n');  break;        // RT1PSIFG
            default: break;
        }


}

unsigned char AsciiToBcd(unsigned char *ptr,unsigned char len)
{
    unsigned char bcd;
    unsigned char Buffer[5]={0},i = 0;

    while (len > 0)
    {
        Buffer[i]=(*ptr++);
        len--;
        i++;
    }
    bcd = atoi((char *)Buffer);
    return bcd;
}
