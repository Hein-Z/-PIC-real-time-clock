/*
 * File:   main.c
 * Author: ME
 *
 * Created on October 26, 2019, 1:26 PM
 */
// PIC16F887 Configuration Bit Settings
// 'C' source line config statements
// CONFIG1
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator: Crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include "TIMER2.h"
#include "LED_7SEGMENT.h"
#define bitset(var,bitno)    var|=1<<bitno
#define bitclr(var,bitno)    var&=~(1<<bitno)
#define bittest(var,bitno)   (var & (1<<bitno))

#define alarmsetLed  (1<<6)
#define alarmport (1<<7)
#define SW_ENTER  0
#define SW_INC    1
#define SW_DEC    2
#define SW_CANCEL 3
#define SW_ON_OFF 0
#define SW_HOLD   1

void Display_Update(char t[]);
void RTC(void);
void SW1_Scan(void);
void SW2_Scan(void);
void SW3_Scan(void);
void SW4_Scan(void);
void RTC_Control(void);
void Alarm_Control(void);
void SecToTime(unsigned int sec,char t[]);
unsigned int Time2Sec(char t[]);
void PORT_Init(void);
void Alarm_Period(void);
void Alarm_Mode(void);
char SysTick=0;
char rtc [8]={0};
char set [8];
char alarm[8]={1,2,0,0,0,0,DP_1|DP_3,0};
unsigned int alarmsec;
unsigned int rtcsec=12*3600U;
char sw1=0,sw2=0,sw3=0,sw4=0;
char rtcmode=0;
char alarmset=0;
char alarmperiod=0;
char alarmmode=1;
void main(void) 
{
    PORT_Init();
    TMR2_Setup(250,TMR2_PRESC_1_4,TMR2_POSTSC_1_5);
    TMR2_Interrupt(ENABLE);
    TMR2_ON();
    ei();
    
    while(1)
    {
        while(SysTick==0)
            ;
        SysTick=0;
        SW1_Scan();
        SW2_Scan();
        SW3_Scan();
        SW4_Scan();
        
        if(rtcsec>=13*3600U-1)
        rtcsec=3599;
        
        if(rtcsec==alarmsec){               //start alarm
            rtc[7]=BL_0|BL_1|BL_2|BL_3|BL_4|BL_5|BL_ALARM;
         alarmperiod=1;
        }
         Alarm_Mode();
        if (rtcmode==0&&alarmset==0)
          Display_Update(rtc);
         if (alarmmode){
         while(alarmperiod)
            Alarm_Period();
         } 
        RTC();
        Alarm_Control();
       
        if (alarmset){
         Display_Update(alarm);
         rtcmode=0;
        }
        else if(rtcmode){
          Display_Update(set);
          alarmset=0;
        }
        RTC_Control();
       
        }
}
void Alarm_Mode(void){
    if(bittest(sw4,SW_ON_OFF) && alarmmode){
             alarmmode=0;
             sw4=0;
             PORTC&=~(1<<7);
         }
    else if(bittest(sw4,SW_ON_OFF) && alarmmode==0){
             alarmmode=1;
             PORTC|=1<<7;
             sw4=0;
         }
    if(alarmmode)
        PORTC|=1<<7;
    else
        PORTC&=~(1<<7);
         }
