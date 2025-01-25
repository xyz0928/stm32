#include "stm32f10x.h"
#include "delay.h"//GetTick():ms
#include "math.h"//sin

void App_PWM_Init(void);//初始化PWM波的产生

int main(void)
{
	App_PWM_Init();
	
	while(1)
	{
		//获取系统当前时间
		float t = GetTick() * 1.0e-3f;//ms*10^-3=s
		
		//设置占空比duty
		float duty = 0.5*(sin(2*3.14 * t) + 1);
		//sin(2pai*t)周期1s，频率1Hz
		//LED亮度范围在0（全暗）~100%=1（全亮），所以*0.5
		
		//计数CCR1值
		uint16_t ccr1 = duty * 1000;
		//ccr1=占空比*周期ms
		//设置CCR1值
		TIM_SetCompare1(TIM1, ccr1);
	}
}


void App_PWM_Init(void)
{
	//1.初始化IO引脚
	//不重映射，TIM1_CH1:PA, TIM1_CH1N:PB13, AFPP
	//假设PWM周期T=1ms=1kHz, 最大输出速度选2MHz
	//PA8
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitA;
	GPIO_InitA.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitA.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitA.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitA);
	
	//PB13
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitB;
	GPIO_InitB.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitB.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitB.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitB);
	
	//2.初始化时基单元
	//TIM1在APB2总线上，频率72MHz，经过PSC变1MHz=1us，ARR=999，CNT计1000us=1ms溢出
	//2.1开启定时器TIM1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	//2.2配置时基单元参数
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 71;//PSC=71
	TIM_TimeBaseInitStruct.TIM_Period = 999;//ARR=999
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;//RCR=0
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//计数方向：上计数
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);
	//2.3配置ARR寄存器的预加载
	//ARR需要手动打开，PSC,RCR默认开启
	TIM_ARRPreloadConfig(TIM1, ENABLE);//使能
	//2.4闭合TIM1开关
	TIM_Cmd(TIM1, ENABLE);
	
	//3.初始化输出比较
	//3.1初始化输出比较通道1的参数
	TIM_OCInitTypeDef TIM_OC1InitStruct;
	//输出比较工作模式
	TIM_OC1InitStruct.TIM_OCMode = TIM_OCMode_PWM1;//PWM1模式
	//正常输出极性
	TIM_OC1InitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//高极性
	//互补输出极性
	TIM_OC1InitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;//高极性
	//正常通道开关输出使能
	TIM_OC1InitStruct.TIM_OutputState = TIM_OutputState_Enable;
	//互补通道开关输出使能
	TIM_OC1InitStruct.TIM_OutputNState = TIM_OutputNState_Enable;
	//设置输出/捕获寄存器CCRx的初始值
	TIM_OC1InitStruct.TIM_Pulse = 0;
	TIM_OC1Init(TIM1, &TIM_OC1InitStruct);
	//开启CCR寄存器预加载
	TIM_CCPreloadControl(TIM1, ENABLE);
	//3.2闭合MOE开关
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

