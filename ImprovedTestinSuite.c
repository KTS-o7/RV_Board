#include <lpc214x.h>
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

// LCD pins
#define RS_ON (IO0SET = 1U << 20)
#define RS_OFF (IO0CLR = 1U << 20)
#define EN_ON (IO1SET = 1U << 25)
#define EN_OFF (IO1CLR = 1U << 25)

// The following variables are used in Up-Down and Ring Counter program
int contUP = 0;
int contDN = 99;
unsigned int rightSFT = 1U << 7;
unsigned int leftSFT = 1;

const int key0 = 16;
const int key1 = 17;
const int key2 = 18;
const int key3 = 19;
short int sine_table[] = {512 + 0, 512 + 53, 512 + 106, 512 + 158, 512 + 208, 512 + 256, 512 + 300, 512 + 342, 512 + 380, 512 + 413, 512 + 442, 512 + 467, 512 + 486, 512 + 503, 512 + 510, 512 + 511, 512 + 510, 512 + 503, 512 + 486, 512 + 467, 512 + 442, 512 + 413, 512 + 380, 512 + 342, 512 + 300, 512 + 256, 512 + 208, 512 + 158, 512 + 106, 512 + 53, 512 + 0,
                          512 - 53, 512 - 106, 512 - 158, 512 - 208, 512 - 256, 512 - 300, 512 - 342, 512 - 380, 512 - 413, 512 - 442, 512 - 467, 512 - 486, 512 - 503, 512 - 510, 512 - 511,
                          512 - 510, 512 - 503, 512 - 486, 512 - 467, 512 - 442, 512 - 413, 512 - 380, 512 - 342, 512 - 300, 512 - 256, 512 - 208, 512 - 158, 512 - 106, 512 - 53};
short int sine_rect_table[] =
    {512 + 0, 512 + 53, 512 + 106, 512 + 158, 512 + 208, 512 + 256, 512 + 300, 512 + 342, 512 + 380, 512 + 413, 512 + 442, 512 + 467, 512 + 486, 512 + 503, 512 + 510, 512 + 511,
     512 + 510, 512 + 503, 512 + 486, 512 + 467, 512 + 442, 512 + 413, 512 + 380, 512 + 342, 512 + 300, 512 + 256, 512 + 208, 512 + 158, 512 + 106, 512 + 53, 512 + 0};

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
void MenuDisplay();
void cycleADC();
void LCD_DisplayString(const char *ptr_stringPointer_u8);
void DACWaveOut();
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
    // Switches on all the segments of one display along with decimal point.
    case ' ':
        return 0xff;
        // simmilarly add for other digit/characters
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
    IO0DIR |= 1U << 31 | 1U << 19 | 1U << 20 | 1U << 30; // to set as o/ps
    PINSEL0 |= 0x0F300000;
    IO0DIR |= 1U << 31 | 0x00FF0000; // to set P0.16 to P0.23 as o/ps
    IO1DIR |= 1U << 25;
    LCD_Reset();
    LCD_Init();
    delay_ms(100);
    LCD_CmdWrite(0x80);
    LCD_DisplayString("Counter");

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
        {
            MenuDisplay();
            break;
        }
        delay_ms(200);
    }
    return 0;
}

// Has seven segment display test code
void sevenSegDisplay()
{
    IO0DIR |= 1U << 31 | 1U << 19 | 1U << 20 | 1U << 30; // to set as o/ps
    IO0DIR |= 1U << 31 | 1U << 19 | 1U << 20 | 1U << 30; // to set as o/ps
    PINSEL0 |= 0x0F300000;
    IO0DIR |= 1U << 31 | 0x00FF0000; // to set P0.16 to P0.23 as o/ps
    IO1DIR |= 1U << 25;
    LCD_Reset();
    LCD_Init();
    delay_ms(100);
    LCD_CmdWrite(0x80);
    LCD_DisplayString("7Seg");
    while (1)
    {
        alphadisp7SEG("88888");
        delay_ms(300);
        alphadisp7SEG("     ");
        delay_ms(300);
        if (!SW6)
        {
            MenuDisplay();
            break;
        }
    }
}

