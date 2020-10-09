#include <msp430.h> 
#include "stdint.h"

float freq = 0;
float capvalue_1 = 0;
float capvalue_2 = 0;
float timestamp_1 = 0;
float timestamp_2 = 0;
float timestamp = 0;
float totaltime = 0;


/*
 * @fn:    void InitSystemClock(void)
 * @brief: 初始化系统时钟
 * @para:  none
 * @return:none
 * @comment: 初始化系统时钟
 */
void InitSystemClock(void)
{
    /*配置DCO为1MHZ时钟*/
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;

    /*配置SMCLK的时钟源为DCO*/
    BCSCTL2 &= ~SELS;
    /*SMCLK的分频系数置为1*/
    BCSCTL2 &= ~(DIVS0 | DIVS1);
}

/*
 * @fn:    void InitUART(void)
 * @brief: 初始化串口，包括设置波特率，数据位，校验位等
 * @para:  none
 * @return:none
 * @comment: 初始化串口
 */
void InitUART(void)
{
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

    /*接收中断启用*/
    IE2 |= UCA0RXIE;
    /*清空接收中断标志*/
    IFG2 &= ~UCA0RXIFG;
}

/*
 * @fn:    void UARTSendString(uint8_t *pbuff, uint_8 num)
 * @brief: 初始化串口发送字符串
 * @para:  pbuff:指向要发送字符串的指针
 *         num:要发送的字符个数
 * @return:none
 * @comment: 初始化串口发送字符串
 */
void UARTSendString(uint8_t *pbuff, uint8_t num)
{
    uint8_t cnt = 0;
    for(cnt = 0; cnt < num; cnt++)
    {
        /*判断是否有数据正在发送*/
        while(UCA0STAT & UCBUSY);
        UCA0TXBUF = *(pbuff + cnt);
    }
}

/*
 * @fn:    void PrintFreq(float freq)
 * @brief: 通过串口发送频率
 * @para:  freq:频率
 * @return:none
 * @comment: 通过串口发送频率
 */
void PrintFreq(float freq)
{
    uint32_t temp = (uint32_t)(freq * 1000);
    uint8_t charbuff[] = {0,0,0,0,0,0,0,0,0};   //最大为999999.999Hz
    int8_t cnt = 0;
    for(cnt = 8;cnt >= 0; cnt--)
    {
        charbuff[cnt] = (uint8_t)(temp % 10) + '0';
        temp /= 10;
    }
    UARTSendString("频率为：",8);
    UARTSendString(charbuff,6);
    UARTSendString(".",1);
    UARTSendString(charbuff + 6, 3);
    UARTSendString("Hz\n",4);
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    InitSystemClock();
    InitUART();

    /*设置时钟源为SMCLK*/
    TA1CTL |= TASSEL_2;
    /*设置工作模式为Up Mode*/
    TA1CTL |= MC_1;
    /*设置时间间隔*/
    TA1CCR0 = 49999;
    /*开启TAIFG中断*/
    TA1CTL = TAIE;

    /*TA1CCR2用于捕捉功能*/
    TA1CCTL2 |= CAP;
    /*上升沿捕捉*/
    TA1CCTL2 |= CM0;
    /*P2.5作为捕捉输入(CCIS2B)*/
    TA1CCTL2 |= CCIS0;
    P2SEL |= BIT5;
    /*允许捕捉比较中断*/
    TA1CCTL2 |= CCIE;

    /*设置时钟源为SMCLK*/
    TA0CTL |= TASSEL_1;
    /*设置工作模式为Up & Dowm*/
    TA0CTL |= MC_0 | MC_1;
    /*设置TA1CCR0为0x00ff*/
    TA0CCR0 = 0x0AAA;
    /*设置TA1CCR2为0x00ff*/
    TA0CCR1 = 0x0555;   //占空比 = （TACCR0 - TACCR2）/ TACCR0 频率 = SMCLK / (TACCR0+1)/2

    /*设置为比较模式*/
    TA0CCTL0 &= ~CAP;
    TA0CCTL1 &= ~CAP;
    /*设置比较输出模式*/
    TA0CCTL1 |= OUTMOD_6;
    /*设置IO复用*/
    P1SEL |= BIT6;
    P1DIR |= BIT6;

    /*打开总中断*/
    __bis_SR_register(GIE);
    while(1)
    {
        __delay_cycles(500000);
        freq = (float)(1000000.0) / totaltime;
        PrintFreq(freq);
    }

    return 0;
}

#pragma vector = TIMER1_A1_VECTOR
__interrupt void Timer_Tick(void)
{
    static uint8_t cnt = 0;
    __bis_SR_register(GIE);//允许中断嵌套
    switch(TA1IV) //读取的话无需手动清零标志位
    {
    case 0x02:  //捕捉比较中断1
        break;
    case 0x04:  //捕捉比较中断2
        if(cnt == 0)
        {
            capvalue_1 = TA1CCR2;   //保存第一次捕捉值
            timestamp_1 = timestamp;    //保存第一次时间戳
            cnt++;
        }
        else
        {
            capvalue_2 = TA1CCR2;   //保存第二次捕捉值
            timestamp_2 = timestamp;    //保存第二次时间戳
            cnt = 0;
            totaltime = (timestamp_2 - timestamp_1) * 50000 + capvalue_2 - capvalue_1; //计算总时间
        }
        break;
    case 0x0A:  //溢出中断
        timestamp ++;
        break;
    default:
        break;
    }
}
