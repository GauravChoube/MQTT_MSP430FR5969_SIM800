/*
 * clock.h
 *
 *  Created on: Jun 12, 2017
 *      Author: gaurav
 */

#ifndef CLOCK_H_
#define CLOCK_H_
/**************************************************************************************
 * Micros of Clock.c File
 *************************************************************************************/
#define LF_CRYSTAL_FREQUENCY_IN_HZ                      32768
#define HF_CRYSTAL_FREQUENCY_IN_HZ                      16000000

#define LFXT_FAILED          0xF0
#define HFXT_FAILED          0xF1

#define BOTH_CRSTAL_ENABLE
//#define LF_CRSTAL_ENABLE
//#define HF_CRSTAL_ENABLE
//#define NO_CRSTAL_DCO_ENABLE

/**************************************************************************************
 * Declaration of function in Clcok.c file
 * Func  : Clock_Init()
 * Descr : Initialized the three clock from different clock source
 * Param :
 * Return: LFXT_FAILED on failed of low frequency oscillator.
 *         HFXT_FAILED on failed of High frequecny Oscillator
 *         Success on Succeed.
 *************************************************************************************/
unsigned char Init_Clock(void);



#endif /* CLOCK_H_ */
