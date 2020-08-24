/* 
 * File:   LED_7SEGMENT.h
 * Author: ME
 *
 * Created on October 27, 2019, 2:31 PM
 */

#ifndef LED_7SEGMENT_H
#define	LED_7SEGMENT_H

#ifdef	__cplusplus
extern "C" {
#endif

#define DIGIT_PORT   PORTC
#define DIGIT_TRIS   TRISC
#define SEGMENT_PORT PORTD
#define SEGMENT_TRIS TRISD
#define SEGMENT_BLINK_ON_TIME  100
#define SEGMENT_BLINK_OFF_TIME 100
    
#define DP_0  (1<<0)
#define DP_1  (1<<1)
#define DP_2  (1<<2)
#define DP_3  (1<<3)
#define DP_4  (1<<4)
#define DP_5  (1<<5)

#define BL_0  (1<<0)
#define BL_1  (1<<1)
#define BL_2  (1<<2)
#define BL_3  (1<<3)
#define BL_4  (1<<4)
#define BL_5  (1<<5)    
#define BL_ALARM (1<<6)
    
#define _A    (1<<0)
#define _B    (1<<1)
#define _C    (1<<2)
#define _D    (1<<3)
#define _E    (1<<4)
#define _F    (1<<5)
#define _G    (1<<6)
#define _Dp   (1<<7)

void LED_7SEGMENT_Update(char val[]);


#ifdef	__cplusplus
}
#endif

#endif	/* LED_7SEGMENT_H */

