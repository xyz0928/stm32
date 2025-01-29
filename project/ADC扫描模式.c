#include "stm32f10x.h"
#include "usart.h"

void App_USART1_Init(void);//串口初始化
void App_TIM1_Init(void);//初始化定时器1TRGO来触发ADC注入序列
void App_ADC1_Init(void);//初始化ADC1(注入序列)

int main(void)
{
	App_USART1_Init();
	//My_USART_SendString(USART1, "HELLO.");//测试串口
	
	App_TIM1_Init();
	
	App_ADC1_Init();
	
	while(1)
	{
		//注入序列是由TIM1_TRGO触发，自动每1ms发送脉冲信号，不需要手动发送脉冲信号
		//1.等待注入序列转换完成
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_JEOC) == RESET);
		//JEOC注入序列转换完成标志位0->1,表示转换完成
		
		//2.读取jdr1,jdr2的值
		uint16_t jdr1 = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
		//读取JDR1寄存器中的数据
		uint16_t jdr2 = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_2);
		//读取JDR2寄存器中的数据
		
		//3.清除JEOC标志位
		ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);
		
		//4.把结果转换成电压
		float voltage1 = jdr1 * (3.3f / 4095);
		float voltage2 = jdr2 * (3.3f / 4095);
		
		//5.把结果通过串口发送到电脑
		My_USART_Printf(USART1, "%.2f,%.2f\n", voltage1, voltage2);
	}
}


void App_USART1_Init(void)
{
	//1.初始化IO引脚,PA9,AFPP
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//2.开启USART时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//3.配置串口参数
	USART_InitTypeDef USART1_InitStruct = {0};
	USART1_InitStruct.USART_BaudRate = 115200;//波特率
	USART1_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//不使用硬件流控
	USART1_InitStruct.USART_Mode = USART_Mode_Tx;//串口向电脑发送数据
	USART1_InitStruct.USART_Parity = USART_Parity_No;//无校验
	USART1_InitStruct.USART_StopBits = USART_StopBits_1;//停止位
	USART1_InitStruct.USART_WordLength = USART_WordLength_8b;//数据位
	USART_Init(USART1, &USART1_InitStruct);
	
	//4.闭合USART总开关
	USART_Cmd(USART1, ENABLE);
}


void App_TIM1_Init(void)
{
	//1.开启定时器1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);//72MHz
	
	//2.配置时基单元参数
	TIM_TimeBaseInitTypeDef TIM1_InitStruct = {0};
	TIM1_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;//CNT上计数
	TIM1_InitStruct.TIM_Period = 999;//ARR,周期=1ms溢出一次，触发一次update事件
	TIM1_InitStruct.TIM_Prescaler = 71;//PSC，1MHz=1us计1个CNT
	TIM1_InitStruct.TIM_RepetitionCounter = 0;//RCR
	TIM_TimeBaseInit(TIM1,&TIM1_InitStruct);
	//2.1开启ARR预加载
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	
	//3.讲TRGO设置成Update模式
	TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);
	//每1ms产生一个Update事件，TRGO输出一个脉冲
	
	//4.闭合TIM1总开关
	TIM_Cmd(TIM1, ENABLE);
}


void App_ADC1_Init(void)
{
	//1.初始化PA0,PA1
	//2个电位器，中间2号引脚接AO接IO引脚，模拟输入
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIOA01 = {0};
	GPIOA01.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIOA01.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIOA01);
	
	//2.配置ADC模块时钟
	//2.1设置ADC1时钟频率
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//6分频
	//72MHz/6=12MHz<14MHz
	//2.2使能ADC1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	//3.配置ADC基本参数
	ADC_InitTypeDef ADC1_InitStruct = {0};
	//连续模式
	ADC1_InitStruct.ADC_ContinuousConvMode = DISABLE;//关闭
	//对齐方式
	ADC1_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
	//双ADC模式
	ADC1_InitStruct.ADC_Mode = ADC_Mode_Independent;//独立模式
//	//常规序列通道（注入序列用不到） 
//	ADC1_InitStruct.ADC_NbrOfChannel = 1;//使用1通道
	//扫描模式
	ADC1_InitStruct.ADC_ScanConvMode = ENABLE;//开启扫描模式
	ADC_Init(ADC1, &ADC1_InitStruct);
	
	//4.配置注入序列的额外参数
	//4.1设置注入序列的长度
	ADC_InjectedSequencerLengthConfig(ADC1, 2);//注入序列前2行
	//4.2注入序列的外部触发信号来源
	ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_TRGO);
	//4.3使能注入序列的外部触发开关
	ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);
	//4.4把ADC1通道0的数据写入到注入序列的第一行
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_13Cycles5);
	//4.5把ADC1通道1的数据写入到注入序列的第二行
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_13Cycles5);
	//参数：1.使用ADC模块的名称
	//      2.ADC的通道编号，使用通道0、1把电阻器的模拟输出输出到ADC中
	//      3.写入到注入序列的行号（1~4），写入到第一、二行
	//      4.采用时间，电阻器10.24cycle接近13.5cycle
	
	//5.闭合ADC总开关
	ADC_Cmd(ADC1, ENABLE);
}	