void Alarm_Period(void){
    while(SysTick==0)
            ;
        SysTick=0;
        
        if(rtcsec>=13*3600U-1)
        rtcsec=3599;
        
          Display_Update(rtc);
        if(rtcsec==(alarmsec+120)){
            rtc[7]=0;
         alarmperiod=0;
        }
        RTC();
        SW1_Scan();
        if(bittest(sw1,SW_ON_OFF))
        {
            rtc[7]=0;
            alarmperiod=0;
            sw1=0;
        }
}
unsigned int Time2Sec(char t[]){
    unsigned int sec;
    sec=(t[0]*10+t[1])*3600UL+(t[2]*10+t[3])*60+t[4]*10+t[5];
    return sec;
}
void Display_Update(char t[])
{
    rtc[6]=DP_1|DP_3;
 //   rtc[7]=BL_2|BL_5;
    LED_7SEGMENT_Update(t);
}
void RTC(void)
{
    static char counter=0;
    counter++;
    if(counter<200)
        return;
    counter=0;       // 1sec
    rtcsec++;
    SecToTime(rtcsec,rtc);
}
void SecToTime(unsigned int sec,char t[])
{
    unsigned char hr,min;
    hr=0;
    while(sec>=3600)
    {
        sec-=3600;
        hr++;
    }    
    min=0;
    while(sec>=60)
    {
        sec-=60;
        min++;
    }
    if (hr>=10)
    {
        t[0]=1;
        t[1]=hr-10;
    }
    else
    {
        t[0]=0;
        t[1]=hr;
    }
    t[2]=0;
    while(min>=10)
    {
        min-=10;
        t[2]++;
    }
        t[3]=min; 
    t[4]=0;    
    while(sec>=10)
    {
        sec-=10;
        t[4]++;
    }
        t[5]=sec;     
}


void SW1_Scan(void)
{
    enum {IDLE,ON_DEBOUNCE,ON,HOLD,OFF_DEBOUNCE};
    static char State=0;
    static unsigned int counter;
    switch(State)
    {
    case IDLE:
        if(!( bittest(PORTB,SW_ENTER)))  // RP0 == 0
        {
            State=ON_DEBOUNCE;
            counter=10;
        }
        break;
    case ON_DEBOUNCE:
        counter--;
        if(counter==0)
        {
            if(!( bittest(PORTB,SW_ENTER)))  // RP0 == 0
           {
              State=ON;
              counter=600;
              bitset(sw1,SW_ON_OFF);
                                // Switch On
            }
            else
           {
               State=IDLE; 
           }
        }
        break;
    case ON:
          counter--;
          if(bittest(PORTB,SW_ENTER))
            {
                counter=10;
                State=OFF_DEBOUNCE;
            }
          if (counter==0)
          {
             if(! ( bittest(PORTB,SW_ENTER)))  // RP0 == 0
              {
                State=HOLD;
                bitset(sw1,SW_HOLD);
                                 // Hold output
              }             
             else
             {
                 counter=10;
                 State=OFF_DEBOUNCE;
             }
             
          }
        break;
    case HOLD:
             if ( bittest(PORTB,SW_ENTER))  // RB0 == 1
              {
                State=OFF_DEBOUNCE;
                counter=10;                 
              }      
        break;
    case OFF_DEBOUNCE:
         counter--;
         if(counter==0)
         {
            if( bittest(PORTB,SW_ENTER))  // RB0 == 1
              {
                State=IDLE;
              }
            else
                State = HOLD;
         }
        break;
    default:
        State=0;
                
    }
    
    
}

void SW2_Scan(void)
{
    enum {IDLE,ON_DEBOUNCE,ON,HOLD,OFF_DEBOUNCE};
    static char State=0;
    static unsigned int counter;
    switch(State)
    {
    case IDLE:
        if(!( bittest(PORTB,SW_INC)))  // RP0 == 0
        {
            State=ON_DEBOUNCE;
            counter=10;
        }
        break;
    case ON_DEBOUNCE:
        counter--;
        if(counter==0)
        {
            if(!( bittest(PORTB,SW_INC)))  // RP0 == 0
           {
              State=ON;
              counter=600;
              bitset(sw2,SW_ON_OFF);
                                // Switch On
           }
            else
           {
               State=IDLE; 
           }
        }
        break;
    case ON:
          counter--;
          if(bittest(PORTB,SW_INC))
            {
                counter=10;
                State=OFF_DEBOUNCE;
            }
          else if (counter==0)
          {
             if(! ( bittest(PORTB,SW_INC)))  // RP0 == 0
              {
                State=HOLD;
                bitset(sw2,SW_HOLD);
                                 // Hold output
              }             
             else
             {
                 counter=10;
                 State=OFF_DEBOUNCE;
             }
             
          }
        break;
    case HOLD:
             if ( bittest(PORTB,SW_INC))  // RB0 == 1
              {
                State=OFF_DEBOUNCE;
                counter=10;                 
              }      
        break;
    case OFF_DEBOUNCE:
         counter--;
         if(counter==0)
         {
            if( bittest(PORTB,SW_INC))  // RB0 == 1
              {
                State=IDLE;
              }
            else
                State = HOLD;
         }
        break;
    default:
        State=0;
                
    }
    
    
}

