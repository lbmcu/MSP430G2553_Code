/*
 * LED1绿灯——P1.0 LED2红灯——P1.6
*/
#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	/*将P1.0设置为输出，P1DIR是P1口的方向寄存器
	 * PxDIR     0——输入，1——输出*/

	P1DIR |= BIT0 | BIT6;
	
	/*将P1.0设置为，P1DIR是P1口的写寄存器
	 * PxOUT    0——低电平，1——高电平
	 * PxIN     读寄存器*/

	/*打开LED*/
    //P1OUT |= BIT0 | BIT6;

	/*关闭LED*/
	P1OUT &= ~BIT0 & ~BIT6;

	while(1)
	{
	    /*闪烁*/
	    P1OUT ^= BIT0 ^ BIT6;
	    __delay_cycles(500000);
	}
	return 0;
}
