#include "stm32f10x.h"

volatile uint32_t currentTick = 0;//记录单片机当前事件(ms)

void App_Delay(uint32_t ms);//延迟函数，参数：延迟的时间

void App_TIM3_Init(void);//TIM3初始化（通用定时器3）

void App_OnBoardLED(void);

int main(void)
{
	//配置中断优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	App_TIM3_Init();
	App_OnBoardLED();
	
	while(1)
	{
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);//亮
		App_Delay(50);//延迟50ms
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);//灭
		App_Delay(50);
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);//亮
		App_Delay(500);//延迟500ms
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);//灭
		App_Delay(500);
	}
}


void App_Delay(uint32_t ms)
{
	uint32_t expireTime = currentTick + ms;//延迟结束时间
	while(currentTick < expireTime);//等待延迟结束
	//当前时间>延迟结束时间，循环结束
}


void App_TIM3_Init(void)
{
	//1.开启定时器3时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	//系统默认时钟频率最高PCLK1=HCLK/2=36MHz,所以TIM_CLK=PCLK1*2=72MHz
	
	//2.配置时基单元参数
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	//预分频器PSC
	TIM_InitStruct.TIM_Prescaler = 71;//PSC=71,1MHz=10^(-6)s=1us
	//自动重装寄存器ARR
	TIM_InitStruct.TIM_Period = 999;//ARR=999,1ms=1000us溢出一次
	//计数器的计数方向
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;//上计数
	                               //TIM_CounterMode_Down//下计数
	                               //TIM_CounterMode_CenterAligned1//中心对齐
	//重复计数器RCR
	TIM_InitStruct.TIM_RepetitionCounter = 0;//RCR=0,CNT每溢出一次产生update事件
	TIM_TimeBaseInit(TIM3, &TIM_InitStruct);
	
	//3.闭合时基单元开关
	TIM_Cmd(TIM3, ENABLE);
	
	//4.配置update中断
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	//使能定时器TIM3中update中断
	
	//5.配置NVIC模块
	NVIC_InitTypeDef NVIC_InitStruct;
	//中断名称
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//闭合中断开关
	NVIC_Init(&NVIC_InitStruct);
}


//配置中断响应函数
void TIM3_IRQHandler(void)
{
	if(TIM_GetFlagStatus(TIM3, TIM_FLAG_Update) == SET)
		//查询update标志位触发中断
	{
		TIM_ClearFlag(TIM3, TIM_FLAG_Update);//清除标志位
		currentTick++;
	}
}


void App_OnBoardLED(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);//灭
}

