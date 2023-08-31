#include <lpc214x.h>
#define LED_OFF (IO0SET = 1U <<31)
#define LED_ON (IO0CLR = 1U <<31)
#define PLOCK 0x00000400

void delay_ms(unsigned int j);
void SystemInit(void);
unsigned char getAlphaCode(unsigned char alphachar);
void alphadisp7SEG(char *buf);
int main()
{
	IO0DIR |= 1U << 31 | 1U << 19 | 1U << 20 | 1U << 30;
	LED_ON;
	SystemInit();
	while(1)
	{
		alphadisp7SEG("fire");
		delay_ms(500);
		alphadisp7SEG("help");
		delay_ms(500);
	}
}

unsigned char getAlphaCode(unsigned char alphachar)
{
	switch(alphachar)
	{
		case 'f': return 0x8e;
		case 'i': return 0xf9;
		case 'r': return 0xce;
		case 'e': return 0x86;
		case 'h': return 0x89;
		case 'l': return 0xc7;
		case 'p': return 0x8c;
		case ' ': return 0xff;
		default : break;
	}
	return 0xff;
}

void alphadisp7SEG(char* buf)
{
	unsigned char i,j;
	unsigned char seg7_data, temp = 0;
	for(i=0; i<5; i++)
	{
		seg7_data = getAlphaCode(*(buf+i));
		for(j=0; j<8; j++)
		{
			temp = seg7_data & 0x80;
			if(temp == 0x80)
				IO0SET |= 1<< 19;
			else
				IO0CLR |= 1<<19;
			
			IO0SET |= 1<<20;
			delay_ms(1);
			IO0CLR |=1<<20;
			seg7_data = seg7_data << 1;
		}
	}
	
	IO0SET |= 1<<30;
	delay_ms(1);
	IO0CLR |=1<<30;
	return;
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

void delay_ms(unsigned int j)
{
	unsigned int x, i;
	for(i=0;i<j;i++)
	for(x=0; x<10000; x++);
}
