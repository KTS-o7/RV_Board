#include <lpc214x.h>
#define LED_OFF (IO0SET = 1U << 31)
#define LED_ON (IO0CLR = 1U << 31)
#define SW2 (IO0PIN & (1 << 14))
#define SW3 (IO0PIN & (1 << 15))
#define SW4 (IO1PIN & (1 << 18))
#define SW5 (IO1PIN & (1 << 19))
#define SW6 (IO1PIN & (1 << 20))
#define IS_ON(pin) (IO1PIN & (1U << (pin)))
#define PLOCK 0x00000400
// #define WHILE_LOOP_VAL 10000
#define COL0 (IO1PIN & 1 << 19)
#define COL1 (IO1PIN & 1 << 18)
#define COL2 (IO1PIN & 1 << 17)
#define COL3 (IO1PIN & 1 << 16)
#define RS_ON (IO0SET = 1U << 20)
#define RS_OFF (IO0CLR = 1U << 20)
#define EN_ON (IO1SET = 1U << 25)
#define EN_OFF (IO1CLR = 1U << 25)

static void delay_us(unsigned int count); // microsecond delay
static void LCD_SendCmdSignals(void);
static void LCD_SendDataSignals(void);
static void LCD_SendHigherNibble(unsigned char dataByte);
static void LCD_CmdWrite(unsigned char cmdByte);
static void LCD_DataWrite(unsigned char dataByte);
static void LCD_Reset(void);
static void LCD_Init(void);
void LCD_DisplayString(const char *ptr_stringPointer_u8);

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

unsigned char lookup_table[4][4] = {{'0', '1', '2', '3'},
                                    {'4', '5', '6', '7'},
                                    {'8', '9', 'a', 'b'},
                                    {'c', 'd', 'e', 'f'}};
unsigned char rowsel = 0, colsel = 0;

void uart_init(void)
{
    // configurations to use serial port
    PINSEL0 |= 0x00000005; // P0.0 & P0.1 ARE CONFIGURED AS TXD0 & RXD0
    U0LCR = 0x83;          /* 8 bits, no Parity, 1 Stop bit */
    U0DLM = 0;
    U0DLL = 8;    // 115200 baud rate
    U0LCR = 0x03; /* DLAB = 0 */
    U0FCR = 0x07; /* Enable and reset TX and RX FIFO. */
}

unsigned char getAlphaCode(unsigned char alphachar)
{
    switch (alphachar)
    {
    // dp g f e d c b a - common anode: 0 segment on, 1 segment off
    case 'f':
        return 0x8e;
    case 'i':
        return 0xf9;
    case 'r':
        return 0xce;
    case 'e':
        return 0x86; // 1000 0110
    case 'h':
        return 0x89;
    case 'l':
        return 0xc7;
    case 'p':
        return 0x8c;
    case ' ':
        return 0xff;
    // simmilarly add for other digit/characters
    default:
        break;
    }
    return 0xff;
}

void delay_ms(unsigned int j)
{
    unsigned int x, i;
    for (i = 0; i < j; i++)
    {
        for (x = 0; x < 1000; x++)
            ; /* loop to generate 1 milisecond delay with CCLK = 60MHz */
    }
}

