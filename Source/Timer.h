/****************************************************************************
 *  File Name  : GSM.h
 *  Created on : Oct 20, 2018
 *  Author     : Gaurav Choubey
 ****************************************************************************/

#ifndef TIMER_H_
#define TIMER_H_

/*****************************************************************************
 * Varibale
 *****************************************************************************/
unsigned int    SleepTime;
unsigned int    SleepCounter;
bool            SleepTimerOn;

unsigned int    GSMmaxTime ;
unsigned int    GSMmaxCounter;
bool            GSMmaxTimerOn;


/*****************************************************************************
 * macros => 0x3000-0x3FFF
 *****************************************************************************/
#define TIMER_A0
#define TIMER_A1
#define TIMER_A2
//#define TIMER_A3
//#define TIMER_B0
/*****************************************************************************
 * Timer A0 interval micros
 */
#define one_min    60
#define Two_min    120
#define three_min  180
#define ten_min    600
#define nine_Sec   9
/*****************************************************************************/

/*****************************************************************************
 *  function
 *****************************************************************************/
#ifdef TIMER_A0
void Init_GSM_RespTimer();
void Start_GSM_RespTimer();
void Stop_GSM_RespTimer();
#endif

#ifdef TIMER_A1

void Init_GSM_MaxTimer();
void Start_GSM_MaxTimer(unsigned int time);
void Stop_GSM_MaxTimer();

#endif

#ifdef TIMER_A2
void Init_Wait_timer();
void wait_for_second(unsigned int sect);
void Stop_wait_timer();
#endif

#ifdef TIMER_A3
void init_timer_A3();
void Start_TimerA3();
void Stop_TimerA3();
#endif

#ifdef TIMER_B0
void init_timer_B0();
void Start_TimerB0();
void Stop_TimerB0();
#endif


#endif /* TIMER_H_ */
