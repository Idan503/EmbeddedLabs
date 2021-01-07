
// This is lab 6
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#pragma config JTAGEN = OFF     
#pragma config FWDTEN = OFF
#pragma config FNOSC =	FRCPLL
#pragma config FSOSCEN =OFF
#pragma config POSCMOD =EC
#pragma config OSCIOFNC =ON
#pragma config FPBDIV =     DIV_1
#pragma config FPLLIDIV =DIV_2
#pragma config FPLLMUL =MUL_20
#pragma config FPLLODIV =DIV_1

void initLCD();
void setLCD(char top_line[], char bottom_line[]);
void busyLCD(void);
void sendControlLCD(char control[], int length);
void init_keyboard();
void beep();
int scan_key();
int in_y();
void busy_wait(int units);
void init_speaker();

char count_msg[16];
int analog_num = 1;

void initADC()
{

    AD1CON1	=0; 
    AD1CON1bits.SSRC = 7;   // Internal counter ends sampling and starts conversion (auto convert)
    AD1CON1bits.FORM = 0;   // Integer 16-bit
	// Setup for manual sampling
    AD1CSSL	=0;
    AD1CON3	=0x0002;     // ADC Conversion Clock Select bits: TAD = 6 TPB
    AD1CON2	=0;
    AD1CON2bits.VCFG = 0;   // Voltage Reference Configuration bits: VREFH = AVDD and VREFL = AVSS
	// Turn on ADC
    AD1CON1bits.ON = 1;
} 

// ************************************************************************** 
//***	ADC_AnalogRead
//**
//**	Parameters:
//**		unsigned char analogPIN - the number of the analog pin that must be read
//**
//**	Return Value:
//**		- the 16 LSB bits contain the result of analog to digital conversion of the analog value of the specified pin
//**
//**	Description:
//**		This function returns the digital value corresponding to the analog pin, 
//**      as the result of analog to digital conversion performed by the ADC module.         

unsigned int ADC_AnalogRead(unsigned char analogPIN)
{
    int adc_val = 0;
    
    IEC0bits.T2IE = 0;
    AD1CHS = analogPIN << 16;       // AD1CHS<16:19> controls which analog pin goes to the ADC
 
    AD1CON1bits.SAMP = 1;           // Begin sampling
    while( AD1CON1bits.SAMP );      // wait until acquisition is done
    while( ! AD1CON1bits.DONE );    // wait until conversion is done
 
    adc_val = ADC1BUF0;
    IEC0bits.T2IE = 1;
    return adc_val;
}


void main(void)
{   
     TRISBbits.TRISB2 = 1;
     ANSELBbits.ANSB2 = 1;
     TRISBbits.TRISB4 = 1; // init mic
     ANSELBbits.ANSB4 = 1; // init mic

     init_keyboard();
     initLCD();
     initADC();
     init_speaker();
     
     int last_pressed;

     
     while(1)
     {  
        busy_wait(2000);
        int pressed = scan_key();
        
        if(pressed != 0xff){
            beep();
            last_pressed = pressed;
        }
        
            
        if(last_pressed == '1' || last_pressed == '2')
        {
            int adc_result;
            if(last_pressed == '1')
                adc_result = ADC_AnalogRead(2);//IN analog RB2
            else
                adc_result = ADC_AnalogRead(4);//IN analog RB2
            
        
            float result = (double)adc_result / 310;
            sprintf(count_msg, "Analog %d - %0.2fV       ", analog_num,result);
            setLCD(count_msg,"                       ");
         }
        else
           setLCD("Please Press 1/2","                     ");
     }
}









//LCD:

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

    char down_line[]={0xc0};
    sendControlLCD(down_line, 1);
            
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
    
    char control[]={0x38,0x38,0x38,0xe,0x6,0x1,0xc};
    sendControlLCD(control, 7);
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
