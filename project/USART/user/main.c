#include "stm32f10x.h"
<<<<<<< HEAD
#include <stdio.h>//FILE
#include "delay.h"//GetTick
=======
>>>>>>> f1ed8b939c8cbafe93805d3be15d322b4ed113ac

/*
void USART_Init(串口名称，初始化参数)
struct USART_InitTypeDef//初始化串口
{
	uint32_t USART_BaudRate;//波特率,9600/115200/921600
	uint16_t USART_WordLength;//数据位长度
													 //USART_WordLength_8b: 8bit
													 //USART_WordLength_9b: 9bit
	uint16_ USART_StopBits;//停止位长度
	                      //USART_StopBits_0_5: 0.5bit
												//USART_StopBits_1: 1bit
												//USART_StopBits_1_5: 1.5bit
												//USART_StopBits_2: 2bit
	uint16_t USART_Parity;//校验方式
	                      //USART_Parity_No: 无校验
												//USART_Parity_Even: 偶校验
												//USART_Parity_Odd: 奇校验
<<<<<<< HEAD
	uint13_t USART_Mode;//数据收发方向
=======
	unit13_t USART_Mode;//数据收发方向
>>>>>>> f1ed8b939c8cbafe93805d3be15d322b4ed113ac
	                   //USART_Mode_Tx: 发数据
										 //USART_Mode_Rx: 收数据
										 //USART_Mode_Tx | USART_Mode_Rx: 收发双向
}
*/

<<<<<<< HEAD
/*
FlagStatus USART_GetFlagStatus(串口名称，查询的标志位)
查询USART的标志位的返回值，RESET=0,SET=1
*/

/*
void USART_SendData(串口名称，要发送的数据（无符号16bit位整型，2个字节）)
把要发送的数据写入发送数据寄存器中
*/

void My_USART_SendBytes(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size);
//定义一个函数，使用串口一次性发送多个字节
//参数：串口名称，要发送的数据（数组），发送字节数量

void My_USART1__Init(void);//定义初始化函数，初始化IO引脚和USART1串口

int main(void)
{
	Delay_Init();//调用函数，获取当前时间
	
	My_USART1__Init();//调用函数，初始化
	
//	//测试
//	uint8_t bytesSend[] = {1,2,3,4,5};//发送的数据
//	My_USART_SendBytes(USART1, bytesSend, 5);//根据Size输出多少
	
//	printf("hello world\r\n");
	// \r\n表示换行回车
	
	while(1)
	{
		uint32_t currentTick = GetTick();//获取当前时间，得到一个数字
		uint32_t miliSeconds = currentTick % 1000;//计算毫秒
		currentTick /= 1000;
		uint32_t seconds = currentTick % 60;//计算秒
		currentTick /= 60;
		uint32_t minutes = currentTick % 60;//计算分钟
		currentTick /= 60;
		uint32_t hours = currentTick;//计算小时
		printf("%02u:%02u:%02u.%03u\r\n",hours,minutes,seconds,miliSeconds);
		//%u无符号整型；%02u,2位不足用0补；%2u，2位不足用空格补
		
		Delay(100);//每100ms打印一次
	}
}

void My_USART_SendBytes(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size)
{
	for(uint32_t i = 0; i < Size; i++)
	{
		//1.等待发送数据寄存器为空
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		//查询TXE标志位，RESET=0,TXE=0,非空，一直循环，当变为SET=1时，空，进行下一步程序
		
		//2.要发送的数据写入发送数据寄存器中
		USART_SendData(USART1, pData[i]);
	}
	
	//3.等待数据发送完成
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	//查询TC标志位，TC=0=RESET,未完成；TC=1=SET,完成	
}

void My_USART1__Init(void)
{
//	//配置USART1引脚，默认在PA9,PA10,REMAP=0
//	//初始化USART1_Tx  PA9,复用推挽模式
=======
void My_USART_SendBytes(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size);
//定义一个函数，使用串口一次性发送多个字节
//参数：串口名称，要发送的数据（数组），发送字节数量 

int main(void)
{
	//	//配置USART1引脚，默认在PA9,PA10,REMAP=0
//	//USART1_Tx  PA9,复用推挽模式
>>>>>>> f1ed8b939c8cbafe93805d3be15d322b4ed113ac
//	GPIO_InitTypeDef GPIO_InitStruct;//定义PA9的IO引脚结构体指针
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//开启GPIOA时钟
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;//引脚9
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽模式
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;//最大输出速度10MHz
//	GPIO_Init(GPIOA, &GPIO_InitStruct);//初始化完成PA9
//	
<<<<<<< HEAD
//	//初始化USART1_Rx  PA10,输入上拉模式
=======
//	//USART_Rx  PA10,输入上拉模式
>>>>>>> f1ed8b939c8cbafe93805d3be15d322b4ed113ac
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
	
<<<<<<< HEAD
	//串口初始化
=======
>>>>>>> f1ed8b939c8cbafe93805d3be15d322b4ed113ac
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
<<<<<<< HEAD
}

int fputc(int ch, FILE *f)//重定义fputc，通过串口发送数据
{
	//1.等发送数据寄存器为空
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	
	//2.要发送的数据写入发送数据寄存器中
	USART_SendData(USART1, (uint8_t)ch);
	                      //强制转换成字节
	
	return ch;
}
=======

	uint8_t bytesSend[] = {1,2,3,4,5};//发送的数据
	My_USART_SendBytes(USART1, bytesSend, 5);//根据Size输出多少

/*
FlagStatus USART_GetFlagStatus(串口名称，查询的标志位)
查询USART的标志位的返回值，RESET=0,SET=1
*/

/*
void USART_SendData(串口名称，要发送的数据（无符号16bit位整型，2个字节）)
把要发送的数据写入发送数据寄存器中
*/

	while(1)
	{
	}
}

void My_USART_SendBytes(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size)
{
	for(uint32_t i = 0; i < Size; i++)
	{
		//1.等待发送数据寄存器为空
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		//查询TXE标志位，RESET=0,TXE=0,非空，一直循环，当变为SET=1时，空，进行下一步程序
		
		//2.要发送的数据写入发送数据寄存器中
		USART_SendData(USART1, pData[i]);
	}
	
	//3.等待数据发送完成
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	//查询TC标志位，TC=0=RESET,未完成；TC=1=SET,完成	
}


>>>>>>> f1ed8b939c8cbafe93805d3be15d322b4ed113ac
