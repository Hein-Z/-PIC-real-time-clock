
#include <xc.h>


void TMR2_Setup(char period,char presc,char postsc)
{
    PR2= period-1;
    T2CON &= ~3;
    switch(presc)
    {
        case 1:
            break;
        case 4:
            T2CON|=1;
            break;
        case 16:
            T2CON|=1<<1;
            break;
        default:
            while(1)
                ;
                
    }
    if (postsc<=16)
    {
        T2CON &=~( 0xF<<3);
           if(postsc!=0)
             T2CON|= (postsc-1)<<3;
        
    }
    else
    {
        while(1)
            ;
    }
}


void TMR2_Init(void)
{
     PR2=249;     //255;
    TMR2ON=1;    //  T2CON |= 1<<2;
    //T2CON |=3<<3; // postsc = 1:4     //1;   // presc = 1:4
    T2CON |=4<<3   |  // postsc=5
            1;        // presc =4
    TMR2IE=1;    //  PIE1  |= 1<<1;
    PEIE=1;      //  INTCON |= 1<< 7 | 1<<6;
    GIE=1;
}

void TMR2_ON(void)
{
    T2CON|=1<<2;
}

void TMR2_Interrupt(char En) // En=1 Enable, En=0 Disable
{
    PIR1 &=~(1<<1);   // clear interrupt flag
    if (En)
        PIE1 |=1<<1;
    else
        PIE1 &=~(1<<1);
    
    INTCON |=1<<6;  // PEIE=1
}

char TMR2_GetIntFlag(void)
{
     return (PIR1 & (1<<1))  ? 1:0;
}

void TMR2_ClearIntFlag(void)
{
    PIR1&=~(1<<1);
}