void SW3_Scan(void)
{
    enum {IDLE,ON_DEBOUNCE,ON,HOLD,OFF_DEBOUNCE};
    static char State=0;
    static unsigned int counter;
    switch(State)
    {
    case IDLE:
        if(!( bittest(PORTB,SW_DEC)))  // RP0 == 0
        {
            State=ON_DEBOUNCE;
            counter=10;
        }
        break;
    case ON_DEBOUNCE:
        counter--;
        if(counter==0)
        {
            if(!( bittest(PORTB,SW_DEC)))  // RP0 == 0
           {
              State=ON;
              counter=600;
              bitset(sw3,SW_ON_OFF);
                                // Switch On
           }
            else
           {
               State=IDLE; 
           }
        }
        break;
    case ON:
          counter--;
          if(bittest(PORTB,SW_DEC))
            {
                counter=10;
                State=OFF_DEBOUNCE;
            }
        else if (counter==0)
          {
             if(! ( bittest(PORTB,SW_DEC)))  // RP0 == 0
              {
                State=HOLD;
                bitset(sw3,SW_HOLD);
                                 // Hold output
              }             
             else
             {
                 counter=10;
                 State=OFF_DEBOUNCE;
             }
             
          }
        break;
    case HOLD:
             if ( bittest(PORTB,SW_DEC))  // RB0 == 1
              {
                State=OFF_DEBOUNCE;
                counter=10;                 
              }      
        break;
    case OFF_DEBOUNCE:
         counter--;
         if(counter==0)
         {
            if( bittest(PORTB,SW_DEC))  // RB0 == 1
              {
                State=IDLE;
              }
            else
                State = HOLD;
         }
        break;
    default:
        State=0;
                
    }
    
    
}

