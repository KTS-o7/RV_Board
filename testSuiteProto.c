#include <lpc214x.h>
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

// The following variables are used in Up-Down and Ring Counter program
int contUP = 0;
int contDN = 99;
unsigned int rightSFT = 1U << 7;
unsigned int leftSFT = 1;

// The following variables are used in Matrix Key Program and Using matrix keys as selectors
unsigned char rowsel = 0, colsel = 0;

// Lookup table for the Matrix Program
unsigned char lookup_table[4][4] = {{'0', '1', '2', '3'},
                                    {'4', '5', '6', '7'},
                                    {'8', '9', 'a', 'b'},
                                    {'c', 'd', 'e', 'f'}};

																		
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
void delay_ms(unsigned int j)
{
    unsigned int x, i;
    for (i = 0; i < j; i++)
    {
        for (x = 0; x < 10000; x++)
            ; /* loop to generate 1 milisecond delay with CCLK = 60MHz */
    }
}

unsigned char programControl()
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

void reset_values(int y)
{
    switch (y)
    {
    case 0:
        contDN = 99;
        rightSFT = 1U << 7;
        leftSFT = 1;
        break;
    case 1:
        contUP = 0;
        rightSFT = 1U << 7;
        leftSFT = 1;
        break;
    case 2:
        contUP = 0;
        contDN = 99;
        rightSFT = 1U << 7;
        break;
    case 3:
        contUP = 0;
        contDN = 99;
        leftSFT = 1;
        break;
    }
}

unsigned char getAlphaCode(unsigned char alphachar)
{
    switch (alphachar)
    {
    // dp g f e d c b a - common anode: 0 segment on, 1 segment off
    case '8':
        return 0x00; // Switches on all the segments of one display along with decimal point.
    case ' ':
        return 0xff;
    // simmilarly add for other digit/characters
    default:
        break;
    }
    return 0xff;
}

void alphadisp7SEG(char *buf)
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
            delay_ms(1);
            IOCLR0 |= 1 << 20;          // IOCLR0 | 0x00100000;
            seg7_data = seg7_data << 1; // get next bit into D7 position
        }
				 // Strobe Signal
    IOSET0 |= 1 << 30; // IOSET0 | 0x40000000;
    delay_ms(1);       // nop();
    IOCLR0 |= 1 << 30; // IOCLR0 | 0x40000000;
			delay_ms(200);
    }
   
    return;
}

// System Initialization code

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

// Has updown and binary counter
int counterProgram()
{
    IO0DIR = 0xFF << 16;
    IO0SET = 0xFF << 16;

    while (1)
    {
        if (!SW2)
        {
            reset_values(0);
            IO0SET = 0xFF << 16;
            IO0CLR = ((contUP / 10) << 4 | contUP % 10) << 16;
            contUP++;
            if (contUP > 99)
                contUP = 0;
        }
        else if (!SW3)
        {
            reset_values(1);
            IO0SET = 0xFF << 16;
            IO0CLR = ((contDN / 10) << 4 | contDN % 10) << 16;
            contDN--;
            if (contDN < 0)
                contDN = 99;
        }
        else if (!SW4)
        {
            reset_values(2);
            IO0SET = 0xFF << 16;
            IO0CLR = leftSFT << 16;
            leftSFT <<= 1;
            if (leftSFT > 0x01 << 7)
                leftSFT = 0x01;
        }
        else if (!SW5)
        {
            reset_values(3);
            IO0SET = 0xFF << 16;
            IO0CLR = rightSFT << 16;
            rightSFT >>= 1;
            if (rightSFT < 0x01)
                rightSFT = 0x01 << 7;
        }
        if (!SW6)
            break;
        delay_ms(200);
    }
    return 0;
}

// Has seven segment display test code
void sevenSegDisplay()
{
    IO0DIR |= 1U << 31 | 1U << 19 | 1U << 20 | 1U << 30; // to set as o/ps

    while (1)
    {
        alphadisp7SEG("88888");
        delay_ms(300);
        alphadisp7SEG("     ");
        delay_ms(300);
        if (!SW6)
            break;
    }
}

void stepperMotorTest()
{
    unsigned int no_of_steps_clk = 200, no_of_steps_aclk = 200;
    IO0DIR |= 1U << 31 | 0x00FF0000 | 1U << 30; // to set P0.16 to P0.23 as o/ps

    SystemInit();

    do
    {
        IO0CLR = 0X000F0000;
        IO0SET = 0X00010000;
        delay_ms(10);
        if (--no_of_steps_clk == 0)
            break;
        IO0CLR = 0X000F0000;
        IO0SET = 0X00020000;
        delay_ms(10);
        if (--no_of_steps_clk == 0)
            break;
        IO0CLR = 0X000F0000;
        IO0SET = 0X00040000;
        delay_ms(10);
        if (--no_of_steps_clk == 0)
            break;
        IO0CLR = 0X000F0000;
        IO0SET = 0X00080000;
        delay_ms(10);
        if (--no_of_steps_clk == 0)
            break;
    } while (1);
    // n1 = n2;
    do
    {
        IO0CLR = 0X000F0000;
        IO0SET = 0X00080000;
        delay_ms(10);
        if (--no_of_steps_aclk == 0)
            break;
        IO0CLR = 0X000F0000;
        IO0SET = 0X00040000;
        delay_ms(10);
        if (--no_of_steps_aclk == 0)
            break;
        IO0CLR = 0X000F0000;
        IO0SET = 0X00020000;
        delay_ms(10);
        if (--no_of_steps_aclk == 0)
            break;
        IO0CLR = 0X000F0000;
        IO0SET = 0X00010000;
        delay_ms(10);
        if (--no_of_steps_aclk == 0)
            break;
    } while (1);
    IO0CLR = 0X00FF0000;
		
    return;
}


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


void lcdTest()
{
    SystemInit();
    IO0DIR |= 1U << 31 | 0x00FF0000; // to set P0.16 to P0.23 as o/ps
    IO1DIR |= 1U << 25;              // to set P1.25 as o/p used for EN
    // make D7 Led on off for testing
    
   
    
    LCD_Reset();
    LCD_Init();
    delay_ms(100);
    LCD_CmdWrite(0x80);
    LCD_DisplayString("RV College Of Engrng");
    LCD_CmdWrite(0xc0);
    LCD_DisplayString(" Computer Sciene");
    LCD_CmdWrite(0x94);
    LCD_DisplayString(" 4th Semester");
    LCD_CmdWrite(0xD4);
    LCD_DisplayString(" B Section");
    while (1)
       if(!SW6)
					break;
}

int main()
{
    while (1)
    {
        unsigned char test = programControl();
        switch (test)
        {
        case '1':
            counterProgram(); 
				break;
        case '2':
            sevenSegDisplay(); 
				break;
        case '3':
            stepperMotorTest(); 
				break;
				case '7':
						lcdTest();
							break;
        default:
            break;
        }
        if (test == 'f')
            break;
    }
    return 0;
}