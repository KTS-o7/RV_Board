#include <lpc214x.h>
#include <string.h>
#include <stdio.h>
#define PLOCK 0x00000400

// Matrix Columns
#define COL0 (IO1PIN & 1 << 19)
#define COL1 (IO1PIN & 1 << 18)
#define COL2 (IO1PIN & 1 << 17)
#define COL3 (IO1PIN & 1 << 16)

// Switch pins
#define SW2 (IOPIN0 & (1 << 14))
#define SW3 (IOPIN0 & (1 << 15))
#define SW4 (IOPIN1 & (1 << 18))
#define SW5 (IOPIN1 & (1 << 19))
#define SW6 (IOPIN1 & (1 << 20))

//LCD pins
#define RS_ON (IO0SET = 1U << 20)
#define RS_OFF (IO0CLR = 1U << 20)
#define EN_ON (IO1SET = 1U << 25)
#define EN_OFF (IO1CLR = 1U << 25)
int sum;
char itemList[9][19] ={{"1 LED 5"},{"2 JUMPER WIRE 2"},{"3 PROX SENSOR 60"},{"4 DC MOTOR 100"},{"5 JOYSTICK 100"},{"6 SWITCH 5"},{"7 LCD 75"},{"8 RELAY 50"},{"9 PCB 70"}};
char s[12]=" Pay ";
int priceMap(int id)
{
switch(id)
{
	case 1: return 5;
	case 2: return 2;
	case 3: return 60;
	case 4: return 100;
	case 5: return 100;
	case 6: return 5;
	case 7: return 75;
	case 8: return 500;
	case 9: return 70;
	
	default : return 0;
}
}




void SystemInit(void)
{
    PLL0CON = 0x01;
    PLL0CFG = 0x24;
    PLL0FEED = 0xAA;
    PLL0FEED = 0x55;
    while (!(PLL0STAT & PLOCK))
    {
        ;
    }
    PLL0CON = 0x03;
    PLL0FEED = 0xAA; // lock the PLL registers after setting the required PLL
    PLL0FEED = 0x55;
    VPBDIV = 0x01; // PCLK is same as CCLK i.e 60Mhz
}


// The following variables are used in Matrix Key Program and Using matrix keys as selectors
unsigned char rowsel = 0, colsel = 0;

// Lookup table for the Matrix Program
unsigned char lookup_table[4][4] = {{'0', '1', '2', '3'},
                                    {'4', '5', '6', '7'},
                                    {'8', '9', 'a', 'b'},
                                    {'c', 'd', 'e', 'f'}};

void delay_ms(unsigned int j)
{
    unsigned int x, i;
    for (i = 0; i < j; i++)
    {
        for (x = 0; x < 10000; x++)
            ; /* loop to generate 1 milisecond delay with CCLK = 60MHz */
    }
}

