#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    /*初始化LED2为输出*/
    P1DIR |= BIT6;

    /*初始化LED2为低电平，熄灭*/
    P1OUT &= ~BIT6;

    /*初始化KEY P1.3为输入*/
    P1DIR &= ~BIT3;
    /*使能P1.3口的上拉电阻*/
    P1REN |= BIT3;
    P1OUT |= BIT3;

    /*打开P1.3口的中断*/
    P1IE |= BIT3;
    /*设定为下降沿触发*/
    P1IES |= BIT3;
    /*清除中断标志位*/
    P1IFG &= ~BIT3;
    /*打开全局中断*/
    __bis_SR_register(GIE);

    while(1)
    {
    }
    return 0;
}

#pragma vector = PORT1_VECTOR
__interrupt void Port1_ISR(void)    //Port1_ISR(void)   中断服务函数
{
    if(P1IFG & BIT3)   //判断是否P1.3产生中断
    {
        P1OUT ^= BIT6;
        P1IFG  &= ~ BIT3;   //清除标志位
    }
}
