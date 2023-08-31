#include <lpc214x.h>
#define LED_OFF (IO0SET = 1U << 31)
#define LED_ON (IO0CLR = 1U << 31)
#define PLOCK 0x00000400
void delay_ms(unsigned int j);
void SystemInit(void);

int main()
{
	unsigned int steps_clk = 200, steps_aclk = 200;
	IO0DIR |= 1U << 31 | 0x00FF0000 | 1U<<30;
	LED_ON; delay_ms(500); LED_OFF;
	SystemInit();
	/*
	do{
		IO0CLR = 0x000F0000; IO0SET = 0x00010000; delay_ms(10); if(--steps_clk == 0) break;
		IO0CLR = 0x000F0000; IO0SET = 0x00020000; delay_ms(10); if(--steps_clk == 0) break;
		IO0CLR = 0x000F0000; IO0SET = 0x00040000; delay_ms(10); if(--steps_clk == 0) break;
		IO0CLR = 0x000F0000; IO0SET = 0x00080000; delay_ms(10); if(--steps_clk == 0) break;
	}while(1);
	do{
		IO0CLR = 0x000F0000; IO0SET = 0x00080000; delay_ms(10); if(--steps_aclk == 0) break;
		IO0CLR = 0x000F0000; IO0SET = 0x00040000; delay_ms(10); if(--steps_aclk == 0) break;
		IO0CLR = 0x000F0000; IO0SET = 0x00020000; delay_ms(10); if(--steps_aclk == 0) break;
		IO0CLR = 0x000F0000; IO0SET = 0x00010000; delay_ms(10); if(--steps_aclk == 0) break;
	}while(1);
	*/
	
	IO0CLR = 0xFF0000;
	while(1);
}
void delay_ms(unsigned int j)
{
		unsigned int x, i;
		for(i=0; i<j; i++)
		for(x=0;x<10000;x++);
}

void SystemInit(void)
{
	PLL0CON = 0x01;
	PLL0CFG = 0x24;
	PLL0FEED = 0xAA;
	PLL0FEED = 0x55;
	while(!(PLL0STAT & PLOCK))
	{
		;
	}
	
	PLL0CON = 0x03;
	PLL0FEED = 0xAA;
	PLL0FEED = 0x55;
	VPBDIV = 0x01;
}
