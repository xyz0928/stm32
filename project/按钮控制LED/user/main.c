#include "stm32f10x.h"

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//开启时钟，A模块
	
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };//声明变量
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;//引脚PA0，接LED
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;//最大输出速度
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;//输出模式：通用推挽
	GPIO_Init(GPIOA, &GPIO_InitStruct);//初始化PA0
	
	//接按钮控制，输入上拉模式，接到PA1和地
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;//按钮接引脚1
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;//输入上拉模式
	                                    //IPD:输入下拉
	                                    //_IN_FLOATING:输入浮空
	                                    //AIN:模拟模式
	GPIO_Init(GPIOA, &GPIO_InitStruct);//调用GPIO_Init(),完成初始化PA1
	
	//写，测试LED
	//GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);//PA0写1，点亮LED
	
	while(1)
	{
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == Bit_RESET)//PA1按钮按下
		{
			GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);//PA0写1，LED灯点亮
		}
		else//松开按钮
		{
			GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET);//PA0写0，LED灭
		}
	}
}
