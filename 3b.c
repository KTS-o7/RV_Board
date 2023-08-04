#include <lpc214x.h>
#define PLOCK 0x00000400


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


void delay_ms(unsigned int j)
{
    unsigned int x, i;
    for (i = 0; i < j; i++)
    {
        for (x = 0; x < 1000; x++)
            ; /* loop to generate 1 milisecond delay with CCLK = 60MHz */
    }
}

void program_3b()
{
    unsigned int no_of_steps_clk = 100, no_of_steps_aclk = 100;
    IO0DIR |= 1U << 31 | 0x00FF0000 | 1U << 30; // to set P0.16 to P0.23 as o/ps
   // LED_ON;
   // delay_ms(500);
    //LED_OFF; // make D7 Led on .. just indicate the program is running
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

int main()
{
program_3b();
	return 0;
}
