


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

void main()
{ int j;
    TRISA &= 0xff00;
   
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