void stepperMotorTest()
{
    unsigned int no_of_steps_clk = 200, no_of_steps_aclk = 200;
    IO0DIR |= 1U << 31 | 0x00FF0000 | 1U << 30; // to set P0.16 to P0.23 as o/ps
    IO0DIR |= 1U << 31 | 0x00FF0000;            // to set P0.16 to P0.23 as o/ps
    IO1DIR |= 1U << 25;
    LCD_Reset();
    LCD_Init();
    delay_ms(100);
    LCD_CmdWrite(0x80);
    LCD_DisplayString("Stepper Motor");
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
        if (!SW6)
        {
            MenuDisplay();
            break;
        }
    } while (1);
    IO0CLR = 0X00FF0000;

    return;
}

void runDCMotor(int direction, int dutycycle)
{
    IO0DIR |= 1U << 28; // set P0.28 as output pin
    PINSEL0 |= 2 << 18; // select P0.9 as PWM6 (option 2)
    if (direction == 1)
        IO0SET = 1 << 28; // set to 1, to choose anti-clockwise direction
    else
        IO0CLR = 1 << 28;               // set to 0, to choose clockwise direction
    PWMPCR = (1 << 14);                 // enable PWM6
    PWMMR0 = 1000;                      // set PULSE rate to value suitable for DC Motor operation
    PWMMR6 = (1000U * dutycycle) / 100; // set PULSE period
    PWMTCR = 0x00000009;                // bit D3 = 1 (enable PWM), bit D0=1 (start the timer) PWMLER = 0X70; // load the new values to PWMMR0 and PWMMR6 registers
}

