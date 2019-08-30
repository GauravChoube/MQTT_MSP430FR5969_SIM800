/*
 * Timer.c
 *
 *  Created on: May 15, 2017
 *      Author: gaurav
 */
/********************************************************************************
 * Include file
 */
#include "Header.h"

/********************************************************************************
 * Extern Variable
 */

/********************************************************************************
 * Global variable for timer.c file
 */


/***********************************************************************************
 * Timer A0:gsm active timer
 **********************************************************************************/
#ifdef TIMER_A0
void Init_GSM_RespTimer()
{

    //configur the timer
    Timer_A_initUpModeParam updata_a0={0};
    updata_a0.clockSource=TIMER_A_CLOCKSOURCE_ACLK;
    updata_a0.clockSourceDivider=TIMER_A_CLOCKSOURCE_DIVIDER_32;
    updata_a0.timerPeriod=50;                                                                 //set this 100msec
    updata_a0.captureCompareInterruptEnable_CCR0_CCIE=TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE;
    updata_a0.timerInterruptEnable_TAIE =  TIMER_A_TAIE_INTERRUPT_DISABLE;
    updata_a0.startTimer=false;

    Timer_A_initUpMode(TIMER_A0_BASE,&updata_a0);

    Timer_A_clear(TIMER_A0_BASE);
}

void Start_GSM_RespTimer()
{

    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

}

void Stop_GSM_RespTimer()
{
    TA0CTL|=TACLR;
    Timer_A_stop(TIMER_A0_BASE);

}


#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER_A0_isr(void)
{
//    if(GSmbufferExcced == false )
//    {
//        endaddressExtendbuffer = tempsendAddr;
//        extenedbufferReady = true;
//        GSmbufferExcced = true;
//    }

    if(ATCmd == true)
    {
        TA1CTL &=~MC_3;
        TA1CTL|=TACLR;

        Start_Buffering=true;
        GSM_Buffer_Ready=true;
        GSMmaxTimerOn=false;
    }
    else
    {
                Resp_received = true;
    }

    TA0CTL &=~MC_3;
    TA0CTL|=TACLR;

}
#endif
/************************************END********************************************/


/***********************************************************************************
 * Timer A1:timer for maxm wait for each atcmd
 **********************************************************************************/
#ifdef TIMER_A1
void Init_GSM_MaxTimer()
{

    //configur the timer
    Timer_A_initUpModeParam updata_a1={0};
    updata_a1.clockSource=TIMER_A_CLOCKSOURCE_ACLK;
    updata_a1.clockSourceDivider=TIMER_A_CLOCKSOURCE_DIVIDER_1;
    updata_a1.timerPeriod= 32768;                                                        //set to 300msec
    updata_a1.captureCompareInterruptEnable_CCR0_CCIE=TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE;
    updata_a1.timerInterruptEnable_TAIE =  TIMER_A_TAIE_INTERRUPT_DISABLE;
    updata_a1.startTimer=false;

    Timer_A_initUpMode(TIMER_A1_BASE,&updata_a1);

    Timer_A_clear(TIMER_A1_BASE);
}

void Start_GSM_MaxTimer(unsigned int time)
{
    GSMmaxCounter=0;
    GSMCmdtimout=false;
    GSMmaxTime=time;
    GSMmaxTimerOn=true;
    Timer_A_stop(TIMER_A1_BASE);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
   // Tx_Uart1_String("\ntimer max start\n");
}


