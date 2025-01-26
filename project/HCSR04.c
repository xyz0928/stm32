#include "stm32f10x.h"
#include "usart.h"
#include "delay.h"

void App_USART1_Init(void);//初始化串口
void App_HCSR04_Init(void);//初始化HCSR04传感器

int main(void)
{
	App_USART1_Init();
	//My_USART_Printf(USART1, "Hello world\r\n");//测试
	
	App_HCSR04_Init();
	
	while(1)
	{
		//1.向CNT写0
		TIM_SetCounter(TIM1, 0);//对计数器CNT清零
		
		//2.清除cc1,cc2标志位
		TIM_ClearFlag(TIM1, TIM_FLAG_CC1);
		TIM_ClearFlag(TIM1, TIM_FLAG_CC2);
		
		//3.开启定时器TIM1
		TIM_Cmd(TIM1, ENABLE);
		
		//4.向传感器Trig引脚发送>10us的脉冲
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);//写1，高电压
		DelayUs(10);//延迟10us
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET);//写0，低电压
		
		//5.等待测量完成
		//获取CC1,CC2标志位状态
		while(TIM_GetFlagStatus(TIM1, TIM_FLAG_CC1) == RESET);
		while(TIM_GetFlagStatus(TIM1, TIM_FLAG_CC2) == RESET);
		
		//6.计算距离
		//读取CCR1的值
		uint16_t ccr1 = TIM_GetCapture1(TIM1);
		//读取CCR2的值
		uint16_t ccr2 = TIM_GetCapture2(TIM1);
		//计算Echo脉宽=(ccr2-ccr1)*分辨率=传播时间
		//分辨率=CNT的每一小格的时间=72MHz/(71+1)=1MHz=1us
		float pulse_wide = (ccr2 - ccr1) * 1.0e-6f;//秒
		//距离
		float distance = pulse_wide * 340.0f / 2;//米
		//打印结果
		My_USART_Printf(USART1, "%.3f\n", distance);
		Delay(100);//延迟100ms
	}
}


void App_USART1_Init(void)
	//Tx:PA9 AFPP, Rx:PA10 IPU, REMAP=0
{
	//1.初始化IO引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//2.初始化USAER1
	//2.1开启USART1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	//2.2初始化USAER1参数
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = 115200;//波特率
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;//收发双向
	USART_InitStruct.USART_Parity = USART_Parity_No;//无校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//停止位
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//数据位
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//不使用硬件流控
	USART_Init(USART1, &USART_InitStruct);
	
	//3.闭合USART总开关
	USART_Cmd(USART1, ENABLE);
}


void App_HCSR04_Init(void)
{
	//1.初始化时基单元
	//1.1开启定时器TIM1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	//1.2配置时基单元参数
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;//CNT,上计数
	TIM_InitStruct.TIM_Period = 65535;//ARR
	TIM_InitStruct.TIM_Prescaler = 71;//PSC
	TIM_InitStruct.TIM_RepetitionCounter = 0;//RCR
	TIM_TimeBaseInit(TIM1, &TIM_InitStruct);
	
	//2.初始化输入捕获
	//2.1初始化IO引脚，PA8, IPD如果引脚意外断开，会提供默认的低电压，模拟空闲
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitA;
	GPIO_InitA.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitA.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitA);
	//2.2初始化输入捕获通道1
	TIM_ICInitTypeDef TIM_CH1;
	TIM_CH1.TIM_Channel = TIM_Channel_1;//通道编号
	TIM_CH1.TIM_ICFilter = 0;//不使用输入滤波
	TIM_CH1.TIM_ICPolarity = TIM_ICPolarity_Rising;//通道1捕获的边沿：上升沿
	TIM_CH1.TIM_ICPrescaler = TIM_ICPSC_DIV1;//分频器的分频系数1
	TIM_CH1.TIM_ICSelection = TIM_ICSelection_DirectTI;//分频器前的大复用器选择：直接
	TIM_ICInit(TIM1, &TIM_CH1);
	//2.3初始化输入捕获通道2
	TIM_CH1.TIM_Channel = TIM_Channel_2;//通道编号
	TIM_CH1.TIM_ICFilter = 0;//不使用输入滤波
	TIM_CH1.TIM_ICPolarity = TIM_ICPolarity_Falling;//通道2捕获的边沿：下降沿
	TIM_CH1.TIM_ICPrescaler = TIM_ICPSC_DIV1;//分频器的分频系数1
	TIM_CH1.TIM_ICSelection = TIM_ICSelection_IndirectTI;//分频器前的大复用器选择：间接
	TIM_ICInit(TIM1, &TIM_CH1);
	
	//3.初始化Trig引脚PA0,OUT_PP
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

