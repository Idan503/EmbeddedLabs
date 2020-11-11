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
#include <stdio.h>
#include <stdlib.h>

// function declarations
void initLCD();
void init_sw();
void increase(int i);
void symmetric_shift(int by);
void beep();
void init_speaker();
void shift_left(int by);
void busy_wait(int units);
void busyLCD(void);
void sendControlLCD(char control[], int length);
void setLCD(char top_line[], char bottom_line[]);

// delay types
const int SHORT_DELAY = pow(2,17);
const int LONG_DELAY = pow(2,18);

int sw1_place, sw2_left, sw2_right; // state parameters
int last; // for resetting LEDs when functionality changes
int delay; // can be either SHORT or LONG


void main (void)
{
    int j,i;
    char control[]={0x38,0x38,0x38,0xe,0x6,0x1};
    

  
    initLCD();

    sendControlLCD(control,6);
    
    setLCD("Initializing:     ", "     Start          ");


    last = -1; //detecting functionality change
    TRISA &= 0xff00; //Setting lights as output (8 lowest bits)
    init_sw();
    init_speaker();
    
    while(1)
    {
        
        initLCD();
        
        if(PORTBbits.RB9)
        {
            //sw07 - exit
            setLCD("Mode 7:            ", "     Exit          ");
            break;
        }
        
        if(PORTBbits.RB11)
        {
            //sw05 - stop (no functionality)
            setLCD("Mode 5:            ", "     Halt          ");
            continue;
        }
        
        if(PORTBbits.RB10)
        {
            //sw06 - beep sound
            setLCD("Mode 6:            ", "     Beep          ");
            beep();
        }
            
        
        if(PORTDbits.RD14)
        {
            //sw04 - smaller delay
            delay = SHORT_DELAY;
        }
        else
            delay = LONG_DELAY;
        
        busy_wait(delay); 
        
        //priority: sw02>sw01>sw00 by order of code
        if(PORTFbits.RF4)
        {

            //sw02 - symmetric 2-side shift
            if(last!=2){
                //Setting to center - first iteration
                // left and right are separating 8 LEDs to 2x4
                sw2_left = 1; // left 4 LEDs value
                sw2_right = 8; // right 4 LEDs value
                PORTA = (sw2_left<<4) + sw2_right;
                // Setting the LEDs with left shifted by 4
            }
            else{
                if(!PORTDbits.RD15) //sw3 => reverse
                {
                    if(delay==SHORT_DELAY)
                    {
                        setLCD("Mode 2:            ", "   Swing Fast     ");
                    }
                    else
                        setLCD("Mode 2:            ", "   Swing Slow      ");
                        
                    symmetric_shift(1);
                }
                else
                {
                    
                    if(delay==SHORT_DELAY)
                    {
                        setLCD("Mode 2:            ", "Swing Reverse Fast     ");
                    }
                    else
                        setLCD("Mode 2:            ", "Swing Reverse Slow     ");
                    
                    symmetric_shift(-1);
                    
                }
            }
            
            last = 2;
            continue;
        }
        
        
        if(PORTFbits.RF5)
        {
            //sw01 - shift
            if(last!=1){
                sw1_place = 0;
                PORTA = pow(2,sw1_place);
            }
            else{
                if(!PORTDbits.RD15)
                {   
                    if(delay==SHORT_DELAY)
                    {
                        setLCD("Mode 1:            ", "Shift Left Fast     ");
                    }
                    else
                        setLCD("Mode 1:            ", "Shift Left Slow     ");
                    
                    shift_left(1);
                }
                else
                {
                    if(delay==SHORT_DELAY)
                    {
                        setLCD("Mode 1:            ", "Shift Right Fast     ");
                    }
                    else
                        setLCD("Mode 1:            ", "Shift Right Slow     ");
                    
                    
                    
                    shift_left(-1);
                 
                    
                    
                }
            }
            
            
            last = 1;
            continue;
        }
        
        
        if(PORTFbits.RF3)
        {
            //sw00 - count
            if(last!=0){
                PORTA = 0;
            }
            
            if(PORTDbits.RD15)
            {
                if(delay==SHORT_DELAY)
                {
                    setLCD("Mode 0:            ", "Count Down Fast     ");
                }
                else
                    setLCD("Mode 0:            ", "Count Down Slow     ");
                increase(-1);
                
            }
            else
            {
                if(delay==SHORT_DELAY)
                {
                    setLCD("Mode 0:            ", "Count Up Fast     ");
                }
                else
                    setLCD("Mode 0:            ", "Count Up Slow     ");
                increase(1);
            }
            
            last = 0;
            continue;
        }
        
        // No current functionality - setting LEDs to 'off'
        PORTA=0;
        last = -1;
    }   
    
    // end of main loop
    // code gets to here when sw07 is enabled
    PORTA=0;
    exit(0);
    
    
    
}

void increase(int count)
{
    PORTA+=count;
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

void busy_wait(int units)
{
    int i;
    for(i=0;i<units;i++);
}

void init_speaker()
{
    TRISBbits.TRISB14 = 0; //Setting speaker as an output
    ANSELBbits.ANSB14 = 0; // prevent analog (this is a digital output : 0/1)
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

// Waiting for the LCD to be again ready
void busyLCD(void)
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
        
        
        PORTDbits.RD5=0;//back to write mode

    }


void initLCD()
{

    
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

    

    
}

void setLCD(char top_line[], char bottom_line[])
{ 
    int i;
    PORTBbits.RB15=1;//rs=1
    for(i=0;i<16;i++)
    {
        PORTE=top_line[i];
        PORTDbits.RD4=1;
        PORTDbits.RD4=0;
        busyLCD();
    }

    char donw_line[]={0xc0};
    sendControlLCD(donw_line, 1);
        
    PORTBbits.RB15=1;//rs=1
    
    for(i=0;i<16;i++)
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
    int i;
    PORTBbits.RB15=0;//rs=0
    PORTDbits.RD5=0;//w=0
    for(i=0;i<length;i++)
    {
        PORTE=control[i];
        PORTDbits.RD4=1;
        PORTDbits.RD4=0;
        busyLCD();
    }
}