unsigned char getKey()
{
    // SystemInit();
    // uart_init();                     // initialize UART0 port
    IO0DIR |= 1U << 31 | 0x00FF0000; // to set P0.16 to P0.23 as o/ps

    do
    {
        while (1)
        {
            // check for keypress in row0,make row0 '0',row1=row2=row3='1'
            rowsel = 0;
            IO0SET = 0X000F0000;
            IO0CLR = 1 << 16;
            if (COL0 == 0)
            {
                colsel = 0;
                break;
            };
            if (COL1 == 0)
            {
                colsel = 1;
                break;
            };
            if (COL2 == 0)
            {
                colsel = 2;
                break;
            };
            if (COL3 == 0)
            {
                colsel = 3;
                break;
            };
            // check for keypress in row1,make row1 '0'
            rowsel = 1;
            IO0SET = 0X000F0000;
            IO0CLR = 1 << 17;
            if (COL0 == 0)
            {
                colsel = 0;
                break;
            };
            if (COL1 == 0)
            {
                colsel = 1;
                break;
            };
            if (COL2 == 0)
            {
                colsel = 2;
                break;
            };
            if (COL3 == 0)
            {
                colsel = 3;
                break;
            };
            // check for keypress in row2,make row2 '0'
            rowsel = 2;
            IO0SET = 0X000F0000;
            IO0CLR = 1 << 18; // make row2 '0'
            if (COL0 == 0)
            {
                colsel = 0;
                break;
            };
            if (COL1 == 0)
            {
                colsel = 1;
                break;
            };
            if (COL2 == 0)
            {
                colsel = 2;
                break;
            };
            if (COL3 == 0)
            {
                colsel = 3;
                break;
            };
            // check for keypress in row3,make row3 '0'
            rowsel = 3;
            IO0SET = 0X000F0000;
            IO0CLR = 1 << 19; // make row3 '0'
            if (COL0 == 0)
            {
                colsel = 0;
                break;
            };
            if (COL1 == 0)
            {
                colsel = 1;
                break;
            };
            if (COL2 == 0)
            {
                colsel = 2;
                break;
            };
            if (COL3 == 0)
            {
                colsel = 3;
                break;
            };
        };
        delay_ms(50); // allow for key debouncing
        while (COL0 == 0 || COL1 == 0 || COL2 == 0 || COL3 == 0)
            ;                                // wait for key release
        delay_ms(50);                        // allow for key debouncing
        IO0SET = 0X000F0000;                 // disable all the rows
        return lookup_table[rowsel][colsel]; // send to serial port(check on the terminal)
    } while (1);
}


// LCD Declarations			
static void delay_us(unsigned int count); // microsecond delay
static void LCD_SendCmdSignals(void);
static void LCD_SendDataSignals(void);
static void LCD_SendHigherNibble(unsigned char dataByte);
static void LCD_CmdWrite(unsigned char cmdByte);
static void LCD_DataWrite(unsigned char dataByte);
static void LCD_Reset(void);
static void LCD_Init(void);
void LCD_DisplayString(const char *ptr_stringPointer_u8);


static void LCD_CmdWrite(unsigned char cmdByte)
{
    LCD_SendHigherNibble(cmdByte);
    LCD_SendCmdSignals();
    cmdByte = cmdByte << 4;
    LCD_SendHigherNibble(cmdByte);
    LCD_SendCmdSignals();
}
static void LCD_DataWrite(unsigned char dataByte)
{
    LCD_SendHigherNibble(dataByte);
    LCD_SendDataSignals();
    dataByte = dataByte << 4;
    LCD_SendHigherNibble(dataByte);
    LCD_SendDataSignals();
}
static void LCD_Reset(void)
{
    /* LCD reset sequence for 4-bit mode*/
    LCD_SendHigherNibble(0x30);
    LCD_SendCmdSignals();
    delay_ms(100);
    LCD_SendHigherNibble(0x30);
    LCD_SendCmdSignals();
    delay_us(200);
    LCD_SendHigherNibble(0x30);
    LCD_SendCmdSignals();
    delay_us(200);
    LCD_SendHigherNibble(0x20);
    LCD_SendCmdSignals();
    delay_us(200);
}
static void LCD_SendHigherNibble(unsigned char dataByte)
{
    // send the D7,6,5,D4(uppernibble) to P0.16 to P0.19
    IO0CLR = 0X000F0000;
    IO0SET = ((dataByte >> 4) & 0x0f) << 16;
}
static void LCD_SendCmdSignals(void)
{
    RS_OFF; // RS - 1
    EN_ON;
    delay_us(100);
    EN_OFF; // EN - 1 then 0
}

static void LCD_SendDataSignals(void)
{
    RS_ON; // RS - 1
    EN_ON;
    delay_us(100);
    EN_OFF; // EN - 1 then 0
}
static void LCD_Init(void)
{
    delay_ms(100);
    LCD_Reset();
    LCD_CmdWrite(0x28u); // Initialize the LCD for 4-bit 5x7 matrix type
    LCD_CmdWrite(0x0Eu); // Display ON cursor ON
    LCD_CmdWrite(0x01u); // Clear the LCD
    LCD_CmdWrite(0x80u); // go to First line First Position
}

