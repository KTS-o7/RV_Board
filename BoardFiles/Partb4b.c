#include <lpc214x.h>
#include <stdio.h>
#define PLOCK 0x00000400
#define LED_OFF (IO0SET = 1U << 31)
#define LED_ON (IO0CLR = 1U << 31)
#define SW2 (IO0PIN & (1 << 14))
#define SW3 (IO0PIN & (1 << 15))
#define SW4 (IO1PIN & (1 << 18))
#define SW5 (IO1PIN & (1 << 19))
#define SW6 (IO1PIN & (1 << 20))
void SystemInit(void);
static void delay_ms(unsigned int j);//millisecond delay
short int sine_table[ ] =
{512+0,512+53,512+106,512+158,512+208,512+256,512+300,512+342,512+380,512+413,
512+442,512+467,512+486,512+503,512+510,512+511,
512+510,512+503,512+486,512+467,512+442,512+413,512+380,512+342,512+300,512+256,512+208,512+158,512+106,512+53,512+0,
512-53,512-106,512-158,512-208,512-256,512-300,512-342,512-380,512-413,512-442,512-
467,512-486,512-503,512-510,512-511,
512-510,512-503,512-486,512-467,512-442,512-413,512-380,512-342,512-300,512-
256,512-208,512-158,512-106,512-53};
short int sine_rect_table[ ] =
{512+0,512+53,512+106,512+158,512+208,512+256,512+300,512+342,512+380,512+413,
512+442,512+467,512+486,512+503,512+510,512+511,
512+510,512+503,512+486,512+467,512+442,512+413,512+380,512+342,512+300,512+256,512+208,512+158,512+106,512+53,512+0};

int main()
{
short int value,i=0;
SystemInit();
PINSEL1 |= 0x00080000; /* P0.25 as DAC output :option 3 - 10 (bits18,19)*/
IO0DIR |= 1U << 31 | 0x00FF0000 ; // to set P0.16 to P0.23 as o/ps
while(1)
{
if (!SW2) /* If switch for sine wave is pressed */
{
while (i!=60 )
{
value = sine_table[i++];
DACR = ( (1<<16) | (value<<6) );
delay_ms(1);
}
i=0;
}
else if (!SW3)
{
while ( i!=30 )
{
value = sine_rect_table[i++];
DACR = ( (1<<16) | (value<<6) );
delay_ms(1);
}
i=0;
}
else if ( !SW4) /* If switch for triangular wave is pressed */
{
value = 0;
while ( value != 1023 )
{
DACR = ( (1<<16) | (value<<6) );
value++;
}
while ( value != 0 )
{
DACR = ( (1<<16) | (value<<6) );
value--;
}
}
else if ( !SW5 ) /* If switch for sawtooth wave is pressed */
{
value = 0;
while ( value != 1023 )
{
DACR = ( (1<<16) | (value<<6) );
value++;
}
}
else if ( !SW6 ) /* If switch for square wave is pressed */
{
value = 1023;
DACR = ( (1<<16) | (value<<6) );
delay_ms(1);
value = 0;
DACR = ( (1<<16) | (value<<6) );
delay_ms(1);
}
else /* If no switch is pressed, 3.3V DC */
{
value = 1023;
DACR = ( (1<<16) | (value<<6) );
}
}
}
void SystemInit(void)
{
PLL0CON = 0x01;
PLL0CFG = 0x24;
PLL0FEED = 0xAA;
PLL0FEED = 0x55;
while( !( PLL0STAT & PLOCK ))
{ ; }
PLL0CON = 0x03;
PLL0FEED = 0xAA;
PLL0FEED = 0x55;
}
void delay_ms(unsigned int j)
{
unsigned int x,i;
for(i=0;i < j;i++)
{
for(x=0; x<10000; x++);
}
}
