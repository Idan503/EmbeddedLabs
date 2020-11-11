#include <xc.h>
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
#include <stdio.h>
#include <stdlib.h>
void busy(void);

void main (void)
{
    int j,i;
    char string[]="Idan Koren-Israeli";
    char control[]={0x38,0x38,0x38,0xe,0x6,0x1};
    TRISBbits.TRISB15 = 0; // RB15 (DISP_RS) set as an output
    ANSELBbits.ANSB15 = 0; // disable analog functionality on RB15 (DISP_RS)
    TRISDbits.TRISD5 = 0; // RD5 (DISP_RW) set as an output
    TRISDbits.TRISD4 = 0; // RD4 (DISP_EN) set as an output
    //TRISEbits.TRISE0 = 1; // RE0 (DB0) set as input (change 1 to 0 for
    TRISE&=0xff00;
    ANSELEbits.ANSE2 = 0;
    ANSELEbits.ANSE4 = 0;
    ANSELEbits.ANSE5 = 0;
    ANSELEbits.ANSE6 = 0;
    PORTBbits.RB15=0;//rs=0
    PORTDbits.RD5=0;//w=0
    ANSELEbits.ANSE7 = 0;

    for(i=0;i<6;i++)
    {
        PORTE=control[i];
        PORTDbits.RD4=1;
        PORTDbits.RD4=0;
        //for(j=0;j<32000;j++);
        busy();
    }
    PORTBbits.RB15=1;//rs=0
    PORTDbits.RD5=0;//w=0
    for(i=0;i<16;i++)
    {
        PORTE=string[i];
        PORTDbits.RD4=1;
        PORTDbits.RD4=0;
        //for(j=0;j<32000;j++);
        busy();
    }
}

void busy(void)
      {
        char RD,RS;
        int STATUS_TRISE;
        int portMap;
        RD=PORTDbits.RD5;
        RS=PORTBbits.RB15;
        STATUS_TRISE=TRISE;
        PORTDbits.RD5 = 1;//w/r
        PORTBbits.RB15 = 0;//rs 
        portMap = TRISE;
        portMap |= 0x80;
        TRISE = portMap;
        do
         {
         PORTDbits.RD4=1;//enable=1
         PORTDbits.RD4=0;//enable=0
         }
       while(PORTEbits.RE7); // BF ?????
       
        PORTDbits.RD5=RD; 
        PORTBbits.RB15=RS;
        TRISE=STATUS_TRISE;   
    }
