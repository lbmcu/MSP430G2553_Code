#include <msp430.h> 
#include "stdint.h"
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


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    InitSystemClock();
    InitUART();

    while(1)
    {
        PrintNumber(23456);
        __delay_cycles(500000);
    }

    return 0;
}
