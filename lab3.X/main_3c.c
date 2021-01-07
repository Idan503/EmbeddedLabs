/* 
 * File:   Lab 3
 * Author: Idan Koren-Israeli
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
#include <math.h>
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
void increase(int count);
void busyLCD(void);
void sendControlLCD(char control[], int length);
void setLCD(char top_line[], char bottom_line[]);
void init_speaker(void);
void busy_wait(int units);
void init_keyboard();
void beep();
int scan_key();
void shift_left(int by);
void symmetric_shift(int by);

void timer_delay(void);

const int SHORT_DELAY = pow(2,16);
const int LONG_DELAY = pow(2,18);

int sw1_place, sw2_left, sw2_right; // state parameters
int delay; // can be either SHORT or LONG
int reverse = 1;

char mode_msg[16];


void main(void)
{
    static int i;
    TRISA&=0xff00;//led turn off

    delay = SHORT_DELAY;
    init_keyboard();
    init_speaker();
    initLCD();
    PORTA = 0x00;
    char control[]={0x38,0x38,0x38,0xe,0x6,0x1};
    
    sendControlLCD(control, 6);
    int finish = 0;
    int old_pressed = '9';
    int last_pressed = '9';
    int pressed = '9';
    sprintf(mode_msg, "     Mode %c          ", pressed);
    
    while(finish!=1)
    {
        busy_wait(delay); 
        pressed = scan_key();
        
        if(pressed != 0xff){
            old_pressed = last_pressed;
            last_pressed = pressed;
            if(pressed!='4' && pressed !='3') //saved for reverse and speed
                sprintf(mode_msg, "     Mode %c          ", pressed);
            beep();
        }
          
        switch(last_pressed)
        {
            case '7':
                setLCD("     Exit          ", mode_msg);
                finish = 1;
                break;
            case '5':
                setLCD("     Stop          ", mode_msg);
                old_pressed = '5';
                break;
            case '6':
                setLCD("     Beep          ", mode_msg);
                PORTA = 0x00;
                beep();
                old_pressed = '6';
                break;
            case '4':
                if(delay==SHORT_DELAY)
                    delay = LONG_DELAY;
                else
                    delay = SHORT_DELAY;
                last_pressed = old_pressed; // back to the last mode
                break;
            case '3':
                reverse = !reverse;
                last_pressed = old_pressed; // back to the last mode
                break;
            case '2':
                //symmetric 2-side shift
                if(old_pressed!='2'){
                    //Setting to center - first iteration
                    // left and right are separating 8 LEDs to 2x4
                    sw2_left = 1; // left 4 LEDs value
                    sw2_right = 8; // right 4 LEDs value
                    PORTA = (sw2_left<<4) + sw2_right;
                    // Setting the LEDs with left shifted by 4
                }
                else{
                    if(reverse) //sw3 => reverse
                    {
                        if(delay==SHORT_DELAY)
                        {
                            setLCD("   Swing Fast      ", mode_msg);
                        }
                        else
                            setLCD("   Swing Slow      ", mode_msg);

                        symmetric_shift(1);
                    }
                    else
                    {
                        if(delay==SHORT_DELAY)
                        {
                            setLCD("Swing Rev. Fast     ", mode_msg);
                        }
                        else
                            setLCD("Swing Rev. Slow     ", mode_msg);
                        symmetric_shift(-1);
                    }
                }
                
                old_pressed = '2';
                break;
            case '1':
                //1 - shift
                if(old_pressed!='1'){
                    sw1_place = 0;
                    PORTA = pow(2,sw1_place);
                }
                else{
                    if(reverse)
                    {   
                        if(delay==SHORT_DELAY)
                        {
                            setLCD("Shift Left Fast     ", mode_msg);
                        }
                        else
                            setLCD( "Shift Left Slow     ", mode_msg);

                        shift_left(1);

                    }
                    else
                    {
                        if(delay==SHORT_DELAY)
                        {
                            setLCD("Shift Right Fast     ", mode_msg );
                        }
                        else
                            setLCD( "Shift Right Slow     ", mode_msg);



                        shift_left(-1);
                    }
                }
                
                old_pressed = '1';
                break;
            case '0':
                //0 - count
                if(old_pressed!='0'){
                    PORTA = 0;
                }

                if(reverse)
                {
                    if(delay==SHORT_DELAY)
                    {
                        setLCD("Count Down Fast     ", mode_msg);
                    }
                    else
                        setLCD("Count Down Slow     ", mode_msg);
                    increase(-1);

                }
                else
                {
                    if(delay==SHORT_DELAY)
                    {
                        setLCD("Count Up Fast     ", mode_msg);
                    }
                    else
                        setLCD("Count Up Slow     ",mode_msg );
                    increase(1);
                }
            
                old_pressed = '0';
                
                break;
            case '9':
                setLCD("     None          ", mode_msg);
                break;
            
        }
        
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
    //clean
        setLCD("                   ", "                  ");
        PORTA=0;
        exit(0);
}

void shift_left(int by)
{
    if(by==0)
        return; // no move
    
    sw1_place += by;
    
    // jump to other side when gets to edge
    if(sw1_place > 7)
    {
        sw1_place = 0;
    }
    else if(sw1_place < 0)
    {
        sw1_place = 7;
    }
    
    PORTA = pow(2,sw1_place); //Showing single led as shifted
}

void timer_delay()
{
    PR4 = 0xafff;                        //          setting time of delay
    TMR4 = 0;                           //             initialize count to 0

    T4CONbits.TCKPS0 = 1;                //            1:256 prescale value
    T4CONbits.TCKPS1 = 1; 
    T4CONbits.TCKPS2 = 1; 
    T4CONbits.TGATE = 0;                //             not gated input (the default)
    T4CONbits.TCS = 0;                  //             PCBLK input (the default)
    T4CONbits.ON = 1;                   //             turn on Timer4
    IPC4bits.T4IP = 2;                  //             priority
    IPC4bits.T4IS = 0;                  //             subpriority
    IFS0bits.T4IF = 0;     
}




int scan_key()
{
    int i=1;
    char scan_key[]={0x44,'1',0x34,'2',0x24,'3',0x43,'4',0x33,'5',0x23,'6',
                     0x42,'7',0x32,'8',0x22,'9',0x41,'0'}; //Dictionary of keys to ascii
    int xy;
    
    timer_delay();
    while(!IFS0bits.T4IF)// Waiting for timer 4
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
    int j=0;
    if(!PORTCbits.RC3)
    { 
        j=1;
    }
   else if (!PORTGbits.RG7)
   { 
        j=2;
   }
   else  if(!PORTGbits.RG8)
    { 
        j=3;
    }
   else if(!PORTGbits.RG9)
    { 
        j=4;
    }
    if(j==0) //No press detected
        return(0xff);
    else
       return(j|(a<<4)); //Formatting a press by x(a) and y(j)

}



void increase(int count)
{
    PORTA+=count;
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
    
    char control[]={0x38,0x38,0x38,0xe,0x6,0x1};
    sendControlLCD(control, 6);
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

void init_speaker()
{
    TRISBbits.TRISB14 = 0; //Setting speaker as an output
    ANSELBbits.ANSB14 = 0; // prevent analog (this is a digital output : 0/1)
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


void init_keyboard()
{
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
}

void busy_wait(int units)
{
    int i;
    for(i=0;i<units;i++);
}

void symmetric_shift(int by)
{
    if(by==0)
        return;
    
    if(by > 0)
    {
        sw2_left = sw2_left<<by;
        sw2_right = sw2_right>>by;
    }
    else if(by < 0)
    {
        sw2_left = sw2_left>>-by;
        sw2_right = sw2_right<<-by;
    }

    
    // Restarts when gets to edge
    
    if(sw2_right == 0)
        sw2_right = 8;
   
    if(sw2_left == 0)
        sw2_left = 8;

    //if(counter == 16) -> counter= 1;
    sw2_left = sw2_left % 15;
    sw2_right = sw2_right %15;
     
    // Setting led as two separated sides
    PORTA = (sw2_left<<4) + sw2_right;
 
}