void LCD_DisplayString(const char *ptr_string)
{
    // Loop through the string and display char by char
    while ((*ptr_string) != 0)
        LCD_DataWrite(*ptr_string++);
}
static void delay_us(unsigned int count)
{
    unsigned int j = 0, i = 0;
    for (j = 0; j < count; j++)
    {
        for (i = 0; i < 10; i++)
            ;
    }
}

void lcdClear()
{
	  //SystemInit();
    IO0DIR |= 1U << 31 | 0x00FF0000; // to set P0.16 to P0.23 as o/ps
    IO1DIR |= 1U << 25;              // to set P1.25 as o/p used for EN
   
    LCD_Reset();
    LCD_Init();
    delay_ms(10);
}

void lcdTest(char *buf)
{
    //SystemInit();
    IO0DIR |= 1U << 31 | 0x00FF0000; // to set P0.16 to P0.23 as o/ps
    IO1DIR |= 1U << 25;              // to set P1.25 as o/p used for EN
    // make D7 Led on off for testing
    
   
    
    LCD_Reset();
    LCD_Init();
    delay_ms(100);
    LCD_CmdWrite(0x80);
    LCD_DisplayString(buf);
    LCD_CmdWrite(0xc0);
  
}
void resetSum()
{
	sum =0;
}

void calculate(int id)
{
	sum+=priceMap(id);
}
void runDCMotor(int dutycycle)
{
	IO0DIR |= 1U << 28; //set P0.28 as output pin
	PINSEL0 |= 2 << 18; //select P0.9 as PWM6 (option 2)
	IO0CLR = 1 << 28; //set to 0, to choose clockwise direction
	PWMPCR = (1 << 14); // enable PWM6
	PWMMR0 = 1000; // set PULSE rate to value suitable for DC Motor operation
	PWMMR6 = (1000U*dutycycle)/100; // set PULSE period
	PWMTCR = 0x00000009; // bit D3 = 1 (enable PWM), bit D0=1 (start the timer)
	PWMLER = 0X70; // load the new values to PWMMR0 and PWMMR6 registers
}
int readSensor(int sen_no)
{
int result=0;
IO1DIR |= 1 << 24;
IO1CLR = 1<< 24; // enable sensor logic: P1.24 - 0
switch (sen_no)
{
case 1: result = IO1PIN & (1<<22); //P1.22 connected to sensor1
break;
case 2: result = IO1PIN & (1<<23); //P1.23 connected to sensor2
break;
default: result = 0;
};
IO1SET = 1<< 24; // disable sensor logic: P1.24
return result;
}

void start_motor(){
	int dig_val;
	IO0DIR |= 1U << 31 | 0x00FF0000 | 1U << 30; // to set P0.16 to P0.23 as o/ps
 
	
	do{
		// dig_val = adc(1,2) / 10;
		// if(dig_val > 100) dig_val =100;
			runDCMotor(50); // run at 50% duty cycle
		// 

		
	}while(1);
}




void delay(void)
{
	T0MR0 = 15000; //use the Timer0 and load the MR0 with count
	T0MCR = 0X0004;
	T0TCR = 1; //start the timer
	while (!(T0TC == T0MR0));
	T0TCR = 2; // reset the counter and stop the timer
}
void delay_nms(int no){
	for (int i = 0; i<no; i++) delay();
}
void delay_ns(int no){
	for (int i = 0; i<no*1000; i++) delay();
}


void beep()
{
	//P0.25 used as AOUT - DAC output
	//val - 0 to 1023 : 10 bit DAC, P0.25 used as AOUT generates analog output
	PINSEL1 |= 0x00080000;
	DACR = ( (1<<16) | (0<<6) );
	delay_ns(1);
	DACR = ( (1<<16) | (1023<<6) );
	
}



