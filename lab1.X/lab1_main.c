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
void shift_left(int by);

// delay types
const int SMALL_DELAY = pow(2,18);
const int BIG_DELAY = pow(2,20);

int sw1_place, sw2_left, sw2_right; // state parameters
int last, delay;
int j; //for delay loop

void main()
{
    delay = SMALL_DELAY;
    last = -1; //no last-functionality
    TRISA &= 0xff00; //Setting lights as output (8 lowest bits)
    init_sw();
    
    
    while(1)
    {
        if(PORTBbits.RB11)
        {
            //sw05 - stop (no function)
            continue;
        }
        
        if(PORTDbits.RD14)
        {
            //sw04 - smaller delay
            delay = SMALL_DELAY;
        }
        else
            delay = BIG_DELAY;
        
        
        for(j=0;j<delay;j++); //delay

           
        
             
        if(PORTFbits.RF4)
        {
            //sw02 - symmetric 2-side shift
            if(last!=2){
                //Setting to center - first iteration
                sw2_left = 1;
                sw2_right = 8;
                PORTA = (sw2_left*16) + sw2_right;
            }
            else{
                if(!PORTDbits.RD15)
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

    }   
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
        sw2_right =8;
   
    if(sw2_left == 0)
        sw2_left =8;

    //if(counter == 16) -> counter= 1;
    sw2_left = sw2_left % 15;
    sw2_right = sw2_right %15;
     
    // Setting led as two separated sides
    PORTA = (sw2_left<<4) + sw2_right;
 
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