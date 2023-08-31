#include <lpc214x.h>
#define IS_ON(pin)(!(IO0PIN & (1U << (pin))))

void delay_ms(unsigned int x);
void reset_values(int y);

int up = 0;
int down = 99;
unsigned int rshift = 1U<<7;
unsigned int lshift = 1;
const int key0 = 16;
const int key1= 17;
const int key2 = 18;
const int key3= 19;

int main()
{
	IO0DIR = (0xff)<<16;
	
	while(1)
	{
		if(IS_ON(key0))
		{
			reset_values(0);
			IO0CLR = 0xff << 16;
			IO0SET |=((up/10)<<4 | (up%10)) << 16;
			up++;
			if(up>99)
				up=0;
		}
		else if(IS_ON(key1))
		{
			reset_values(1);
			IO0CLR = 0xFF << 16;
			IO0SET |=((down/10)<<4 | (down%10)) << 16;
			down--;
			if(down<0)
				down=99;
		}
		else if(IS_ON(key2))
		{
			reset_values(2);
			IO0CLR = 0xFF << 16;
			IO0SET |=lshift << 16;
			lshift<<=1;
			if(lshift> 1U<<7)
				lshift=1;
		} 
		else if(IS_ON(key3))
		{
			reset_values(3);
			IO0CLR = 0xFF << 16;
			IO0SET |= rshift<< 16;
			rshift>>=1;
			if(rshift<1)
				rshift = 1U<<7;
		}
		delay_ms(1000);
	}
}

void reset_values(int y)
{
	switch(y)
	{
		case 0:
			down = 99;
		rshift = 1U<<7;
		lshift = 1;
		break;
		case 1:
			up = 0;
		rshift = 1U<<7;
		lshift = 1;
		break;
		case 2:
			up = 0;
		down=99;
		rshift = 1U<<7;
		break;
		case 3:
			up = 0;
		down=99;
		lshift = 1;
		break;
	}
}

void delay_ms(unsigned int ms){
	for(int i=0; i<ms; i++)
	for(int j= 0; j<10000; j++);
}