unsigned char getAlphaCode(unsigned char alphachar)
{
    switch (alphachar)
    {
    // dp g f e d c b a - common anode: 0 segment on, 1 segment off
    case '0':
        return 0xC0;
    case '1':
        return 0xf9;
    case '2':
        return 0xA4;
    case '3':
        return 0xB0; // 1000 0110
    case '4':
        return 0x99;
    case '5':
        return 0x92;
    case '6':
        return 0x82;
    case '7':
        return 0xf8;
		case '8':
        return 0x80;
    case '9':
        return 0x90;
    
    default:
        return 0xff;
    }
    
}

void alphadisp7SEG(char*buf)
{
    unsigned char i, j;
    unsigned char seg7_data, temp = 0;
    for (i = 0; i < 5; i++) // because only 5 seven segment digits are present
    {
        seg7_data = getAlphaCode(*(buf + i)); // instead of this look up table can be used
        // to shift the segment data(8bits)to the hardware (shift registers) using Data,Clock,Strobe
        for (j = 0; j < 8; j++)
        {
            // get one bit of data for serial sending
            temp = seg7_data & 0x80; // shift data from Most significan bit (D7)
            if (temp == 0x80)
                IOSET0 |= 1 << 19; // IOSET0 | 0x00080000;
            else
                IOCLR0 |= 1 << 19; // IOCLR0 | 0x00080000;
            // send one clock pulse
            IOSET0 |= 1 << 20; // IOSET0 | 0x00100000;
            delay();
            IOCLR0 |= 1 << 20;          // IOCLR0 | 0x00100000;
            seg7_data = seg7_data << 1; // get next bit into D7 position
        }
    }
    IOSET0 |= 1 << 30; // IOSET0 | 0x40000000;
    delay();       // nop();
    IOCLR0 |= 1 << 30; // IOCLR0 | 0x40000000;
    return;
}


int main()
{
	//while(readSensor(2));
	IO0DIR = 1U << 19 | 1U << 20 | 1U << 30; //make P1.16 as output
	int i = 0,counter =0,j;
	// SystemInit();
	lcdTest("Request initiating...");

		delay_ms(1000);
	lcdTest("WELCOME TO  KIOSK");
	for(j=0;j<20;j++){LCD_CmdWrite(0x18);delay_ms(200);}
	// Initialization sequence
	while(1)
	{
		if(getKey() == 'f')
			break;
	}
	resetSum();
	while(1)
	{
		char Var;
		for(i=0;i<9;i++)
	{
		//lcdClear();
		lcdTest(itemList[i]);
		delay_ms(250);
		Var = getKey();
		
		if(Var == ' ' )
		{
			continue;
		}
		else if(Var == '1'){
		lcdTest("Item added");
			
		calculate(i+1);
			counter++;
			if(counter == 5)
			break;
		}
		else if(Var=='e') break;
		
	
	}
	if(counter == 5 || Var=='e')
			break;
	}
	
	char str[4];
	sprintf(str, "%d", sum);
	strcat(s,str);
	lcdTest(s);
	delay_ns(2);
	
	int ia = 5;
	char buf[5];
	 // 0000....100 – Stop the timer, after match	
	lcdTest("Processing........");
	while (ia > 0){
		sprintf(buf, "%5d", ia);
		alphadisp7SEG(buf);
		delay_ns(1);
		ia--;	
	}
	alphadisp7SEG("     ");
	beep();
	delay_nms(100);
	
	lcdTest("Please make         the payment");
	while(1)
	{
		if(getKey() == 'd'){
			lcdTest("Paid");
			break;
		}
			
	}
	delay_ms(1000);
	
	

	lcdTest(" Thank you");
	lcdTest("Visit again");
	
	start_motor();
	
	while(1);
	
return 0;
}
