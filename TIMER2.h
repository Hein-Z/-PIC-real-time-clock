/* 
 * File:   TIMER2.h
 * Author: ME
 *
 * Created on October 19, 2019, 2:43 PM
 */

#ifndef TIMER2_H
#define	TIMER2_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#define ENABLE  1
#define DISABLE 0

#define TMR2_PRESC_1_1   1
#define TMR2_PRESC_1_4   4
#define TMR2_PRESC_1_16  16
    
#define TMR2_POSTSC_1_1  1 
#define TMR2_POSTSC_1_2  2 
#define TMR2_POSTSC_1_3  3 
#define TMR2_POSTSC_1_4  4 
#define TMR2_POSTSC_1_5  5 
#define TMR2_POSTSC_1_6  6 
#define TMR2_POSTSC_1_7  7 
#define TMR2_POSTSC_1_8  8 
#define TMR2_POSTSC_1_9  9 
#define TMR2_POSTSC_1_10  10 
#define TMR2_POSTSC_1_11  11 
#define TMR2_POSTSC_1_12  12 
#define TMR2_POSTSC_1_13  13 
#define TMR2_POSTSC_1_14  14
#define TMR2_POSTSC_1_15  15 
#define TMR2_POSTSC_1_16  16 
    

    
void TMR2_Init(void);
void TMR2_Setup(char period,char presc,char postsc);
void TMR2_Interrupt(char En);
void TMR2_ON(void);
char TMR2_GetIntFlag(void);
void TMR2_ClearIntFlag(void);



#ifdef	__cplusplus
}
#endif

#endif	/* TIMER2_H */

