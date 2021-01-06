
/* 
 * File:  Lab4.c
 * Author: Idan Koren-Israeli
 *
 * Created on July 17, 2019, 2:20 PM
 */
//#include <xc.h>
#pragma config JTAGEN = OFF     
#pragma config FWDTEN = OFF
#pragma config FNOSC =	FRCPLL
#pragma config FSOSCEN =	OFF
#pragma config POSCMOD =	EC
#pragma config OSCIOFNC =	ON
#pragma config FPBDIV =     DIV_1
#pragma config FPLLIDIV =	DIV_2
#pragma config FPLLMUL =	MUL_20
#pragma config FPLLODIV =	DIV_1
#include <xc.h>  
#include <sys/attribs.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

void setLCD(char top_line[], char bottom_line[]);
void initLCD();
void initTMR2_3();
void initTMR4();
void busyLCD(void);
void initRGB();
void setControlLCD();
void init_timers();
void setRGB(int,int,int);
void sendControlLCD(char control[], int length);
 void __ISR(_TIMER_4_VECTOR, ipl2auto) Timer4SR(void); 
 void __ISR(_TIMER_23_VECTOR, ipl2auto) Timer23SR(void);
void __ISR(_TIMER_1_VECTOR, ipl2auto) Timer1SR(void); 
    
char count_msg[16];

int timer4_count = 0;
int counter = 0;

void __ISR(_TIMER_4_VECTOR, ipl2auto) Timer4SR() 
{
    
    timer4_count++; //0.01 s * 100 = 1 s
    if(timer4_count%100 != 0){
        IFS0bits.T4IF = 0;
        return;
    }
    
   counter++;
    counter %=99;
   sprintf(count_msg, "              %d", counter);
   if(counter < 10){
       count_msg[15] = count_msg[14]; // 1 digit number
       count_msg[14] = ' ';
   }
   setLCD("                ", count_msg);
   IFS0bits.T4IF = 0;
}


void __ISR(_TIMER_23_VECTOR, ipl2auto) Timer23SR()
{
    setRGB(0,1,0);
    
    IFS0bits.T3IF = 0;
    IFS0bits.T2IF = 0;
}

void __ISR(_TIMER_1_VECTOR, ipl2auto) Timer1SR()
{
    setRGB(1,0,0);
    IFS0bits.T1IF = 0;  
}
         
   
 void main(){   
    int j;
    TRISA &= 0xff00;
    PORTA = 0x0;
    IPTMR=0;//INTERRUPT PROXIMITY TIMER REGISTER
    asm("ei");//on interrupt
    initLCD();
    initRGB();
    init_timers();
    
    while(1);   
}

 void init_timers(){
    INTCONbits.MVEC=1;//vector interrupt
    initTMR1();
    initTMR2_3();
    initTMR4();
 }

 void initTMR1(){
         
    PR1 = 0x0C35;                        //             set period register, generates one interrupt every 10 ms
    TMR1 = 0;                           //             initialize count to 0

    T1CONbits.TCKPS0 = 1;                //            1:256 prescale value
    T1CONbits.TCKPS1 = 1; 
    T1CONbits.TGATE = 0;                //             not gated input (the default)
    T1CONbits.TCS = 0;                  //             PCBLK input (the default)
    T1CONbits.ON = 1;                   //             turn on Timer1
    IPC1bits.T1IP = 2;                  //             priority
    IPC1bits.T1IS = 0;                  //             subpriority
    IFS0bits.T1IF = 0;                  //             clear interrupt flag
    IEC0bits.T1IE = 1;
 }
 
 void initTMR2_3(){
    T2CONbits.ON=0;
    T2CONbits.TGATE=0;
    T2CONbits.TCS=0;
    T2CONbits.T32=1;//mode 32bit
    T2CONbits.TCKPS0=1;//????? 256
    T2CONbits.TCKPS1=1;
    T2CONbits.TCKPS2=1;
    T3CONbits.ON=0;
    TMR3=0;//TMRy
    TMR2=0;//TMRx
    PR2=0x8968;//PR2x
    PR3=0x0009;//PR2y
    T2CONbits.ON=1;//start timer
    IPC3bits.T3IP = 2;                  //             priority
    IPC3bits.T3IS = 0;                  //             subpriority
    IFS0bits.T2IF=0;
    IFS0bits.T3IF=0; //interrupt flag
    IEC0bits.T3IE = 1;
    IEC0bits.T2IE = 1;

   

    //while(!IFS0bits.T3IF);
     //counter = 3000;

 }
 
 

