#include <xc.h>
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
// function declarations
void init_sw();
void busy_wait(int units);
void update_center();
void set_leds(char bits);

int center;
int left_side, right_side;
int j; //for delay loop

void main()
{
    init_sw();
    TRISA &= 0xff00;
   
    
    
    while(1)
    {
        update_center();
        if(center == -1){
            // No center -> no animation
            set_leds(0);
            continue;
        }
        
        set_leds((char)pow(2,center));
        // turning on the center led
        
        for(j=0;j<8;j++){
            busy_wait(200000);

            left_side++;
            right_side--;
            
            if(left_side >= 8 && right_side <= -1)
                break; // ended animation
            
            
            char light_num = 0;
            if(center < 8 && center > -1)
                light_num |= (char) pow(2,center);
            if(left_side<8)
                light_num |= (char) pow(2,left_side);
            if(right_side>-1)
                light_num |= (char) pow(2,right_side);
            
            set_leds(light_num);
            // turning on wing leds
            
        }      
    }
     
}

void busy_wait(int units)
{
    int i;
    for(i=0;i<units;i++);
}

void update_center()
{
    if(PORTFbits.RF3)
        center = 0;
    else if(PORTFbits.RF5)
        center = 1;
    else if(PORTFbits.RF4)
        center = 2;
    else if(PORTDbits.RD15)
        center = 3;
    else if(PORTDbits.RD14)
        center = 4;
    else if(PORTBbits.RB11)
        center = 5;
    else if(PORTBbits.RB10)
        center = 6;
    else if(PORTBbits.RB9)
        center = 7;
    else
        center = -1;
    
    left_side = center;
    right_side = center;
        
}

void set_leds(char bits)
{
    PORTAbits.RA7 = (bits & ( 1 << 7 )) >> 7;
    PORTAbits.RA6 = (bits & ( 1 << 6 )) >> 6;
    PORTAbits.RA5 = (bits & ( 1 << 5 )) >> 5;
    PORTAbits.RA4 = (bits & ( 1 << 4 )) >> 4;
    PORTAbits.RA3 = (bits & ( 1 << 3 )) >> 3;
    PORTAbits.RA2 = (bits & ( 1 << 2 )) >> 2;
    PORTAbits.RA1 = (bits & ( 1 << 1 )) >> 1;
    PORTAbits.RA0 = bits;
}

void init_sw()
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