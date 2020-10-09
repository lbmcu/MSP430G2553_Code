#include <msp430.h> 
#include "stdint.h"

uint8_t combuff[20] = {0};  //长度为20的命令缓冲区，用于保存串口接收到的命令
uint8_t iscomend = 0;   //命令结束标志位
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
 * @fn:    void PrintNumber(uint16_t num)
 * @brief: 初始化串口发送数字
 * @para:  num：变量
 * @return:none
 * @comment: 初始化串口发送数字
 */
void PrintNumber(uint16_t num)
{
    uint8_t cnt = 0;
    uint8_t buff[6] = {0,0,0,0,0,'\n'};

    for(cnt = 0; cnt < 5; cnt++)
    {
        buff[4 - cnt] = (uint8_t)(num % 10 + '0');
        num /= 10;
    }
    UARTSendString(buff,6);
}

/*
 * @fn:    void Execute(uint8_t *combuff)
 * @brief: 串口命令执行函数
 * @para:  combuff：指向串口命令缓冲区的指针
 * @return:none
 * @comment: 串口命令执行函数
 */
void Execute(uint8_t *combuff)
{
    const uint8_t charbuff[5][10] = {"王龙","米雷龙","班长","LED1 ON!","LED1 OFF!"};
    if(combuff[0] == charbuff[0][0] && combuff[1] == charbuff[0][1])
    {
        UARTSendString("aa",2);
    }
    else if(combuff[0] == charbuff[1][0] && combuff[1] == charbuff[1][1])
    {
        UARTSendString("bb",2);
    }
    else if(combuff[0] == charbuff[2][0] && combuff[1] == charbuff[2][1])
    {
        UARTSendString("cc",2);
    }
    else if(combuff[0] == charbuff[3][0] && combuff[6] == charbuff[3][6])
    {
        UARTSendString("Yes,LED ON!",11);
        P1OUT |= BIT0;
    }
    if(combuff[0] == charbuff[4][0] && combuff[6] == charbuff[4][6])
    {
        UARTSendString("Yes,LED OFF!",12);
        P1OUT &= ~BIT0;
    }
}


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    InitSystemClock();
    InitUART();
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;
    __bis_SR_register(GIE);//打开总中断
    while(1)
    {
       if(iscomend)
       {
           iscomend = 0; //清除标志位，防止重复执行
           Execute(combuff);
       }
    }

    return 0;
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void UART_Receive_ISR(void)    //Port1_ISR(void)   中断服务函数
{
    static uint8_t cnt = 0;
    if(IFG2 & UCA0RXIFG)//检测是否是USCI_AO的接收中断，USCI_AO和USCI_BO的接收中断共享同一向量
    {

        IFG2 &= ~UCA0RXIFG; //清空接收中断标志
        combuff[cnt++] = UCA0RXBUF; //保存命令
        cnt %= 20;  //防止cnt大于20，导致缓存区溢出
        if(combuff[cnt - 1] == '\n')
        {
            cnt = 0;    //复位计数器
            iscomend = 1;   //命令接收完毕标志
        }
    }
    /*清空接收中断标志*/
    IFG2 &= ~UCA0RXIFG;
}