void reset_values(int y)
{
    switch (y)
    {
    case 0:
        contDN = 99;
        rightSFT = 0x01 << 7;
        leftSFT = 0x01;
        break;
    case 1:
        contUP = 0;
        rightSFT = 0x01 << 7;
        leftSFT = 0x01;
        break;
    case 2:
        contUP = 0;
        contDN = 99;
        rightSFT = 0x01 << 7;
        break;
    case 3:
        contUP = 0;
        contDN = 99;
        leftSFT = 0x01;
        break;
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
    }
    IOSET0 |= 1 << 30; // IOSET0 | 0x40000000;
    delay_ms(1);       // nop();
    IOCLR0 |= 1 << 30; // IOCLR0 | 0x40000000;
    return;
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
void program_1a()
{
    IO0DIR = 1U << 31;
    IO0SET = 1U << 31;
    //int n = WHILE_LOOP_VAL;
    while (1)
    {
        if (!(IO0PIN & (1 << 14))) //(if(!SW2 )
        {
            IO0CLR = 1U << 31; // LED_ON
            delay_ms(250);
            IO0SET = 1U << 31; // LED_OFF
            delay_ms(250);
        }
    }
}

void program_1b()
{
    IO0DIR = 0xFF<<16;
	IO0SET = 0xFF << 16;
    //int n = WHILE_LOOP_VAL;
    while (1)
    {
        if (!SW2)
        {
            reset_values(0);
            IO0SET = 0xFF << 16;
            IO0CLR = ((contUP / 10) << 4 | (contUP % 10)) << 16;
            contUP++;
            if (contUP >99)
                contUP = 0;
        }
        else if (!SW3)
        {
            reset_values(1);
            IO0SET = 0xFF << 16;
            IO0CLR = ((contDN / 10) << 4 | (contDN % 10)) << 16;
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
        delay_ms(200);
    }
}

void program_2b()
{
    IO0DIR |= 1U << 31 | 1U << 19 | 1U << 20 | 1U << 30; // to set as o/ps
    LED_ON;                                              // make D7 Led on .. just indicate the program is running
    SystemInit();
    //int n = WHILE_LOOP_VAL;
    while (1)
    {
        alphadisp7SEG("fire ");
        delay_ms(500);
        alphadisp7SEG("help ");
        delay_ms(500);
    }
}

void program_3b()
{
    unsigned int no_of_steps_clk = 1, no_of_steps_aclk = 2;
    IO0DIR |= 1U << 31 | 0x00FF0000 | 1U << 30; // to set P0.16 to P0.23 as o/ps
    LED_ON;
    delay_ms(500);
    LED_OFF; // make D7 Led on .. just indicate the program is running
    SystemInit();
    //int n1 = WHILE_LOOP_VAL;
    //int n2 = n1;
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
    while (1)
        ;
}

void program_4b()
{
    short int value, i = 0;
    SystemInit();
    PINSEL1 |= 0x00080000;           /* P0.25 as DAC output :option 3 - 10 (bits18,19)*/
    IO0DIR |= 1U << 31 | 0x00FF0000; // to set P0.16 to P0.23 as o/ps
    //int n = WHILE_LOOP_VAL;
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
            while (i != 30)
            {
                value = sine_rect_table[i++];
                DACR = ((1 << 16) | (value << 6));
                delay_ms(1);
            }
            i = 0;
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
        else if (!SW6) /* If switch for square wave is pressed */
        {
            value = 1023;
            DACR = ((1 << 16) | (value << 6));
            delay_ms(1);
            value = 0;
            DACR = ((1 << 16) | (value << 6));
            delay_ms(1);
        }
        else /* If no switch is pressed, 3.3V DC */
        {
            value = 1023;
            DACR = ((1 << 16) | (value << 6));
        }
    }
}

void program_5b()
{
    SystemInit();
    uart_init();                     // initialize UART0 port
    IO0DIR |= 1U << 31 | 0x00FF0000; // to set P0.16 to P0.23 as o/ps
    // make D7 Led on off for testing
    LED_ON;
    delay_ms(500);
    LED_OFF;
    delay_ms(500);
    //int n1 = WHILE_LOOP_VAL;
    //int n2 = WHILE_LOOP_VAL;
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
            ;                                 // wait for key release
        delay_ms(50);                         // allow for key debouncing
        IO0SET = 0X000F0000;                  // disable all the rows
        U0THR = lookup_table[rowsel][colsel]; // send to serial port(check on the terminal)
    } while (1);
}

void program_6b()
{
    int dig_val;
    IO0DIR |= 1U << 31 | 0x00FF0000 | 1U << 30; // to set P0.16 to P0.23 as o/ps
    LED_ON;
    delay_ms(500);
    LED_OFF; // make D7 Led on / off for program checking
    SystemInit();
    //int n = WHILE_LOOP_VAL;
    do
    {
        dig_val = adc(1, 2) / 10;
        if (dig_val > 100)
            dig_val = 100;
        runDCMotor(2, dig_val); // run at 10% duty cycle
    } while (1);
}

void program_7b()
{
    SystemInit();
    IO0DIR |= 1U << 31 | 0x00FF0000; // to set P0.16 to P0.23 as o/ps
    IO1DIR |= 1U << 25;              // to set P1.25 as o/p used for EN
    // make D7 Led on off for testing
   // int n = WHILE_LOOP_VAL;
    LED_ON;
    delay_ms(500);
    LED_OFF;
    delay_ms(500);
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
        ;
}

int main()
{
    //program_1a();
    //program_1b();
    //program_2b();
    program_3b();
		//program_4b();
		//program_5b();
		//program_6b();
		//program_7b();
    return 0;
}