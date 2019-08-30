/*
 * Mpu.c
 *
 *  Created on: Nov 23, 2017
 *      Author: gaurav
 */

#include "header.h"

 void Init_MPU()
 {
   MPU_initThreeSegmentsParam MPU_val={0};
   MPU_val.seg1boundary=0x0440;
   MPU_val.seg2boundary=0x0FC0;
   MPU_val.seg1accmask |=MPU_READ|MPU_EXEC;
   MPU_val.seg2accmask |=MPU_READ|MPU_EXEC|MPU_WRITE;
   MPU_val.seg3accmask |=MPU_READ|MPU_EXEC;

    MPU_initThreeSegments(MPU_BASE, &MPU_val);

    MPU_disablePUCOnViolation(MPU_BASE, MPU_FIRST_SEG | MPU_SECOND_SEG | MPU_THIRD_SEG);
    MPU_clearAllInterrupts(MPU_BASE);
    MPU_enableNMIevent(MPU_BASE);
    MPU_start(MPU_BASE);
 }


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=SYSNMI_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(SYSNMI_VECTOR)))
#endif
void SYSNMI_ISR(void)
{
    switch(__even_in_range(SYSSNIV,0x18))
    {
    case 0x00: break;
    case 0x02: break;
    case 0x04: break;
    case 0x06: break;
    case 0x08: break;
    case 0x0A: break;
    case 0x0C:

                                 //MPUSEG1IFG
        Tx_Uart1_String("MPU voilation In 1 Segment 1\n");
       PMM_trigBOR();
        break;
    case 0x0E:
        Tx_Uart1_String("MPU voilation In 2 Segment 2\n");
        ////////DebugUart("MPU voilation In 2 Segment 2\n");
        PMM_trigBOR();
        break;                          //MPUSEG2IFG
    case 0x10:
        Tx_Uart1_String("MPU voilation In 3 Segment 3\n");
        ////////DebugUart("MPU voilation In 3 Segment 3\n");
        PMM_trigBOR();
        break;                           //MPUSEG3IFG
    case 0x12: break;
    case 0x14: break;
    case 0x16: break;
    case 0x18: break;
    default: break;
    }
}
