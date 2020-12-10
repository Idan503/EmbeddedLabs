/* 
 * File:   lcd_fader.c
 * Author: MenachemE
 *
 * Created on July 11, 2019, 5:49 PM
 */

#include <xc.h>
#pragma config JTAGEN =     OFF      
#pragma config FWDTEN =     OFF
#pragma config FNOSC =      FRCPLL
#pragma config FSOSCEN =	OFF
#pragma config POSCMOD =	EC
#pragma config OSCIOFNC =	ON
#pragma config FPBDIV =     DIV_1
#pragma config FPLLIDIV =	DIV_2
#pragma config FPLLMUL =	MUL_20
#pragma config FPLLODIV =	DIV_1
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
void busyLCD(void);
void initSW(void);
void initLCD(void);
void sendControlLCD(char[], int);
void busy_wait(int units);
void printNameAndID(int);
void clearLCD();
void setLCD(char[], char[]);
void beep();
void init_speaker();


int SHIFT_DELAY = pow(2,19);

void main(void) {
    int j, i;
    
    initLCD();
    initSW();
    init_speaker();
    
    // Control statements that will be used later for animation
    char shift_left[] = {0x18};
    char shift_right[] = {0x1c};
    

    int current_line = -1; //line of printing (0 for top line, 1 for bottom line)
    
    while(1)
    {
        busy_wait(SHIFT_DELAY);
        
        if(PORTFbits.RF3)
        {
            //sw00 - change shift side
            
            sendControlLCD(shift_right,1);
        }
        else
            sendControlLCD(shift_left,1);
        
        
        
        if(PORTFbits.RF5)
        {
            //sw01 - top line
            if(current_line!=0)
            {
                printNameAndID(0);
                current_line = 0;
            }
        }
        else{
            //bottom line
            if(current_line!=1)
            {
                printNameAndID(1);
                current_line = 1;
            }
        }
        
               
        if(PORTBbits.RB9)
        {
            //sw07 - beep sound
            beep();
        }
          
        
    }

    
}



// line: 0 for top line, 1 for bottom line
void printNameAndID(int line){
    int i;
    
      //8 * 7
    char CG_name[56] = {
        0x01, 0x05, 0x05, 0x05, 0x05, 0x05, 0x1f, 0x00,
        0x2c, 0x24, 0x24, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x4f, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x40,
        0x6c, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
        0x8e, 0x81, 0x81, 0x85, 0x85, 0x84, 0x84, 0x80,
        0xa6, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa0,
        0xcf, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc0
    };
    
    /*
     All the Hebrew letters
        0x26, 0x29, 0x31, 0x21, 0x29, 0x2b, 0x36, 0x34,
        0x0c, 0x02, 0x02, 0x02, 0x01, 0x03, 0x05, 0x09,
        0x1f, 0x01, 0x01, 0x01, 0x01, 0x09, 0x09, 0x09,
        0x06, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
        0x07, 0x02, 0x02, 0x02, 0x02, 0x02, 0x01, 0x01,
        0x1f, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
        0x08, 0x09, 0x09, 0x0b, 0x09, 0x09, 0x0d, 0x0f,
        0x0f, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x0f,
        0x02, 0x04, 0x0e, 0x01, 0x01, 0x03, 0x01, 0x0c,
        0x08, 0x07, 0x09, 0x09, 0x01, 0x01, 0x01, 0x0f,
        0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x0f,
        0x0f, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0f,
        0x06, 0x09, 0x09, 0x05, 0x01, 0x01, 0x01, 0x0f,
        0x09, 0x0a, 0x0a, 0x08, 0x08, 0x02, 0x01, 0x0f,
        0x0f, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x0e, 0x09, 0x09, 0x0e, 0x08, 0x08, 0x09, 0x0e,
        0x0f, 0x09, 0x09, 0x09, 0x05, 0x05, 0x05, 0x0d}
     * 
     */
    int name_len = 11;
    int id_len = 11;
    char name[] = {-1, 0, 1, 2, 3, -1, 4, 5, 6, 3, -1}; //" Idan Koren" (in Hebrew)
    char id[] = { " 211546288 "};
    char control[] = {0x38, 0x38, 0x38, 0xe, 0x6, 0x1, 0x40,0x0c};
    // set CGRAM=0x40
    sendControlLCD(control, 8);
    
    

    
    PORTBbits.RB15 = 1; //rs
    for (i = 0; i < 56; i++) 
    {
        PORTE = CG_name[i];
        PORTDbits.RD4 = 1; //enable=1
        PORTDbits.RD4 = 0; //enable=0
        busyLCD();
    }
    PORTBbits.RB15 = 0; //rs control 
    PORTE = 0x80; //DDRAM
    PORTDbits.RD4 = 1; //enable=1
    PORTDbits.RD4 = 0; //enable=0
    busyLCD();
    PORTBbits.RB15 = 1; //rs 

       
    if(line==1){
        // down line
        char down_line[]={0xc0};
        sendControlLCD(down_line, 1);
    }
    
    for (i = 0; i < id_len; i++) 
    {
        //Writing ID
        PORTE = id[i];
        PORTDbits.RD4 = 1; //enable=1
        PORTDbits.RD4 = 0; //enable=0
        busyLCD();
    }

    
    for (i = name_len-1; i >= 0 ; i--) 
    {
        // Writing Name
        if(name[i]==-1){
            //Space 
            PORTE = ' ';
        }
        else{
            PORTE = name[i];
        }
        
        PORTDbits.RD4 = 1; //enable=1
        PORTDbits.RD4 = 0; //enable=0
        busyLCD();
    }

}

void clearLCD()
{
    char clear[] = "                        ";
    setLCD(clear, clear);
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


void busy_wait(int units)
{
    int i;
    for(i=0;i<units;i++);
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

void initSW(void)
{
    TRISFbits.TRISF3 = 1; // RF3 (SW0) configured as input
    TRISFbits.TRISF5 = 1; // RF5 (SW1) configured as input
    TRISFbits.TRISF4 = 1; // RF4 (SW2) configured as input
    TRISDbits.TRISD15 = 1; // RD15 (SW3) configured as input
    TRISDbits.TRISD14 = 1; // RD14 (SW4) configured as input
    TRISBbits.TRISB11 = 1; // RB11 (SW5) configured as input
    ANSELBbits.ANSB11 = 0; // RB11 (SW5) disabled analog
    TRISBbits.TRISB10 = 1; // RB10 (SW6) configured as input
    ANSELBbits.ANSB10 = 0; // RB10 (SW6) disabled analog
    TRISBbits.TRISB9 = 1; // RB9 (SW7) configured as input
    ANSELBbits.ANSB9 = 0; // RB9 (SW7) disabled analog
}

void beep(){
    //switching sound on & off fast to make the beep
    int i;
    for(i=0;i<500;i++){
        PORTBbits.RB14 = 1;
        busy_wait(400);
        PORTBbits.RB14 = 0;
        busy_wait(400);
    }
}

void init_speaker()
{
    TRISBbits.TRISB14 = 0; //Setting speaker as an output
    ANSELBbits.ANSB14 = 0; // prevent analog (this is a digital output : 0/1)
}