void SW4_Scan(void)
{
    enum {IDLE,ON_DEBOUNCE,ON,HOLD,OFF_DEBOUNCE};
    static char State=0;
    static unsigned int counter;
    switch(State)
    {
    case IDLE:
        if(!( bittest(PORTB,SW_CANCEL)))  // RP0 == 0
        {
            State=ON_DEBOUNCE;
            counter=10;
        }
        break;
    case ON_DEBOUNCE:
        counter--;
        if(counter==0)
        {
            if(!( bittest(PORTB,SW_CANCEL)))  // RP0 == 0
           {
              State=ON;
              counter=600;
              bitset(sw4,SW_ON_OFF);
                                // Switch On
           }
            else
           {
               State=IDLE; 
           }
        }
        break;
    case ON:
          counter--;
          if(bittest(PORTB,SW_CANCEL))
            {
                counter=10;
                State=OFF_DEBOUNCE;
            }
          else if (counter==0)
          {
             if(! ( bittest(PORTB,SW_CANCEL)))  // RP0 == 0
              {
                State=HOLD;
                bitset(sw4,SW_HOLD);
                                 // Hold output
             }             
             else
             {
                 counter=10;
                 State=OFF_DEBOUNCE;
             }
             
          }
        break;
    case HOLD:
             if ( bittest(PORTB,SW_CANCEL))  // RB0 == 1
              {
                State=OFF_DEBOUNCE;
                counter=10;                 
              }      
        break;
    case OFF_DEBOUNCE:
         counter--;
         if(counter==0)
         {
            if( bittest(PORTB,SW_CANCEL))  // RB0 == 1
              {
                State=IDLE;
              }
            else
                State = HOLD;
         }
        break;
    default:
        State=0;
                
    }
    
    
}
void RTC_Control(void)
{
    char static State=0;
    
    switch(State)
    {
        case 0: //RTC Mode
            if (bittest(sw1,SW_HOLD))   // Enter Hold
            {
                rtcmode=1;
                for(char i=0;i<8;i++)
                {
                    set[i]=rtc[i];
                    State=1;     // Set SEC1
                    set[7]=BL_5;
                }
            }
            break;
        case 1:    // Set SEC1
            if (bittest(sw2,SW_ON_OFF))
            {
                set[5]++;
                if(set[5]>9)
                    set[5]=0;
            }
            else if (bittest(sw3,SW_ON_OFF))
            {
                if (set[5]>0)
                    set[5]--;
                else
                    set[5]=9;
            }
            else if (bittest(sw4,SW_ON_OFF))
            {
                State=0;
                rtcmode=0;
               
            }
            else if(bittest(sw1,SW_HOLD))
            {
                rtcsec=Time2Sec(set);
                for(char i=0;i<6;i++)
                {
                    rtc[i]=set[i];
                    
                }
              
                rtcmode=0;
                State=0;
            }
            else if(bittest(sw1,SW_ON_OFF))
            {
                 set[7]=BL_4;
                 State=2;
            }
            break;
        case 2:    // Set SEC10
            if (bittest(sw2,SW_ON_OFF))
            {
                set[4]++;
                if(set[4]>=6)
                    set[4]=0;
            }
            else if (bittest(sw3,SW_ON_OFF))
            {
                if (set[4]>0)
                    set[4]--;
                else
                    set[4]=5;
            }
            else if (bittest(sw4,SW_ON_OFF))
            {
                State=0;
                rtcmode=0;
                
            }
            else if(bittest(sw1,SW_HOLD))
            {
                rtcsec=Time2Sec(set);
                for(char i=0;i<6;i++)
                {
                    rtc[i]=set[i];
                    
                }
                rtcmode=0;
                
                State=0;
            }   
            else if(bittest(sw1,SW_ON_OFF))
            {
                 set[7]=BL_3;
                 State=3;
            }
            break;
        case 3:    // Set MIN1
            if (bittest(sw2,SW_ON_OFF))
            {
                set[3]++;
                if(set[3]>9)
                    set[3]=0;
            }
            else if (bittest(sw3,SW_ON_OFF))
            {
                if (set[3]>0)
                    set[3]--;
                else
                    set[3]=9;
            }
            else if (bittest(sw4,SW_ON_OFF))
            {
                State=0;
                rtcmode=0;
                
            }
            else if(bittest(sw1,SW_HOLD))
            {
                rtcsec=Time2Sec(set);
                for(char i=0;i<6;i++)
                {
                    rtc[i]=set[i];
                    
                }
                rtcmode=0;
                State=0;
                
            }
            else if(bittest(sw1,SW_ON_OFF))
            {
                 set[7]=BL_2;
                 State=4;
            }
                 
            
            break;
        case 4:    // Set MIN10
              if (bittest(sw2,SW_ON_OFF))
            {
                set[2]++;
                if(set[2]>=6)
                    set[2]=0;
            }
            else if (bittest(sw3,SW_ON_OFF))
            {
                if (set[2]>0)
                    set[2]--;
                else
                    set[2]=5;
            }
            else if (bittest(sw4,SW_ON_OFF))
            {
                State=0;
                rtcmode=0;
                
            }
            else if(bittest(sw1,SW_HOLD))
            {
                rtcsec=Time2Sec(set);
                for(char i=0;i<6;i++)
                {
                    rtc[i]=set[i];
                    
                }
                rtcmode=0;
                State=0;
               
            }   
            else if(bittest(sw1,SW_ON_OFF))
            {
                 set[7]=BL_1|BL_0;
                 State=5;
            }
            break;
        case 5:    // Set HOUR1
   if (bittest(sw2,SW_ON_OFF))
            {
                set[1]++;
                if(set[0]==0){
                if(set[1]>9){
                    set[1]=0;
                    set[0]=1;
                }
                }
                if(set[0]==1){
                    if(set[1]>2){
                        set[1]=1;
                        set[0]=0;
                    }
                }
            }
            else if (bittest(sw3,SW_ON_OFF))
            {
                if(set[0]==1){
                if (set[1]>0)
                    set[1]--;
                else
                {
                    set[0]=0;
                    set[1]=9;
                }
                }
                if(set[0]==0){
                    if(set[1]>1)
                        set[1]--;
                    else{
                        set[0]=1;
                        set[1]=2;
                    }
                }
                
            }
            else if (bittest(sw4,SW_ON_OFF))
            {
                State=0;
                rtcmode=0;
                
            }
            else if(bittest(sw1,SW_HOLD))
            {
                rtcsec=Time2Sec(set);
                for(char i=0;i<6;i++)
                {
                    rtc[i]=set[i];
                    
                }
                rtcmode=0;
                State=0;
                
            }
            else if(bittest(sw1,SW_ON_OFF))
            {
                 set[7]=BL_5;
                 State=1;
            }            
            break;
    }
    sw1=sw2=sw3=sw4=0;
}
void Alarm_Control(void)
{
     char static State=0;
   
    static char alarmcancel[8]={1,2,0,0,0,0,DP_1|DP_3,0};
    switch(State)
    {
        case 0: //RTC Mode
            for(char i=0;i<=7;i++){
                alarm[i]=alarmcancel[i];
            }
            alarmsec=Time2Sec(alarmcancel);
            if (bittest(sw4,SW_HOLD))
            {
                alarmset=1;
                State=1;     // Set SEC1
                alarm[7]=BL_5;
                sw4=0;
            }
            break;
        case 1:
            if(bittest(sw1,SW_HOLD))
            {
                for(char i=0;i<=7;i++){
                alarmcancel[i]=alarm[i];
            }   
                State=0;
                alarmset=0;
                sw1=0;
            }
            else if (bittest(sw1,SW_ON_OFF))// Set SEC1
            {
                alarm[7]=BL_4;
                State=2;
                sw1=0;
            }
            else if (bittest(sw2,SW_ON_OFF))
            {
                alarm[5]++;
                if(alarm[5]>9)
                    alarm[5]=0;
                sw2=0;
            }
            else if (bittest(sw3,SW_ON_OFF))
            {
                if (alarm[5]>0)
                    alarm[5]--;
                else
                    alarm[5]=9;
                sw3=0;
            }
            else if (bittest(sw4,SW_ON_OFF))
            {
                State=0;
                alarmset=0;
               
                sw4=0;
            }
            break;
        case 2:    // Set SEC10
            if(bittest(sw1,SW_HOLD))
            {
                for(char i=0;i<=7;i++){
                alarmcancel[i]=alarm[i];
            }   
               State=0;
                alarmset=0;
                sw1=0;
            }
            else if(bittest(sw4,SW_HOLD))
            {
                for(char i=0;i<=7;i++){
                alarmcancel[i]=alarm[i];
            }
                State=0;
                alarmset=0;
                
             sw4=0;
            }
            else if (bittest(sw1,SW_ON_OFF))
            {
                alarm[7]=BL_3;
                State=3;
                sw1=0;
            }
            else if (bittest(sw2,SW_ON_OFF))
            {
                alarm[4]++;
                if(alarm[4]>5)
                    alarm[4]=0;
                sw2=0;
            }
            else if (bittest(sw3,SW_ON_OFF))
            {
                if (alarm[4]>0)
                    alarm[4]--;
                else
                    alarm[4]=5;
                sw3=0;
            }
            else if (bittest(sw4,SW_ON_OFF))
            {
                State=0;
                alarmset=0;
                sw4=0;
            }
            break;
        case 3:    // Set MIN1
            if(bittest(sw1,SW_HOLD))
            {
                for(char i=0;i<=7;i++){
                alarmcancel[i]=alarm[i];
            }
                State=0;
                alarmset=0;
                sw1=0;
            }
            else if (bittest(sw1,SW_ON_OFF))
            {
                alarm[7]=BL_2;
                State=4;
                sw1=0;
            }
            else if (bittest(sw2,SW_ON_OFF))
            {
                alarm[3]++;
                if(alarm[3]>9)
                    alarm[3]=0;
                sw2=0;
            }
            else if (bittest(sw3,SW_ON_OFF))
            {
                if (alarm[3]>0)
                    alarm[3]--;
                else
                    alarm[3]=9;
                sw3=0;
            }
            else if (bittest(sw4,SW_ON_OFF))
            {
                State=0;
                alarmset=0;
               sw4=0;
            }
            break;
        case 4:    // Set MIN10
            if(bittest(sw1,SW_HOLD))
            {
                for(char i=0;i<=7;i++){
                alarmcancel[i]=alarm[i];
            }
                State=0;
                alarmset=0;
               sw1=0;
            }
            else if (bittest(sw1,SW_ON_OFF))
            {
                alarm[7]=BL_1|BL_0;
                sw1=0;
                State=5;
            }
            else if (bittest(sw2,SW_ON_OFF))
            {
                alarm[2]++;
                if(alarm[2]>5)
                    alarm[2]=0;
                sw2=0;
            }
            else if (bittest(sw3,SW_ON_OFF))
            {
                if (alarm[2]>0)
                    alarm[2]--;
                else
                    alarm[2]=5;
                sw3=0;
            }
            else if (bittest(sw4,SW_ON_OFF))
            {
                State=0;
                alarmset=0;
                
                sw4=0;
            }
            break;
        case 5:  
            if(bittest(sw1,SW_HOLD))
            {
                for(char i=0;i<=7;i++){
                alarmcancel[i]=alarm[i];
            }
                State=0;
                alarmset=0;
                sw1=0;
            }
            else if (bittest(sw1,SW_ON_OFF))
            {
                alarm[7]=BL_5;
                State=1;
                sw1=0;
            }
           if (bittest(sw2,SW_ON_OFF))
            {
                alarm[1]++;
                if(alarm[0]==0){
                if(alarm[1]>9){
                    alarm[1]=0;
                    alarm[0]=1;
                }
                }
                if(alarm[0]==1){
                    if(alarm[1]>2){
                        alarm[1]=1;
                        alarm[0]=0;
                    }
                }
                sw2=0;
            }
            else if (bittest(sw3,SW_ON_OFF))
            {
                sw3=0;
                if(alarm[0]==1){
                if (alarm[1]>0)
                    alarm[1]--;
                else
                {
                    alarm[0]=0;
                    alarm[1]=9;
                }
                }
                if(alarm[0]==0){
                    if(alarm[1]>1)
                        alarm[1]--;
                    else{
                        alarm[0]=1;
                        alarm[1]=2;
                    }
                }
                }
            else if (bittest(sw4,SW_ON_OFF))
            {
                sw4=0;
                State=0;
                alarmset=0;
            }// Set HOUR1
            break;
        default:
            State=0;
        }
}
void PORT_Init(void)
{
    PORTD =0;
    PORTC&=~0xFF;
    TRISD&=~0xFF;
    TRISC&=~0xFF;
    ANSELH =0; // PORTB - Digital IO
    
 //   
}

void __interrupt() ISR(void)
{
    if( TMR2_GetIntFlag())
    {
        TMR2_ClearIntFlag();
        SysTick=1;
    }
}