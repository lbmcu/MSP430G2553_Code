#include <msp430.h> 
#include "stdint.h"

uint32_t currenttime = 40500;   //用来保存时间的变量，初值代表11:15:00
uint8_t flag = 0;
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
 * @fn:    void PrintTime(uint32_t time)
 * @brief: 初始化串口发送字符串
 * @para:  pbuff:指向要发送字符串的指针
 *         num:要发送的字符个数
 * @return:none
 * @comment: 初始化串口发送字符串
 */
void PrintTime(uint32_t time)
{
    uint8_t charbuff[] = {0,0,':',0,0,':',0,0,'\n'};
    charbuff[7] = (uint8_t)((time % 60) % 10) + '0';    //得到当前秒个位
    charbuff[6] = (uint8_t)((time % 60) / 10) + '0';    //得到当前秒十位
    charbuff[4] = (uint8_t)((time % 3600) / 60 % 10) + '0';    //得到当前分个位
    charbuff[3] = (uint8_t)((time % 3600) / 60 / 10) + '0';    //得到当前分十位
    charbuff[1] = (uint8_t)((time / 3600) % 10) + '0';    //得到当前时个位
    charbuff[0] = (uint8_t)(time / 3600 / 10) + '0';    //得到当前时十位
    UARTSendString("当前时间：",10);
    UARTSendString(charbuff,9);
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    /*配置DCO为1MHZ时钟*/
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;

    InitSystemClock();
    InitUART();

    P1DIR |= BIT0 ;

    /*配置时钟源为SMCLK*/
    TA1CTL |= TASSEL_2;
    /*设置工作模式为Up Mode*/
    TA1CTL |= MC_1;
    /*设置定时时间间隔*/
    TA1CCR0 = 49999;     //0.05s

    /*打开定时器TAIFG中断*/
    TA1CTL |= TAIE;
    /*打开全局中断*/
    __bis_SR_register(GIE);

    while(1)
    {
        if(flag == 1)
        {
            flag = 0;
            P1OUT ^= BIT0;
            PrintTime(currenttime);
        }
    }

    return 0;
}

#pragma vector = TIMER1_A1_VECTOR
__interrupt void Timer_Tick(void)
{
    static uint8_t cnt = 0;
    switch(TA1IV) //读取的话无需手动清零标志位
    {
    case 0x02:
        break;
    case 0x04:
        break;
    case 0x0A:
        cnt++;
        if(cnt == 20)
        {
            cnt = 0;
            flag = 1;   //1s时间到了
            currenttime ++; //时间加1
            currenttime %= 86400;   //一天24小时，防止溢出
        }
        break;
    default:
        break;
    }
}

