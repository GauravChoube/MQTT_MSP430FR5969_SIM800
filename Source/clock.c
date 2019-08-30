/*
 * clock.c
 *
 *  Created on: Jun 12, 2017
 *      Author: gaurav
 */

#include <msp430.h>
#include "driverlib/MSP430FR5xx_6xx/driverlib.h"
#include "clock.h"

/**************************************************************************************
 * Definition of clock_init()
 *************************************************************************************/
unsigned char Init_Clock(void)
{

	uint8_t timeout=0xff;

	/**************************************************************************************
	 * Frequency of different Clock source when both crystal enable.
	 * MCLK and SMCLK both run by HFTX crystal running on 16Mhz and
	 * ACLK run by LFXT crystal running on 32Khz.
	 * MCLK  =  16Mhz
	 * SMCLk =  16MHz
	 * ACLK  =  32Khz
	 *************************************************************************************/
#ifdef BOTH_CRSTAL_ENABLE

	/*
	 * Set LFXT and HFXT clock pins to crystal input. By default, they're configured as GPIO.
	 * If you don't configure these pins, your code will be "stuck" in CS_turnOnLFXT function
	 */
	GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_PJ, GPIO_PIN4 + GPIO_PIN5 + GPIO_PIN6 + GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

	CS_setExternalClockSource( LF_CRYSTAL_FREQUENCY_IN_HZ, HF_CRYSTAL_FREQUENCY_IN_HZ );

	/*
	 * Initialize LFXT crystal oscillator without timeout. In case of crystal failure
	 * the code remains 'stuck' in this function.
	 */
	while(--timeout)
	{
		CS_turnOnLFXT( CS_LFXT_DRIVE_0 );
		break;
	}
	if(timeout==0) return LFXT_FAILED;

	/*
	 * Initialize HFXT crystal oscillator without timeout. In case of failure
	 * the code remains 'stuck' in this function.
	 */
	timeout=0xff;
	while(--timeout)
	{
		CS_turnOnHFXT(CS_HFXT_DRIVE_16MHZ_24MHZ);
		break;
	}
	if(timeout==0) return LFXT_FAILED;

	FRAMCtl_configureWaitStateControl(FRAMCTL_ACCESS_TIME_CYCLES_7);

	CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
	//Set SMCLK to use HFXT as its oscillator source (16MHz) SMCLK = 8 MHz
	CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
	//Set MCLK to use HFXT as its oscillator source (16MHz)
	CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);


#endif

	/**************************************************************************************
	 * Frequency of different Clock source when LFXT crystal enable only.
	 * MCLK and SMCLK both run by DCO internal oscillator running on 8Mhz
	 * and ACLK run by LFXT crystal running on 32Khz.
	 * MCLK  =  8Mhz
	 * SMCLk =  8MHz
	 * ACLK  =  32Khz
	 *************************************************************************************/
#ifdef LF_CRSTAL_ENABLE
	CS_setExternalClockSource( LF_CRYSTAL_FREQUENCY_IN_HZ, 0 );
	GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_PJ, GPIO_PIN4 |GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);

	/*
	 * Initialize LFXT crystal oscillator without timeout. In case of crystal failure
	 * the code remains 'stuck' in this function.
	 */
	while(--timeout)
	{
		CS_turnOnLFXT( CS_LFXT_DRIVE_0 );
		break;
	}
	if(timeout==0) return LFXT_FAILED;

	CS_setDCOFreq( CS_DCORSEL_1, CS_DCOFSEL_3 );

	CS_initClockSignal( CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1 );
	// Set ACLK to use VLOCLK as its oscillator source (10KHz)
//	CS_initClockSignal( CS_ACLK, CS_VLOCLK_SELECT, CS_CLOCK_DIVIDER_1 );

	// Set SMCLK to use DCO as its oscillator source
	CS_initClockSignal( CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

	// Set MCLK to use DCO as its oscillator source
	CS_initClockSignal( CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1 );

#endif

	/**************************************************************************************
	 * Frequency of different Clock source when HFXT crystal enable only.
	 * MCLK and SMCLK both run by HFXT crystal running on 16Mhz
	 * and ACLK run by VLO internal oscillator running on 10Khz.
	 * MCLK  =  16Mhz
	 * SMCLk =  16MHz
	 * ACLK  =  10Khz
	 *************************************************************************************/
#ifdef HF_CRSTAL_ENABLE
	CS_setExternalClockSource( 0, HF_CRYSTAL_FREQUENCY_IN_HZ );

	GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_PJ, GPIO_PIN6 |GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);
	timeout=0xff;
	/*
	 * Initialize HFXT crystal oscillator without timeout. In case of failure
	 * the code remains 'stuck' in this function.
	 */
	while(--timeout)
	{
		CS_turnOnHFXT(CS_HFXT_DRIVE_16MHZ_24MHZ);
		break;
	}
	if(timeout==0) return HFXT_FAILED;

	FRAMCtl_configureWaitStateControl(FRAMCTL_ACCESS_TIME_CYCLES_7);


	// Set ACLK to use VLOCLK as its oscillator source (10KHz)
	CS_initClockSignal( CS_ACLK, CS_VLOCLK_SELECT, CS_CLOCK_DIVIDER_1 );
	//Set SMCLK to use HFXT as its oscillator source (16MHz) SMCLK = 16 MHz
	CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
	//Set MCLK to use HFXT as its oscillator source (16MHz)
	CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);


#endif

	/**************************************************************************************
	 * Frequency of different Clock source when no crystal enable.
	 * MCLK and SMCLK both run by DCO internal oscillator running on 8Mhz
	 * and ACLK run by VLO internal oscillator running on 10Khz.
	 * MCLK  =  8Mhz
	 * SMCLk =  8MHz
	 * ACLK  =  10Khz
	 *************************************************************************************/
#ifdef NO_CRSTAL_DCO_ENABLE
	// Set DCO to run at 8MHz
	CS_setDCOFreq( CS_DCORSEL_1, CS_DCOFSEL_3 );


	// Set ACLK to use VLOCLK as its oscillator source (10KHz)
	CS_initClockSignal( CS_ACLK, CS_VLOCLK_SELECT, CS_CLOCK_DIVIDER_1 );

	// Set SMCLK to use DCO as its oscillator source 8MHZ
	CS_initClockSignal( CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

	// Set MCLK to use DCO as its oscillator source 8MHZ
	CS_initClockSignal( CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1 );

#endif

return 0; //success
}