void initTMR4(){
    
   //should generate inter. every 1 second (impossible - should be 10 ms instead)
    // Added Image
    
    PR4 = 0x0C35;    // each 10 ms, loop in function of ie
    TMR4 = 0;                           //             initialize count to 0

    T4CONbits.TCKPS0 = 1;                //            1:256 prescale value
    T4CONbits.TCKPS1 = 1; 
    T4CONbits.TCKPS2 = 1; 
    T4CONbits.TGATE = 0;                //             not gated input (the default)
    T4CONbits.TCS = 0;                  //             Non outside pulses
    T4CONbits.ON = 1;                   //             turn on Timer4
    IPC4bits.T4IP = 2;                  //             priority
    IPC4bits.T4IS = 0;                  //             subpriority
    IFS0bits.T4IF = 0;                  //             clear interrupt flag
    IEC0bits.T4IE = 1;

}
 
 
void setLCD(char top_line[], char bottom_line[])
{ 
    int i;
    PORTBbits.RB15=1;//rs=1
    for(i=0;i<24;i++)
    {
        PORTE=top_line[i];
        PORTDbits.RD4=1;
        PORTDbits.RD4=0;
        busyLCD();
    }

    char down_line[]={0xc0};
    sendControlLCD(down_line, 1);
            
    for(i=0;i<24;i++)
    {
        PORTE=bottom_line[i];
        PORTDbits.RD4=1;
        PORTDbits.RD4=0;
        busyLCD();
    }
    
    char up_line[]={0x80};
    sendControlLCD(up_line, 1);
    
}



void busyLCD(void) {
    char RD, RS;
    int STATUS_TRISE;
    int portMap;
    RD = PORTDbits.RD5;
    RS = PORTBbits.RB15;
    STATUS_TRISE = TRISE;
    PORTDbits.RD5 = 1; //w/r
    PORTBbits.RB15 = 0; //rs 
    portMap = TRISE;
    portMap |= 0x80;
    TRISE = portMap;
    do {
        PORTDbits.RD4 = 1; //enable=1
        PORTDbits.RD4 = 0; //enable=0
    } while (PORTEbits.RE7); // BF ??
    PORTDbits.RD5 = RD;
    PORTBbits.RB15 = RS;
    TRISE = STATUS_TRISE;
}


void sendControlLCD(char control[], int length)
{
    int old_RD15 = PORTBbits.RB15;
    int old_RD5 = PORTDbits.RD5;
    int i;
    PORTBbits.RB15=0;//rs=0
    PORTDbits.RD5=0;//for writing
    for(i=0;i<length;i++)
    {
        PORTE=control[i];
        PORTDbits.RD4=1;
        PORTDbits.RD4=0;
        busyLCD();
    }
    
    PORTBbits.RB15=old_RD15;
    PORTDbits.RD5= old_RD5;
}


void initLCD(void){
    TRISBbits.TRISB15 = 0; // RB15 (DISP_RS) set as an output
    ANSELBbits.ANSB15 = 0; // disable analog functionality on RB15 (DISP_RS)
    TRISDbits.TRISD5 = 0; // RD5 (DISP_RW) set as an output
    TRISDbits.TRISD4 = 0; // RD4 (DISP_EN) set as an output
    TRISE &= 0xff00;
    ANSELEbits.ANSE2 = 0;
    ANSELEbits.ANSE4 = 0;
    ANSELEbits.ANSE5 = 0;
    ANSELEbits.ANSE6 = 0;
    ANSELEbits.ANSE7 = 0;
    PORTBbits.RB15 = 0; //rs=0
    PORTDbits.RD5 = 0; //w=0
    
    char control[]={0x38,0x38,0x38,0xe,0x6,0x1};
    sendControlLCD(control, 6);
}

void initRGB(){
    TRISDbits.TRISD2 = 0; // R LED8_R AN25/RPD2/RD2 output
    TRISDbits.TRISD12 = 0; //G LED8_G RPD12/PMD12/RD12 output
    TRISDbits.TRISD3 = 0; // B LED8_B AN26/RPD3/RD3 output
}

void setRGB(int red, int green, int blue){
    PORTDbits.RD2 = red;
    PORTDbits.RD12 = green;
    PORTDbits.RD3 = blue;
}
   