unsigned int adc(int no, int ch)
{
    // adc(1,4) for temp sensor LM34, digital value will increase as temp increases
    // adc(1,3) for LDR - digival value will reduce as the light increases
    // adc(1,2) for trimpot - digital value changes as the pot rotation
    unsigned int val;
    PINSEL0 |= 0x0F300000; /* Select the P0_13 AD1.4 for ADC function */
    /* Select the P0_12 AD1.3 for ADC function */
    /* Select the P0_10 AD1.2 for ADC function */
    switch (no) // select adc
    {
    case 0:
        AD0CR = 0x00200600 | (1 << ch); // select channel
        AD0CR |= (1 << 24);             // start conversion
        while ((AD0GDR & (1U << 31)) == 0)
            ;
        val = AD0GDR;
        break;
    case 1:
        AD1CR = 0x00200600 | (1 << ch); // select channel
        AD1CR |= (1 << 24);             // start conversion
        while ((AD1GDR & (1U << 31)) == 0)
            ;
        val = AD1GDR;
        break;
    }
    val = (val >> 6) & 0x03FF; // bit 6:15 is 10 bit AD value
    return val;
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
void AdcWithDCMotor()
{
    int dig_val;
    IO0DIR |= 1U << 31 | 0x00FF0000 | 1U << 30;          // to set P0.16 to P0.23 as o/ps
    IO0DIR |= 1U << 31 | 1U << 19 | 1U << 20 | 1U << 30; // to set as o/ps
    PINSEL0 |= 0x0F300000;
    IO0DIR |= 1U << 31 | 0x00FF0000; // to set P0.16 to P0.23 as o/ps
    IO1DIR |= 1U << 25;
    LCD_Reset();
    LCD_Init();
    delay_ms(100);
    LCD_CmdWrite(0x80);
    LCD_DisplayString("ADC DC Motor");
    delay_ms(500);

    SystemInit();
    // int n = WHILE_LOOP_VAL;
    do
    {
        dig_val = adc(1, 2) / 10;
        if (dig_val > 100)
            dig_val = 100;
        runDCMotor(2, dig_val); // run at 10% duty cycle
        if (!SW6)
        {
            MenuDisplay();
            break;
        }
    } while (1);
}

void lcdTest()
{
    SystemInit();
    IO0DIR |= 1U << 31 | 0x00FF0000;                     // to set P0.16 to P0.23 as o/ps
    IO1DIR |= 1U << 25;                                  // to set P1.25 as o/p used for EN
                                                         // make D7 Led on off for testing
    IO0DIR |= 1U << 31 | 1U << 19 | 1U << 20 | 1U << 30; // to set as o/ps
    PINSEL0 |= 0x0F300000;
    IO0DIR |= 1U << 31 | 0x00FF0000; // to set P0.16 to P0.23 as o/ps
    IO1DIR |= 1U << 25;
    LCD_Reset();
    LCD_Init();
    delay_ms(100);
    LCD_CmdWrite(0x80);
    LCD_DisplayString("LCD Testing");
    delay_ms(1000);
    LCD_Reset();
    LCD_Init();
    delay_ms(100);
    LCD_CmdWrite(0x80);
    LCD_DisplayString("RV College Of Engrng");
    LCD_CmdWrite(0xc0);
    LCD_DisplayString(" Computer Science");
    LCD_CmdWrite(0x94);
    LCD_DisplayString(" 4th Sem 2022-23");
    LCD_CmdWrite(0xD4);
    LCD_DisplayString(" B Section");
    while (1)
        if (!SW6)
        {
            MenuDisplay();
            break;
        }
}

void DACWaveOut()
{
    short int value, i = 0;
    SystemInit();
    PINSEL1 |= 0x00080000;                               /* P0.25 as DAC output :option 3 - 10 (bits18,19)*/
    IO0DIR |= 1U << 31 | 0x00FF0000;                     // to set P0.16 to P0.23 as o/ps
    IO0DIR |= 1U << 31 | 1U << 19 | 1U << 20 | 1U << 30; // to set as o/ps
    PINSEL0 |= 0x0F300000;
    IO0DIR |= 1U << 31 | 0x00FF0000; // to set P0.16 to P0.23 as o/ps
    IO1DIR |= 1U << 25;
    LCD_Reset();
    LCD_Init();
    delay_ms(100);
    LCD_CmdWrite(0x80);
    LCD_DisplayString("DAC Test");
    while (1)
    {
        if (!SW2) /* If switch for sine wave is pressed */
        {
            while (i != 60)
            {
                value = sine_table[i++];
                DACR = ((1 << 16) | (value << 6));
                delay_ms(1);
            }
            i = 0;
        }
        else if (!SW3)
        {
            value = 1023;
            DACR = ((1 << 16) | (value << 6));
            delay_ms(1);
            value = 0;
            DACR = ((1 << 16) | (value << 6));
            delay_ms(1);
        }
        else if (!SW4) /* If switch for triangular wave is pressed */
        {
            value = 0;
            while (value != 1023)
            {
                DACR = ((1 << 16) | (value << 6));
                value++;
            }
            while (value != 0)
            {
                DACR = ((1 << 16) | (value << 6));
                value--;
            }
        }
        else if (!SW5) /* If switch for sawtooth wave is pressed */
        {
            value = 0;
            while (value != 1023)
            {
                DACR = ((1 << 16) | (value << 6));
                value++;
            }
        }

        else /* If no switch is pressed, 3.3V DC */
        {
            value = 1023;
            DACR = ((1 << 16) | (value << 6));
        }
        if (!SW6)
        {
            MenuDisplay();
            break;
        }
    }
}

// void readSensor(){
//  int result = 0;
//	IO0DIR |= 1U << 31 | 1U << 19 | 1U << 20 | 1U << 30 | 1U << 24; // to set as o/ps
//	while(1){
//	IO0SET |= 1<<31;
//	IO1CLR = 1<<24;
//	result = IO1PIN &(1<<22);
//	IO1SET = 1<<24;
//		if(result) IO0CLR |= 1<<31;
//		delay_ms(100);
//	char buf[1];
//	sprintf(buf, "%d", result);
//	alphadisp7SEG(buf);
//	alphadisp7SEG("     ");

//		if(!SW6) break;
//	}
//}

void readSensor()
{

    int result = 0;
    IO0DIR |= 1U << 31 | 1U << 19 | 1U << 20 | 1U << 30 | 1U << 24;
    IO1DIR |= 1 << 24;
    IO0DIR |= 1U << 31 | 1U << 19 | 1U << 20 | 1U << 30; // to set as o/ps
    PINSEL0 |= 0x0F300000;
    IO0DIR |= 1U << 31 | 0x00FF0000; // to set P0.16 to P0.23 as o/ps
    IO1DIR |= 1U << 25;
    LCD_Reset();
    LCD_Init();
    delay_ms(100);
    LCD_CmdWrite(0x80);
    LCD_DisplayString("Proximity Sensor");
    while (1)
    {
        IO1CLR = 1 << 24; // enable sensor logic: P1.24 - 0

        result = IO1PIN & (1 << 22); // P1.22 connected to sensor1

        IO1SET = 1 << 24; // disabl sensor logic: P1.24
        if (result)
            alphadisp7SEG(" 1 ");
        else
            alphadisp7SEG(" 0 ");
        if (!SW6)
        {
            MenuDisplay();
            break;
        }
    }
}

void PWM_Init(void)
{
    PINSEL0 |= 2 << 18 | 2 << 16;
    // SELECT P0.8 PWM4 AND P0.9PWM6 AS 2ND OPTION FOR PWM OPERATION
    PINSEL1 |= 1 << 10;                     // SELECT P0.21 PWM5 AS OPTION 1 FOR PWM OPERATION
    PWMPCR = (1 << 12 | 1 << 13 | 1 << 14); // Enable PWM4,PWM5 and PWM
    PWMMR0 = 100;                           // load the value to MR0 to fix the pulse rate
    PWMTCR = 0x00000009;                    // bit D3 = 1 (enable PWM), bit D0=1 (start the timer)
}

void RGB(void)
{
    IO0DIR |= 1U << 31 | 1U << 19 | 1U << 20 | 1U << 30; // to set as o/ps
    PINSEL0 |= 0x0F300000;
    IO0DIR |= 1U << 31 | 0x00FF0000; // to set P0.16 to P0.23 as o/ps
    IO1DIR |= 1U << 25;
    LCD_Reset();
    LCD_Init();
    delay_ms(100);
    LCD_CmdWrite(0x80);
    LCD_DisplayString("RGB Light");
    unsigned int i;
    PWM_Init();
    while (1)
    {
        for (i = 0; i <= 100; i = i + 1)
        {
            PWMMR4 = 100;
            PWMMR5 = i;
            PWMMR6 = 100;
            PWMLER = 0X70; // to enable copy to Match registers from shadow regs. 1110000
            delay_ms(10);
        }
        for (i = 0; i <= 100; i = i + 1)
        {
            PWMMR4 = 100;
            PWMMR5 = 100;
            PWMMR6 = i;
            PWMLER = 0X70; // to enable copy to Match registers from shadow regs. 1110000
            delay_ms(10);
        }
        for (i = 0; i <= 100; i = i + 1)
        {
            PWMMR4 = i;
            PWMMR5 = 100;
            PWMMR6 = 100;
            PWMLER = 0X70; // to enable copy to Match registers from shadow regs. 1110000
            delay_ms(10);
        }
        if (!SW6)
        {
            MenuDisplay();
            break;
        }
    }
}

void relay()
{
    int result = 1;
    IO0DIR |= 1U << 31 | 1U << 19 | 1U << 20 | 1U << 30 | 1U << 24;

    IO0DIR |= 1U << 0;                                   // configure P0.0 as output, connected to Relay
    IO0DIR |= 1U << 31 | 1U << 19 | 1U << 20 | 1U << 30; // to set as o/ps
    PINSEL0 |= 0x0F300000;
    IO0DIR |= 1U << 31 | 0x00FF0000; // to set P0.16 to P0.23 as o/ps
    IO1DIR |= 1U << 25;
    LCD_Reset();
    LCD_Init();
    delay_ms(100);
    LCD_CmdWrite(0x80);
    LCD_DisplayString("Relay Test");
    while (1)
    {

        // P0.1 connected to sensor1

        if (result)
            IO0CLR = 1U << 0;

        else
            IO0SET = 1U << 0;
        delay_ms(100);
        if (result)
            alphadisp7SEG(" 1 ");
        else
            alphadisp7SEG(" 0 ");

        result = !result;

        if (!SW6)
        {
            MenuDisplay();
            break;
        }
    }
}

void MenuDisplay()
{
    IO0DIR |= 1U << 31 | 0x00FF0000; // to set P0.16 to P0.23 as o/ps
    IO1DIR |= 1U << 25;              // to set P1.25 as o/p used for EN
    LCD_Reset();
    LCD_Init();
    delay_ms(100);
    LCD_CmdWrite(0x80);
    LCD_DisplayString("1-Counter 2-7Seg ");
    LCD_CmdWrite(0xc0);
    LCD_DisplayString("3-step 4-DAC 5-RGB");
    LCD_CmdWrite(0x94);
    LCD_DisplayString(" 6-DC 7-LCD 8-ADC");
    LCD_CmdWrite(0xD4);
    LCD_DisplayString("9-Prox A-Relay");
}

void InstructionWrite(char *buf)
{
    IO0DIR |= 1U << 31 | 0x00FF0000; // to set P0.16 to P0.23 as o/ps
    IO1DIR |= 1U << 25;              // to set P1.25 as o/p used for EN
    LCD_Reset();
    LCD_Init();
    delay_ms(100);
    LCD_CmdWrite(0x80);
    LCD_DisplayString(buf);
    LCD_CmdWrite(0xc0);
    LCD_DisplayString("Press SW6 to exit");
}
int main()
{ // Print instructions to run the program
    SystemInit();
    LCD_Reset();
    LCD_CmdWrite(0x80);
    LCD_DisplayString("Welcome to");
    LCD_CmdWrite(0xc0);
    LCD_DisplayString("RV Board Testing");
    LCD_CmdWrite(0x94);
    LCD_DisplayString("Suite.");
    delay_ms(500);
    MenuDisplay();

    while (1)
    {

        unsigned char test = programControl();
        switch (test)
        {
        case '1':
            InstructionWrite("Counter Prgm");
            counterProgram();
            break;
        case '2':
            InstructionWrite("7Seg Prgm")
                sevenSegDisplay();
            break;
        case '3':
            InstructionWrite("Stepper Motor");
            stepperMotorTest();
            break;
        case '4':
            InstructionWrite("DAC Prgm");
            DACWaveOut();
            break;
        case '5':
            InstructionWrite("RGB Prgm");
            RGB();
            break;
        case '6':
            InstructionWrite("DC Motor Prgm");
            AdcWithDCMotor();
            break;
        case '7':
            InstructionWrite("LCD Prgm");
            lcdTest();
            break;
        case '8':
            // InstructionWrite("ADC Prgm");
            cycleADC();
            break;
        case '9':
            InstructionWrite("Proximity Prgm");
            readSensor();
            break;
        case 'a':
            InstructionWrite("Relay Prgm");
            relay();
            break;
        default:
            break;
        }
        if (test == 'f')
            break;
    }
    return 0;
}

void cycleADC()
{
    int no = 1;
    int ch = 2;
    unsigned int val;
    IO0DIR |= 1U << 31 | 1U << 19 | 1U << 20 | 1U << 30; // to set as o/ps
    PINSEL0 |= 0x0F300000;
    IO0DIR |= 1U << 31 | 0x00FF0000; // to set P0.16 to P0.23 as o/ps
    IO1DIR |= 1U << 25;
    LCD_Reset();
    LCD_Init();
    delay_ms(100);
    LCD_CmdWrite(0x80);
    LCD_DisplayString("ADC Prgm");
    LCD_CmdWrite(0xc0);
    LCD_DisplayString("Press SW6 to exit");
    LCD_CmdWrite(0x94);
    LCD_DisplayString("cycle ADC");
    while (1)
    {
        AD1CR = 0x00200600 | (1 << ch); // select channel
        AD1CR |= (1 << 24);             // start conversion
        while ((AD1GDR & (1U << 31)) == 0)
            ;
        val = AD1GDR;
        val = (val >> 6) & 0x03FF; // bit 6:15 is 10 bit AD value
        char buf[5];
        switch (ch)
        {
        case 2:
            alphadisp7SEG("  02 ");
            break;
        case 3:
            alphadisp7SEG("  03 ");
            break;
        case 4:
            alphadisp7SEG("  04 ");
            break;
        }
        sprintf(buf, "%5d", val);
        delay_ms(2);
        alphadisp7SEG(buf);
        ch++;
        if (ch > 4)
            ch = 2;

        if (!SW6)
        {
            MenuDisplay();
            break;
        }
    }
    // adc(1,4) for temp sensor LM34, digital value will increase as temp increases
    // adc(1,3) for LDR - digival value will reduce as the light increases
    // adc(1,2) for trimpot - digital value changes as the pot rotation
}