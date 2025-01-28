#include "stm32f10x.h"

void App_ADC1_Init(void);//对ADC1模块初始化
void App_OnBoardLED(void);//初始化板载LED

int main(void)
{
	App_ADC1_Init();
	
	App_OnBoardLED();
	
	while(1)
	{
		//1.清除EOC标志位（转换完成标志位）
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);//EOC写0
		
		//2.通过常规序列的软件启动方式发送脉冲
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		
		//3.等待常规序列转换完成
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);//0
		//EOC转换完成标志位0->1，表示常规序列执行完成
		
		//4.读取DR寄存器中的值
		//常规序列转换完成会把结果保存在DR寄存器中
		uint16_t dr = ADC_GetConversionValue(ADC1);//16bit二进制数，右对齐
		
		//5.把结果转换成电压
		float voltage = dr * (3.3f / 4095);
		//总电压3.3V，111111111111=4095
		//ADC分辨率=3.3/4095
		
		//6.根据电压值控制PC13亮灭
		//AO模拟输出的是光敏传感器一端的电压大小，光敏传感器光敏电阻R2与信号源内阻R1串联分压
		//光越强，R2越小，分到的电压越小，dr越小，voltage越小
		if(voltage > 1.5)//光弱
		{
			GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);//灭
		}
		else//光强
		{
			GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);//亮
		}
	}
}


void App_ADC1_Init(void)
{
	//1.初始化IO引脚，光敏传感器AO接ADC1通道1普通PA0引脚，PA0模拟模式
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;//模拟模式
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//2.配置ADC模块时钟
	//2.1设置ADC1模块时钟的分频系数
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//6分频
	//系统默认PCLK2=72MHz,ADC频率=72/6=12MHz<14MHz
	//2.2使能ADC1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	//3.初始化ADC的基本参数
	ADC_InitTypeDef ADC_InitStruct = {0};
	//连续模式
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;//关闭连续模式
	//对齐方式
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
															//0000 b11 b10...b1 b0，左对齐4个0在最右边
	//12位，但DR寄存器是16bit，所以会多出4个0进行左右对齐
	//选择常规序列的外部触发信号
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//软件启动
	//双ADC模式
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;//独立模式，AD1,AD2独立工作互不影响，不进行配合
	//常规序列的通道数
	ADC_InitStruct.ADC_NbrOfChannel = 1;//使用第1个通道
	//扫描模式
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;//不使用扫描模式
	ADC_Init(ADC1, &ADC_InitStruct);
	
	//4.配置常规序列
	//4.1配置常规序列通道
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_13Cycles5);
	//参数：1.使用ADC模块的名称
	//      2.ADC的通道编号，使用通道0把光敏电阻的模拟输出输出到ADC中
	//      3.写入到常规序列的行号（1~16），写入到第一行
	//      4.采用时间，光敏传感器10.24cycle接近13.5cycle
	//4.2闭合常规序列的外部触发开关
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);
	
	//5.闭合ADC总开关
	ADC_Cmd(ADC1, ENABLE);
}


void App_OnBoardLED(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIOC_Init = {0};
	GPIOC_Init.GPIO_Pin = GPIO_Pin_13;
	GPIOC_Init.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIOC_Init.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIOC_Init);
}


