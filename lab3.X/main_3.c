/* 
 * File:   key_xy.c
 * Author: MenachemE
 *
 * Created on July 25, 2019, 4:08 PM
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <xc.h>
#include <sys/attribs.h>
#include <string.h>
#include <p32xxxx.h>
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

void initLCD(void);
void busyLCD(void);
void sendControlLCD(char control[], int length);
void setLCD(char top_line[], char bottom_line[]);

void main(void)
{
    static int i;
    TRISA&=0xff00;//led
    TRISCbits.TRISC2=0;//RC2
    TRISCbits.TRISC1=0;//RC1
    TRISCbits.TRISC4=0;//RC4            
    TRISGbits.TRISG6=0;//RG6
    ANSELGbits.ANSG6=0;//???????
    TRISCbits.TRISC3 =1;//RC3
    CNPUCbits.CNPUC3;               
    TRISGbits.TRISG7=1;//RG7
    ANSELGbits.ANSG7=0;
    CNPUGbits.CNPUG7;                  
    TRISGbits.TRISG8=1;//RG8
    ANSELGbits.ANSG8=0;//???????
    CNPUGbits.CNPUG8;//?????
    TRISGbits.TRISG9=1;//RG9
    ANSELGbits.ANSG9=0;//???????
    CNPUGbits.CNPUG9;//????? 

    
    initLCD();
    char control[]={0x38,0x38,0x38,0xe,0x6,0x1};
    char msg[16];
    sendControlLCD(control, 6);
    
    
    while(1)
    {
        int pressed = scan_key();
        
        if(pressed == 0xff)
            continue;
                
        sprintf(msg, "    Mode %c          ", pressed);
        setLCD("                   ", msg);
        busyLCD();
        //PORTC=0; - IDK
        PORTCbits.RC2=0;
        PORTCbits.RC1=0;
        PORTCbits.RC4=0;
        PORTGbits.RG6=0;
        while(!PORTCbits.RC3);
        while(!PORTGbits.RG7);
        while(!PORTGbits.RG8);
        while(!PORTGbits.RG9);
        // Waiting for one of the keys to be released
    }
}



int scan_key()
{
    int i=1;
    char scan_key[]={0x44,'1',0x34,'2',0x24,'3',0x43,'4',0x33,'5',0x23,'6',
                     0x42,'7',0x32,'8',0x22,'9',0x41,'0'}; //Dictionary of keys to ascii
    int xy,counter;
    for(counter=0;counter<1000;counter++)
    {
        // A single scan for xy press keyboard location
        PORTCbits.RC2=1;
        PORTCbits.RC1=1;
        PORTCbits.RC4=1;
        PORTGbits.RG6=1;
        PORTCbits.RC2=0;
        xy=in_y(1);
        if (xy!=0xff)
           break;
        PORTCbits.RC2=1;
        PORTCbits.RC1=0;
        xy=in_y(2);
        if (xy!=0xff)
           break;
        PORTCbits.RC1=1;
        PORTCbits.RC4=0;
        xy=in_y(3);
        if (xy!=0xff)
           break;
        PORTCbits.RC4=1;
        PORTGbits.RG6=0;
        xy=in_y(4);
        if (xy!=0xff)
           break;
        PORTGbits.RG6=1;
    }
    if(xy==0xff) // no key press detected
        return(0xff);

    PORTBbits.RB15=1;//rs=1 data (ascii)
    for(i=0;i<20;i+=2)
        if(scan_key[i]==xy){
        
            return scan_key[i+1];
        
        }
    return(0xff);
}



int in_y( int a)
{ 
    int j=1,flag=0;
    if(!PORTCbits.RC3)
    { 
        flag=1;
        j=1;
    }
   else if (!PORTGbits.RG7)
   { 
       flag=1;
        j=2;
   }
   else  if(!PORTGbits.RG8)
    { 
       flag=1;
        j=3;
    }
   else if(!PORTGbits.RG9)
    { 
       flag=1; 
        j=4;
    }
    if(flag==0)
        return(0xff);
    else
       return(j|(a<<4));

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

    char donw_line[]={0xc0};
    sendControlLCD(donw_line, 1);
            
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