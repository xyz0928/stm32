#include "stm32f10x.h"
#include "delay.h"

//按钮初始化
void App_Button_Init(void);

//PC13初始化
void App_OnBoardLED_Init(void);

int main(void)
{
	App_Button_Init();
	
	App_OnBoardLED_Init();
	
	uint8_t current = Bit_SET;//按钮初始当前值，1高电压，按钮松开
	uint8_t previous = Bit_SET;//按钮初始上次值，1高电压，按钮松开
	while(1)
	{
		previous = current;
		current = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);//读取当前PA0的值
		if(current != previous)
			//按钮发生变化
		{
			if(current == Bit_SET)//当前是高电压1
				//从低到高变化，按钮松开
			//在按钮松开的时候改变LED状态
			{
				if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13) == Bit_SET)//1灭
					//读取上次输出数据寄存器的值
				{
					GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);//1->0亮
				}
				else//0亮 
				{
					GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);//0->1灭
				}
			}
			else//当前低电压0
				//从高到低变化，按钮按下
			{
			}
			Delay(10);//加10ms延迟，按钮消抖
		}
	}
}


void App_Button_Init(void)
	//PA0,输入上拉
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;//输入上拉
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}


void App_OnBoardLED_Init(void)
	//PC13,通用输出开漏
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	//GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);//初始PC13灭
}	


