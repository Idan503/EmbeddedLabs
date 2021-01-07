
#include <xc.h>
#include <string.h>
#include <sys/attribs.h>
#include "ssd.h"
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

void SSD_DigitsNoDot(int a, int b, int c, int d);
void init_btn();
void busy_delay();
void start_clock();
void on_left_press();
void on_right_press();
void on_up_press();
void on_down_press();
void on_center_press();
void init_digits();
int is_clock_over();
void update_digits();
void clock_tick();

int m_clock_time;

int m_digits[4];
int current_selection = 0; // 0 leftmost, 3 rightmost



void main()
{
    init_digits();
    init_btn();
    TRISA &= 0xff00;
    SSD_Init();
    
    //SSD_DigitsNoDot(digits[0],digits[1],digits[2],digits[3]);
    update_digits();
    int changed; //flag
    while(1)
    {
        changed = 0;
        
        busy_delay();
        if(PORTBbits.RB1) // Up
        {
            on_up_press();
            changed = 1;
        }
        busy_delay();
        if(PORTBbits.RB0){ // Left
            on_left_press();
            changed = 1;
        }
        busy_delay();
        if(PORTFbits.RF0){ // Center
            on_center_press();
            //changed = 1;
        }
        busy_delay();
        if(PORTBbits.RB8){ // Right
            on_right_press();
            changed = 1;
        }
        busy_delay();
        if(PORTAbits.RA15){ // Down
            on_down_press();
            changed = 1;
        }
        busy_delay();
        
        
        
        if(changed)
            update_digits();
        
    }

}




void start_clock()
{
    m_clock_time = (m_digits[0] * 1000) + (m_digits[1] * 100)
            + (m_digits[2] * 10) + m_digits[3];
            
    while(m_clock_time != 0){
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
        PR2=0XC4B3;//PR2x
        PR3=0x0004;//PR2y
        T2CONbits.ON=1;//start timer
        IFS0bits.T2IF=0;
        IFS0bits.T3IF=0;//
        while(!IFS0bits.T3IF);//timer stop
        clock_tick();
    }
    
}

void clock_tick()
{
    
    m_clock_time--;
    
    m_digits[0] = m_clock_time / 1000;
    m_digits[1] = (m_clock_time % 1000) / 100;
    m_digits[2] = (m_clock_time % 100) / 10;
    m_digits[3] = (m_clock_time % 10);
    update_digits();
}




void update_digits()
{
    SSD_DigitsNoDot(m_digits[0],m_digits[1],m_digits[2],m_digits[3]);
}

void on_left_press()
{
    current_selection--;
    if(current_selection<0)
        current_selection = 3;
}


void on_right_press()
{
    current_selection++;
    if(current_selection>3)
        current_selection = 0;
}


void on_up_press()
{
    m_digits[current_selection]++;
    if(m_digits[current_selection] > 9)
        m_digits[current_selection] = 0;
}


void on_down_press()
{
    m_digits[current_selection]--;
    if(m_digits[current_selection] < 0)
        m_digits[current_selection] = 9;
   
}

void on_center_press()
{
    start_clock();
}

void init_digits()
{
    m_digits[0] = 0;
    m_digits[1] = 0;
    m_digits[2] = 0;
    m_digits[3] = 0;
}

void SSD_DigitsNoDot(int a, int b, int c, int d)
{
    SSD_WriteDigits(d,c,b,a,0,0,0,0);
}

void busy_delay()
{
    int i;
    for(i=0;i<64000;i++);
}



void init_btn()
{
    TRISBbits.TRISB1 = 1; // RB1 (BTNU) configured as input
    ANSELBbits.ANSB1 = 0; // RB1 (BTNU) disabled analog
    TRISBbits.TRISB0 = 1; // RB0 (BTNL) configured as input
    ANSELBbits.ANSB0 = 0; // RB0 (BTNL) disabled analog
    TRISFbits.TRISF0 = 1; // RF0 (BTNC) configured as input
    TRISBbits.TRISB8 = 1; // RB8 (BTNR) configured as input
    ANSELBbits.ANSB8 = 0; // RB8 (BTNR) disabled analog
    TRISAbits.TRISA15 = 1; // RA15 (BTND) configured as input 

}

