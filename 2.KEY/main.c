#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	/*初始化LED2为输出*/
	P1DIR |= BIT6;

	/*初始化LED2为低电平，熄灭*/
	P1OUT &= ~BIT6;

	/*初始化KEY P1.3为输入*/
	P1DIR &= ~BIT3;
	/*使能P1.3口的上拉电阻*/
	P1REN |= BIT3;
	P1OUT |= BIT3;

	while(1)
	{
	    if(P1IN & BIT3) /*如果P1.3口为高电平，证明按键没有被按下*/
	    {
	        P1OUT &= ~BIT6; //熄灭LED2
	    }
	    else
	    {
	        P1OUT |= BIT6;  //点亮LED2
	    }
	}
	return 0;
}
