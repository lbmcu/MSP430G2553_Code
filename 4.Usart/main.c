#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	/*配置DCO为1MHZ时钟*/
	DCOCTL = CALDCO_1MHZ;
	BCSCTL1 = CALBC1_1MHZ;

	/*配置SMCLK的时钟源为DCO*/
	BCSCTL2 &= ~SELS;
	/*SMCLK的分频系数置为1*/
	BCSCTL2 &= ~(DIVS0 | DIVS1);

	/*复位USCI_Ax*/
	UCA0CTL1 |= UCSWRST;

	/*设置为异步模式*/
	UCA0CTL0 &= ~UCSYNC;

	/*配置UART时钟源为SMCLK*/
	UCA0CTL1 |= UCSSEL1;

	/*配置波特率为9600*/
	UCA0BR0 = 0x68;
	UCA0BR1 = 0x00;
	UCA0MCTL = 1 << 1;

	/*配置端口,使能端口复用*/
	P1SEL   |= BIT1 + BIT2;
	P1SEL2  |= BIT1 + BIT2;

	/*清除复位位，使能UART*/
    UCA0CTL1 &= ~UCSWRST;

	while(1)
	{
	    UCA0TXBUF = 0xAA;
	    __delay_cycles(500000);
	}

	return 0;
}
