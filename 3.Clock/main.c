#include <msp430.h> 

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    /*设置MCLK频率1,8,12,16*/
    DCOCTL = CALDCO_16MHZ;
    BCSCTL1 = CALBC1_16MHZ;

    /*P1.6设置为输出*/
    P1DIR |= BIT0 | BIT6;
    /*打开LED*/
    //P1OUT |= BIT0 | BIT6;
    /*关闭LED*/
    P1OUT &= ~BIT0 & ~BIT6;

    while(1)
    {
        P1OUT ^= BIT0 ^ BIT6;
        __delay_cycles(500000);
    }
    return 0;
}