void Stop_GSM_MaxTimer()
{
    GSMCmdtimout=false;
    Timer_A_stop(TIMER_A1_BASE);
    GSMmaxTimerOn=false;
  //  Tx_Uart1_String("\ntimer max stop\n");
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER_A1_isr(void)
{

    if(++GSMmaxCounter >= GSMmaxTime)
    {
       GSMCmdtimout=true;
        TA1CTL &=~MC_3;
        GSMmaxTimerOn=false;
        GSMmaxCounter=0;
    }
    TA1CTL|=TACLR;
}
#endif

/************************************END********************************************/

/***********************************************************************************
 * Timer A2:Sleep timer =>input in second
 **********************************************************************************/
#ifdef TIMER_A2
void Init_Wait_timer()
{
    //configur the timer
        Timer_A_initUpModeParam updata_a2={0};
        updata_a2.clockSource=TIMER_A_CLOCKSOURCE_ACLK;
        updata_a2.clockSourceDivider=TIMER_A_CLOCKSOURCE_DIVIDER_1;
        updata_a2.timerPeriod=32768;                                                                     //set to 1sec
        updata_a2.captureCompareInterruptEnable_CCR0_CCIE=TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE;
        updata_a2.timerInterruptEnable_TAIE =  TIMER_A_TAIE_INTERRUPT_DISABLE;
        updata_a2.startTimer=false;

        Timer_A_initUpMode(TIMER_A2_BASE,&updata_a2);

        Timer_A_clear(TIMER_A2_BASE);




}

void wait_for_second(unsigned int sect)
{
    SleepCounter=0;
    SleepTime=sect;
    SleepTimerOn=true;
    TA2CTL|=TACLR;
    TA2CTL &= ~MC_3;
    Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);
    while(SleepTimerOn);

}
void Stop_wait_timer()
{

    Timer_A_stop(TIMER_A2_BASE);

}

#pragma vector=TIMER2_A0_VECTOR
__interrupt void TIMER_A2_isr(void)
{
    if(++SleepCounter >= SleepTime)
       {
        SleepCounter=0;
         TA2CTL|=TACLR;
         TA2CTL &= ~MC_3;
          SleepTimerOn=false;
        }
}
#endif
/************************************END********************************************/

/***********************************************************************************
 * Timer A3
 **********************************************************************************/
#ifdef TIMER_A3
void init_timer_A3()
{
	//configur the timer
		Timer_A_initUpModeParam updata_a3={0};
		updata_a3.clockSource=TIMER_A_CLOCKSOURCE_ACLK;
		updata_a3.clockSourceDivider=TIMER_A_CLOCKSOURCE_DIVIDER_32;
		updata_a3.timerPeriod=100;                                                                     //set to 1sec
		updata_a3.captureCompareInterruptEnable_CCR0_CCIE=TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE;
		updata_a3.timerInterruptEnable_TAIE =  TIMER_A_TAIE_INTERRUPT_DISABLE;
		updata_a3.startTimer=false;

		Timer_A_initUpMode(TIMER_A3_BASE,&updata_a3);

		Timer_A_clear(TIMER_A3_BASE);




}
void Start_TimerA3()
{

    Timer_A_startCounter(TIMER_A3_BASE, TIMER_A_UP_MODE);

}
void Stop_TimerA3()
{

    Timer_A_stop(TIMER_A3_BASE);

}
#pragma vector=TIMER3_A0_VECTOR
__interrupt void TIMER_A3_isr(void)
{
//          TA3CTL &=~MC_3;
          TA3CTL|=TACLR;
          Tx_Uart0_Char('\n');
}
#endif
/************************************END********************************************/

/***********************************************************************************
 * Timer B0
 **********************************************************************************/
#ifdef TIMER_B0
void init_timer_B0()
{
    //configur the timer
        Timer_A_initUpModeParam updata_b0={0};
        updata_b0.clockSource=TIMER_A_CLOCKSOURCE_ACLK;
        updata_b0.clockSourceDivider=TIMER_A_CLOCKSOURCE_DIVIDER_32;
        updata_b0.timerPeriod=100;                                                                     //set to 1sec
        updata_b0.captureCompareInterruptEnable_CCR0_CCIE=TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE;
        updata_b0.timerInterruptEnable_TAIE =  TIMER_A_TAIE_INTERRUPT_DISABLE;
        updata_b0.startTimer=false;

        Timer_A_initUpMode(TIMER_B0_BASE,&updata_b0);

        Timer_A_clear(TIMER_B0_BASE);




}
void Start_TimerB0()
{

    Timer_A_startCounter(TIMER_B0_BASE, TIMER_A_UP_MODE);

}
void Stop_TimerB0()
{

    Timer_A_stop(TIMER_B0_BASE);

}

#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMER_B0_isr(void)
{
        //  TB0CTL &=~MC_3;
          TB0CTL|=TACLR;
          Tx_Uart0_Char('\n');
}
#endif
/************************************END********************************************/
