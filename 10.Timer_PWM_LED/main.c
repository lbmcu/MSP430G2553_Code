#include <msp430.h> 

int main(void)
{
    unsigned int cnt = 0;
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	/*配置DCO为1MHZ时钟*/
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;

    /*初始化P1.6为输入*/
    P1DIR &= ~BIT6;

    /*设置时钟源为SMCLK*/
    TA1CTL |= TASSEL_2;
    /*设置工作模式为Up & Dowm*/
    TA1CTL |= MC_0 | MC_1;
    /*设置TA1CCR0为0x00ff*/
    TA1CCR0 = 0x00FF;
    /*设置TA1CCR2为0x00ff*/
    TA1CCR2 = 0x00FF;   //占空比 = （TACCR0 - TACCR2）/ TACCR0 频率 = SMCLK / (TACCR0+1)/2

    /*设置为比较模式*/
    TA1CCTL0 &= ~CAP;
    TA1CCTL2 &= ~CAP;
    /*设置比较输出模式*/
    TA1CCTL2 |= OUTMOD_6;
    /*设置IO复用*/
    P2SEL |= BIT5;
    P2DIR |= BIT5;

    while(1)
    {
        for(cnt = 0;cnt < 0x00FF; cnt++)
        {
           TA1CCR2 =  cnt;
           __delay_cycles(5000);
        }

        for(cnt = 0x00FF;cnt > 0; cnt--)
        {
           TA1CCR2 =  cnt;
           __delay_cycles(5000);
        }
    }

	return 0;
}
