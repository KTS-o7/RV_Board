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
    }
    // Strobe Signal
    IOSET0 |= 1 << 30; // IOSET0 | 0x40000000;
    delay_ms(1);       // nop();
    IOCLR0 |= 1 << 30; // IOCLR0 | 0x40000000;
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
    while (1)
        ;
}

int main()
{
    while (1)
    {
        unsigned char test = programControl();
        switch (test)
        {
        case '1':
            counterProgram() break;
        case '2':
            sevenSegDisplay() break;
        case '3':
            stepperMotorTest() break;
        default:
            break;
        }
        if (test == 'f')
            break;
    }
    return 0;
}