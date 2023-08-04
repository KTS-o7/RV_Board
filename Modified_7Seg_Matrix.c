#include <lpc214x.h>
#define COL0 (IO1PIN & 1 << 19)
#define COL1 (IO1PIN & 1 << 18)
#define COL2 (IO1PIN & 1 << 17)
#define COL3 (IO1PIN & 1 << 16)

unsigned char lookup_table[4][4] = {{'h', 'e', 'l', 'o'},
                                    {'l', 'p', 'r', '7'},
                                    {'i', 'c', 'e', 'b'},
                                    {'*', '-', '+', ' '}};
unsigned char rowsel = 0, colsel = 0;
																		
/*void uart_init(void)
{
    // configurations to use serial port
    PINSEL0 |= 0x00000005; // P0.0 & P0.1 ARE CONFIGURED AS TXD0 & RXD0
    U0LCR = 0x83;          // 8 bits, no Parity, 1 Stop bit 
    U0DLM = 0;
    U0DLL = 8;    // 115200 baud rate
    U0LCR = 0x03; // DLAB = 0 
    U0FCR = 0x07; //  Enable and reset TX and RX FIFO. 
}	*/				
	
void delay_ms(unsigned int j)
{
    unsigned int x, i;
    for (i = 0; i < j; i++)
    {
        for (x = 0; x < 10000; x++)
            ; /* loop to generate 1 milisecond delay with CCLK = 60MHz */
    }
}

unsigned char program_5b()
{
   // SystemInit();
    //uart_init();                     // initialize UART0 port
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
            ;                                 // wait for key release
        delay_ms(50);                         // allow for key debouncing
        IO0SET = 0X000F0000;                  // disable all the rows
        return lookup_table[rowsel][colsel]; // send to serial port(check on the terminal)
    } while (1);
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
		case 'o':
				return 0xc0;
		case 'c':
				return 0xc6;
    // simmilarly add for other digit/characters
    default:
        break;
    }
    return 0xff;
}

void alphadisp7SEG(char *buf,int size)
{
    unsigned char i, j;
    unsigned char seg7_data, temp = 0;
    for (i = 0; i < size; i++) // because only 5 seven segment digits are present
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
				 IOSET0 |= 1 << 30; // IOSET0 | 0x40000000;
    delay_ms(1);       // nop();
    IOCLR0 |= 1 << 30; // IOCLR0 | 0x40000000;
				delay_ms(100);
    }
   
    return;
}

void program_2b()
{
    IO0DIR |= 1U << 31 | 1U << 19 | 1U << 20 | 1U << 30; // to set as o/ps
    //LED_ON;                                              // make D7 Led on .. just indicate the program is running
    //SystemInit();
    int i=0;
		char buf[11];
		for(i=0;i<11;i++)
						*(buf+i) = program_5b();
		
				
    while (1)
    {		
        alphadisp7SEG(buf,11);
        delay_ms(500);
        alphadisp7SEG("     ",5);
        //delay_ms(500);
    }
		
}


int main()
{
	program_2b();
	return 0;
}