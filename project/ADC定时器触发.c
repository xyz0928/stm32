#include "stm32f10x.h"
#include "usart.h"

void App_USART1_Init(void);//串口初始化
void App_TIM1_Init(void);//初始化定时器1TRGO
void App_ADC1_Init(void);//初始化ADC1（注入序列）

int main(void)
{
	App_USART1_Init();
	//My_USART_SendString(USART1, "Hello World.\r\n");//测试串口初始化
	
	App_TIM1_Init();
	App_ADC1_Init();
	
	while(1)
	{
		//注入序列是由TIM1_TRGO触发，自动每1ms发送脉冲信号，不需要手动发送脉冲信号
		//1.等待注入序列转换完成
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_JEOC) == RESET);
		//查询JEOC注入序列转换完成标志位0->1，表示执行完成
		
		//2.读取JDR1寄存器的值，12位二进制数
		uint16_t jdr1 = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
		//注入序列的每一个通道对应一个JDR寄存器，共4对
		
		//3.清除JEOC标志位
		ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);
		
		//4.把结果转换成电压
		float voltage = jdr1 * (3.3f / 4095);
		//总电压3.3V，111111111111=4095
		//ADC分辨率=3.3/4095
		
		//5.通过串口把结果发送到电脑
		My_USART_Printf(USART1, "%0.2f\n", voltage);
	}
}


void App_USART1_Init(void)
{
	//1.初始化IO引脚
	//只使用Tx:PA9,AFPP，向电脑发送数据
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//2.开启USART时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//3.配置串口参数
	USART_InitTypeDef USART_InitStruct = {0};
	USART_InitStruct.USART_BaudRate = 115200;//波特率
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//不使用硬件流控
	USART_InitStruct.USART_Mode = USART_Mode_Tx;//发送数据
	USART_InitStruct.USART_Parity = USART_Parity_No;//无校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//停止位
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//数据位
	USART_Init(USART1, &USART_InitStruct);	
	
	//4.闭合USART总开关
	USART_Cmd(USART1, ENABLE);
}


void App_TIM1_Init(void)
{
	//1.开启定时器1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);//72MHz
	
	//2.配置时基单元的参数
	TIM_TimeBaseInitTypeDef  TIM1_InitStruct = {0};
	TIM1_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;//CNT上计数
	TIM1_InitStruct.TIM_Period = 999;//ARR,周期=1ms
	TIM1_InitStruct.TIM_Prescaler = 71;//PSC,1MHz=1us
	TIM1_InitStruct.TIM_RepetitionCounter = 0;//RCR
	TIM_TimeBaseInit(TIM1, &TIM1_InitStruct);
	//2.1开启ARR寄存器预加载
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	
	//3.从模式控制器的TRGO（主模式）设置成Update模式
	//选择TRGO来源
	TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);
	//每1ms产生一个Update事件，TRGO输出一个脉冲
	
	//4.开启定时器1总开关
	TIM_Cmd(TIM1, ENABLE);
}


void App_ADC1_Init(void)
{
	//1.初始化IO引脚，光敏传感器AO接ADC1通道0接PA0输入模拟模式
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIOA0 = {0};
	GPIOA0.GPIO_Pin = GPIO_Pin_0;
	GPIOA0.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIOA0);
	
	//2.配置ADC模块时钟
	//2.1设置ADC1时钟频率
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//6分频
	//72MHz/6=12MHz<14MHz
	//2.2使能ADC1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	//3.配置ADC的基本参数
	ADC_InitTypeDef ADC1_InitStruct = {0};
	//连续模式
	ADC1_InitStruct.ADC_ContinuousConvMode = DISABLE;//关闭连续模式
	//对齐方式
	ADC1_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
//	//选择常规序列的外部触发信号(注入序列用不到)
//	ADC1_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigInjecConv_None;//软件启动
	//双ADC模式
	ADC1_InitStruct.ADC_Mode = ADC_Mode_Independent;//独立模式
//	//常规序列的通道（注入序列用不到）
//	ADC1_InitStruct.ADC_NbrOfChannel = 1;//使用第一个通道（注入序列4个通道）
	//扫描模式
	ADC1_InitStruct.ADC_ScanConvMode = DISABLE;//不使用扫描模式
	ADC_Init(ADC1, &ADC1_InitStruct);
	
	//4.配置注入序列额外参数
	//4.1设置注入序列的长度
	ADC_InjectedSequencerLengthConfig(ADC1, 1);
	//4.2选择注入序列外部触发信号来源
	ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_TRGO);//来源TIM1的TRGO
	//4.3使能外部触发
	ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);//闭合注入序列外部触发开关
	
	//5.配置注入序列的通道
	//把ADC1通道0的数据写入到注入序列的第一行
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_13Cycles5);
	//参数：1.使用ADC模块的名称
	//      2.ADC的通道编号，使用通道0把光敏电阻的模拟输出输出到ADC中
	//      3.写入到注入序列的行号（1~4），写入到第一行
	//      4.采用时间，光敏传感器10.24cycle接近13.5cycle
	
	//6.闭合ADC总开关
	ADC_Cmd(ADC1,  ENABLE);
}

