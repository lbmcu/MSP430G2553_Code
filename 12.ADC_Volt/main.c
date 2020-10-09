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
 * @fn:    void PrintFloat(float num)
 * @brief: 初始化串口发送浮点型数字
 * @para:  num：浮点型变量
 * @return:none
 * @comment: 初始化串口发送浮点型数字
 */
void PrintFloat(float num)
{
    uint8_t buff[] = {0,'.',0,0,0,'\n'};
    uint16_t temp = (uint16_t)(num * 1000);
    buff[0] = (uint8_t)(temp / 1000) + '0';
    buff[2] = (uint8_t)((temp % 1000) / 100) + '0';
    buff[3] = (uint8_t)((temp / 100) / 10) + '0';
    buff[4] = (uint8_t)(temp % 10) + '0';
    UARTSendString(buff,6);
}

/*
 * @fn:    void InitADC(void)
 * @brief: ADC初始化
 * @para:  none
 * @return:none
 * @comment: ADC初始化
 */
void InitADC(void)
{
    /*设置ADC时钟MCLK*/
    ADC10CTL1 |= ADC10SSEL_2;
    /*ADC 2分频*/
    ADC10CTL1 |= ADC10DIV_0;
    /*设置ADC基准源*/
    ADC10CTL0 |= SREF_1;
    /*设置ADC采样保持时间64CLK*/
    ADC10CTL0 |= ADC10SHT_3;
    /*设置ADC采样率200k*/
    ADC10CTL0 &= ~ADC10SR;
    /*ADC基准选择2.5V*/
    ADC10CTL0 |= REF2_5V;
    /*开启基准*/
    ADC10CTL0 |= REFON;
    /*选择ADC输入通道A0*/
    ADC10CTL1 |= INCH_0;
    /*允许A0模拟输入*/
    ADC10AE0 |= 0x0001;
    /*开启ADC*/
    ADC10CTL0 |= ADC10ON;
}

/*
 * @fn:    uint16_t GetADCValue(void)
 * @brief: 进行一次ADC转换并返回ADC转换结果
 * @para:  none
 * @return:ADC转换结果
 * @comment: ADC转换结果为10bit，以uint16_t类型返回，低10位有效数据
 */
uint16_t GetADCValue(void)
{
    /*开始转换*/
    ADC10CTL0 |= ADC10SC|ENC;
    /*等待转换完成*/
    while(ADC10CTL1 & ADC10BUSY);
    /*返回结果*/
    return ADC10MEM;
}

int main(void)
{
    float voltage = 0.0;
    uint16_t adcvalue = 0;
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	InitSystemClock();
	InitUART();
	InitADC();
	
	while(1)
	{
	    adcvalue = GetADCValue();
	    voltage = adcvalue * 2.5 / 1023;

	    UARTSendString("ADC10转接结果为：",17);
	    PrintNumber(adcvalue);
	    UARTSendString("相应电压值为：",14);
	    PrintFloat(voltage);
        __delay_cycles(300000);
	}
	return 0;
}
