#include "stm32f10x.h"
#include "usart.h"
#include "delay.h"//GetTick()
#include <math.h>//sin
#include "delay.h"

void App_USART_Init(void);//初始化串口
void App_TIM3_Init(void);//初始化定时器3，用来产生PWM信号
void App_TIM1_Init(void);//初始化定时器1，测量PWM信号参数

int main(void)
{
	App_USART_Init();
	//My_USART_SendString(USART1, "你好\r\n");//测试串口，发送字符串
	
	App_TIM3_Init();
	TIM_SetCompare1(TIM3, 200);//ccr=200，200/1000=20%占空比
	//产生PWM信号，周期1000us，占空比20%
	
	App_TIM1_Init();
	
	while(1)
	{
		/*测试TIM3初始化，PA6接PC13实现呼吸灯效果
		float t = GetTick() * 1.0e-3f;//获取系统当前时间，ms*10^-3=s
		float duty = 0.5 * (sin(2 * 3.14 * t) + 1);//占空比
		uint16_t ccr1 = duty * 1000;//计算ccr1的值
		TIM_SetCompare1(TIM3, ccr1);//把ccr1的值设置到定时器3中
		*/
		
		//1.清除trigger标志位
		TIM_ClearFlag(TIM1, TIM_FLAG_Trigger);
		//2.等待Trigger标志位从0->1
		while(TIM_GetFlagStatus(TIM1, TIM_FLAG_Trigger) == RESET);
		//3.读取ccr1,ccr2值
		uint16_t ccr1 = TIM_GetCapture1(TIM1);
		uint16_t ccr2 = TIM_GetCapture2(TIM1);
		//4.计算PWM周期=ccr1*分辨率(1us)*10^3,单位毫秒
		float period = ccr1 * 1.0e-6f * 1.0e3f;
		//5.计算占空比=ccr2/ccr1*100%
		float duty = ((float)ccr2 / (float)ccr1) * 100.0f;
		//强制类型转换，否则结果为0
		//6.把结果发送到电脑
		My_USART_Printf(USART1, "周期=%.2fms, 占空比=%.2f%%\r\n", period, duty);
		Delay(100);//延迟100ms
	}
}


void App_USART_Init(void)
{
	//1.初始化IO引脚，Tx:PA9,AFPP，只需要使用串口向电脑发送数据
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct = {0};//结构体最好赋初值
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//2.初始化USART1
	//2.1开启USART时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	//2.2初始化USART1参数
	USART_InitTypeDef USART_InitStruct = {0};//结构体最好赋初值
	USART_InitStruct.USART_BaudRate = 115200;//波特率
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//不使用硬件流控
	USART_InitStruct.USART_Mode = USART_Mode_Tx;//发数据
	USART_InitStruct.USART_Parity = USART_Parity_No;//无校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//停止位
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//数据位
	USART_Init(USART1, &USART_InitStruct);
	
	//3.闭合USART开关
	USART_Cmd(USART1, ENABLE);
}


void App_TIM3_Init(void)
{
	//1.初始化时基单元
	//1.1开启TIM3时钟,在APB1总线上
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	//1.2配置时基单元参数
	TIM_TimeBaseInitTypeDef TIM3_InitStruct = {0};
	TIM3_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;//CNT上计数
	TIM3_InitStruct.TIM_Period = 999;//ARR
	TIM3_InitStruct.TIM_Prescaler = 71;//PSC
	//TIM3_InitStruct.TIM_RepetitionCounter = 0;//定时器3没有RCR，可以不写
	TIM_TimeBaseInit(TIM3, &TIM3_InitStruct);
	//1.3配置ARR寄存器预加载
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	//1.4闭合时基单元总开关
	TIM_Cmd(TIM3, ENABLE);
	
	//2.初始化TIM3_CH1:PA6,AFPP，输出比较通道1
	//2.1初始化PA6
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIOA6 = {0};
	GPIOA6.GPIO_Pin = GPIO_Pin_6;
	GPIOA6.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIOA6.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIOA6);
	//2.2初始化定时器3输出比较通道1的参数，通用定时器3没有互补通道
	TIM_OCInitTypeDef TIM_OC31InitStruct = {0};
	TIM_OC31InitStruct.TIM_OCMode = TIM_OCMode_PWM1;//模式PWM1
	TIM_OC31InitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//输出极性：高极性
	TIM_OC31InitStruct.TIM_OutputState = TIM_OutputState_Enable;//正常通道开关使能
	TIM_OC31InitStruct.TIM_Pulse = 0;//捕获/比较寄存器x(CCRx)的值
	TIM_OC1Init(TIM3, &TIM_OC31InitStruct);
	//2.3闭合MOE主输出使能开关
	TIM_CtrlPWMOutputs(TIM3, ENABLE);
	//2.4使能CCRx的预加载
	TIM_CCPreloadControl(TIM3, ENABLE);
}	


void App_TIM1_Init(void)
{
	//1.配置时基单元
	//1.1开启定时器1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	//1.2配置时基单元参数
	TIM_TimeBaseInitTypeDef TIM1_InitStruct = {0};
	TIM1_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;//CNT上计数
	TIM1_InitStruct.TIM_Period = 65535;//ARR,周期max，防止溢出
	TIM1_InitStruct.TIM_Prescaler = 71;//PSC,1us1个CNT
	TIM1_InitStruct.TIM_RepetitionCounter = 0;//RCR
	TIM_TimeBaseInit(TIM1, &TIM1_InitStruct);
	//1.3配置ARR寄存器预加载
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	//闭合时基单元总开关
	TIM_Cmd(TIM1, ENABLE);
	
	//2.初始化输入捕获,TIM1_CH1:PA8,IPD引脚输入
	//2.1初始化PA8,IPD
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIOA8 = {0};
	GPIOA8.GPIO_Pin = GPIO_Pin_8;
	GPIOA8.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIOA8);
	//2.2初始化输入捕获通道1,TI1FP1
	TIM_ICInitTypeDef TIM11_InitStruct = {0};
	TIM11_InitStruct.TIM_Channel = TIM_Channel_1;//通道1
	TIM11_InitStruct.TIM_ICFilter = 0;//不使用输入滤波
	TIM11_InitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;//捕获上升沿
	TIM11_InitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;//分频器1分频
	TIM11_InitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;//复用器信号选择：直接
	TIM_ICInit(TIM1, &TIM11_InitStruct);
	//2.3初始化输入捕获通道2
	TIM_ICInitTypeDef TIM12_InitStruct = {0};
	TIM12_InitStruct.TIM_Channel = TIM_Channel_2;//通道2
	TIM12_InitStruct.TIM_ICFilter = 0;//无滤波
	TIM12_InitStruct.TIM_ICPolarity = TIM_ICPolarity_Falling;//捕获下降沿
	TIM12_InitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;//1分频
	TIM12_InitStruct.TIM_ICSelection = TIM_ICSelection_IndirectTI;//信号选择间接
	TIM_ICInit(TIM1, &TIM12_InitStruct);
	
	//3.初始化从模式控制器
	//3.1从模式控制器触发输入
	TIM_SelectInputTrigger(TIM1, TIM_TS_TI1FP1);//TRGI的输入信号来源TI1FP1
	//3.2设置从模式控制器从模式TGRI
	TIM_SelectSlaveMode(TIM1,TIM_SlaveMode_Reset);//TRGI复位模式：TRGI上升沿对CNT清零
}	

