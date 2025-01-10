#include "stm32f10x.h"
#include "delay.h"//Delay()函数头文件

int main(void)
{
	//1.开启GPIOx时钟
	//要开哪个时钟就开启对应的GPIOx，ENABLE是开启时钟的意思
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	//2.初始化IO引脚，PC13,通用输出开漏模式，最大输出速度2MHz
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };//定义结构体
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;//连接PC13引脚
	                                      //引脚编号GPIO_Pin_0...GPIO_Pin_15
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;//最大输出速度2MHz,还可以根据需要换成5\10MHz
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;//模式选择：通用输出开漏
													  //GPIO_Mode_Out_PP：通用输出推挽
													  //GPIO_Mode_AF_PP：复用输出推完
													 //GPIO_Mode_AF_OD：复用输出开漏
	GPIO_Init(GPIOC, &GPIO_InitStruct);//初始化IO引脚，自动写0，灯亮
	
//	//向输出寄存器写入
//	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);//写1，灯灭
//	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);//写0，灯亮
	
	while(1)//闪烁
	{
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);//写0，亮
		Delay(100);//延迟100ms
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);//写1，灭
		Delay(100);
	}
}
