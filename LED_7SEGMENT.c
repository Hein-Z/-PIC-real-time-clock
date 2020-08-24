#include <xc.h>
#include "LED_7SEGMENT.h"

const char SegTab[16]={ _A | _B | _C | _D | _E | _F ,      //0
                        _B | _C  ,                         //1
                        _A | _B | _D | _E | _G ,           //2
                        _A | _B | _C | _D | _G ,           //3
                        _B | _C |_F | _G ,                 //4
                        _A | _C | _D | _F | _G ,           //5
                        _A | _C | _D | _E | _F | _G ,      //6
                        _A | _B | _C ,                     //7
                        _A | _B | _C | _D | _E | _F | _G , //8
                        _A | _B | _C | _D | _F | _G ,      //9
                        _A | _B | _C | _E | _F | _G ,      //A
                        _C | _D | _E | _F | _G ,           //b
                        _D | _E | _G ,                     //c
                        _B | _C | _D | _E | _G ,           //d
                        _A | _D | _E | _F | _G ,           //E 
                        _A | _E | _F | _G };               //F
char BlinkStatus=0;
void LED_7SEGMENT_Update(char val[])
{
   static char State=0;
   static char BlinkCounter=0;
   
   BlinkCounter++;
   if ( BlinkCounter==SEGMENT_BLINK_ON_TIME)
   {
       BlinkStatus=1;
   }
   else if( BlinkCounter>=(SEGMENT_BLINK_ON_TIME+SEGMENT_BLINK_OFF_TIME) )
   {
       BlinkStatus=0;
       BlinkCounter=0;
   }
   
   SEGMENT_PORT=0;     // Turn off segments
   DIGIT_PORT&=~0x3F; // Turn off drivers
   PORTC&=~0x40;
  if( val[7] & BL_ALARM && BlinkStatus==1)//alarm
      PORTC|=1<<6;
   
   switch(State)
   {
       case 0:
           SEGMENT_PORT = SegTab[val[0]];
           if( val[6] & DP_0)
           SEGMENT_PORT |=1<<7;    
           if ( val[7] & BL_0 && BlinkStatus==1 )
           {
               SEGMENT_PORT=0;
           }
           
           DIGIT_PORT|=1<<5;
           State=1;
           break;
       case 1:
           SEGMENT_PORT = SegTab[val[1]];
           if( val[6] & DP_1)
           SEGMENT_PORT |=1<<7;   
           if ( val[7] & BL_1 && BlinkStatus==1 )
           {
               SEGMENT_PORT=0;
           }
           
           
           DIGIT_PORT|=1<<4;
           State=2;
           break;
       case 2:
           SEGMENT_PORT = SegTab[val[2]];
           if( val[6] & DP_2)
             SEGMENT_PORT |=1<<7;   
           if ( val[7] & BL_2 && BlinkStatus==1 )
           {
               SEGMENT_PORT=0;
           }
           
           DIGIT_PORT|=1<<3;
           State=3;
           break;
       case 3:
           SEGMENT_PORT = SegTab[val[3]];
           
           if( val[6] & DP_3)
             SEGMENT_PORT |=1<<7;   
           if ( val[7] & BL_3 && BlinkStatus==1 )
           {
               SEGMENT_PORT=0;
           }
           DIGIT_PORT|=1<<2;
           State=4;
           break;    
       case 4:
           SEGMENT_PORT = SegTab[val[4]];
           if( val[6] & DP_4)
             SEGMENT_PORT |=1<<7;   
           if ( val[7] & BL_4 && BlinkStatus==1 )
           {
               SEGMENT_PORT=0;
           }
           DIGIT_PORT|=1<<1;
           State=5;
           break;
       case 5:
           SEGMENT_PORT = SegTab[val[5]];
           if( val[6] & DP_5)
           SEGMENT_PORT |=1<<7;   
            if ( val[7] & BL_5 && BlinkStatus==1 )
           {
               SEGMENT_PORT=0;
           }
          DIGIT_PORT|=1<<0;
           State=0;
           break;
       default:
           State=0;
   }
 }