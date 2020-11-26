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
void increase(int i);
void symmetric_shift(int by);
void beep();
void init_speaker();
void shift_left(int by);
void busy_wait(int units);

// delay types
const int SHORT_DELAY = pow(2,17);
const int LONG_DELAY = pow(2,18);

int sw1_place, sw2_left, sw2_right; // state parameters
int last; // for resetting LEDs when functionality changes
int delay; // can be either SHORT or LONG

void main()
{
    last = -1; //detecting functionality change
    TRISA &= 0xff00; //Setting lights as output (8 lowest bits)
    init_sw();
    init_speaker();
    
    while(1)
    {
        if(PORTBbits.RB9)
        {
            //sw07 - exit
            break;
        }
        
        if(PORTBbits.RB11)
        {
            //sw05 - stop (no functionality)
            continue;
        }
        
        if(PORTBbits.RB10)
        {
            //sw06 - beep sound
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
                    symmetric_shift(1);
                }
                else
                    symmetric_shift(-1);
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
                    shift_left(1);
                }
                else
                    shift_left(-1);
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
                increase(-1);
            else
                increase(1);
            
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