#include <lpc214x.h>
#define PLOCK 0x00000400


/*void SystemInit(void)
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
}*/


void delay_ms(unsigned int j)
{
    unsigned int x, i;
    for (i = 0; i < j; i++)
    {
        for (x = 0; x < 1000; x++)
            ; /* loop to generate 1 milisecond delay with CCLK = 60MHz */
    }
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



void program_2b()
{
    IO0DIR |= 1U << 31 | 1U << 19 | 1U << 20 | 1U << 30; // to set as o/ps
    //LED_ON;                                              // make D7 Led on .. just indicate the program is running
    //SystemInit();
    
    while (1)
    {
        alphadisp7SEG("fire ");
        delay_ms(500);
        alphadisp7SEG("help ");
        delay_ms(500);
    }
}

int main()
{
	program_2b();
	return 0;
}