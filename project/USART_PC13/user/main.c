#include "stm32f10x.h"

void My_USART1_Init(void);//定义初始化函数，初始化IO引脚和USART1串口

void My_OnBoardLED_Init(void);//定义函数初始化板载LED灯，PC13

int main(void)
{
	
	My_USART1_Init();//调用函数，初始化
	My_OnBoardLED_Init();

	while(1)
	{
		//1.等待接收数据寄存器RDR非空
		while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);//RXNE=0,空，一直循环，直到RXNE=1非空，进行后面程序
		
		//2.把数据从接收数据寄存器中读取
		uint8_t byteRcvd = USART_ReceiveData(USART1);
		
		//3.对数据进行处理
		if(byteRcvd == '0')//接收字符为0
		{
			GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);//灭灯，开漏接法1灭
		}
		else if(byteRcvd == '1')
		{
			GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);//亮灯
		}
	}
}


void My_OnBoardLED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;//声明结构体
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//开启GPIOC时钟
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;//通用输出开漏模式
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;//最大输出速度
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);//PC13写1，灭
}


void My_USART1_Init(void)
{
//	//配置USART1引脚，默认在PA9,PA10,REMAP=0
//	//USART1_Tx  PA9,复用推挽模式
//	GPIO_InitTypeDef GPIO_InitStruct;//定义PA9的IO引脚结构体指针
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//开启GPIOA时钟
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;//引脚9
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽模式
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;//最大输出速度10MHz
//	GPIO_Init(GPIOA, &GPIO_InitStruct);//初始化完成PA9
//
//	//USART_Rx  PA10,输入上拉模式
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//开启GPIOA时钟
//	GPIO_InitTypeDef GPIO_InitStruct1;//定义PA10的引脚结构体
//	GPIO_InitStruct1.GPIO_Pin = GPIO_Pin_10;
//	GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_IPU;//输入上拉模式
//	GPIO_Init(GPIOA, &GPIO_InitStruct1);

	//USART1引脚重映射，REMAP=1，到PB6,PB7
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//开启AFIO模块（变量外设）时钟
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);//USART重映射
	GPIO_InitTypeDef GPIO_InitStruct;//定义结构体指针
	
	//PB6，接USB TO TTL 的RXD
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//开启GPIOB时钟
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);//初始化完成PB6引脚
	
	//PB7，接USB TO TTL 的TXD
	//最后GND接GEN
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStruct);//初始化完成PB7引脚
	
	//串口初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//开启USART1时钟
	USART_InitTypeDef USART_InitStruct;//定义串口USART结构体指针
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;//双向
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//8位数据位
	USART_InitStruct.USART_Parity = USART_Parity_No;//无校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//1位停止位
	USART_Init(USART1, &USART_InitStruct);//串口初始化完成
	
	USART_Cmd(USART1, ENABLE);//USART模块使能
	                //DISABLE,禁止
}





