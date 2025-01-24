#include "stm32f10x.h"

void App_OnBoardLED_Init(void);//初始化板载LED

void App_Button_Init(void);//初始化按钮

int main(void)
{
	//中断优先级分组，对所有中断都有效
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	App_OnBoardLED_Init();
	App_Button_Init();
	
	while(1)
	{
	}
}


void App_OnBoardLED_Init(void)
	//PC13
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	GPIO_WriteBit(GPIOC,GPIO_Pin_13, Bit_SET);//灭
}


void App_Button_Init(void)
	//2个按钮，接PA5右,PA6左，输入上拉
{
	//1.初始化PA5,PA6
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	//按钮按下，IO引脚接地为低电压
	//按钮松开，IO引脚悬空，上拉电阻作用下IO引脚高电压
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//2.为EXTI5,EXTI6分配引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	//使用AFIO模块为EXTI的线选择引脚
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource5);//PA5选作线5的输入信号
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource6);//PA6选作线6的输入信号
	
	//3.初始化EXTI的线，无需使能EXTI模块的时钟
	EXTI_InitTypeDef  EXTI_InitStruct;//声明变量
	EXTI_InitStruct.EXTI_Line = EXTI_Line5 | EXTI_Line6;//EXTI线（0~19）
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;//中断模式
														//EXTI_Mode_Event//事件模式
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;//触发边沿：上升沿
																//EXTI_Trigger_Falling//下降沿
																//EXTI_Trigger_Rising_Falling//双边沿
	//按钮按下为下降沿，松开为上升沿
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;//闭合中断模式开关
	EXTI_Init(&EXTI_InitStruct);//配置完成EXTI线5，6
	
	//4.配置中断
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;//中断名称，线5/6对应EXTI9_5中断
	//在"stm32f10x.h"
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级（）
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//子优先级
	//只有一个中断，2个优先级随便写
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//闭合中断开关
	NVIC_Init(&NVIC_InitStruct);	
}


//配置中断响应函数
//中断响应函数名称，在"startup_stm32f10x_md.s"
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line5) == SET)//线5触发中断，中断标志位0->1高电压
		//获取EXTI线的标志位的值
	{
		EXTI_ClearFlag(EXTI_Line5);//清除中断标志位（写0，1->0），停止中断否则会一直触发此中断
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);//亮
	}
	if(EXTI_GetFlagStatus(EXTI_Line6) == SET)//线6触发中断，中断标志位0->1高电压
	{
		EXTI_ClearFlag(EXTI_Line6);//清除中断标志位（写0，1->0），停止中断否则会一直触发此中断
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);//灭
	}
}


