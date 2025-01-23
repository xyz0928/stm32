#include "stm32f10x.h"
#include "delay.h"
//#include "usart.h"

void App_OnBoardLED_Init(void);//板载LED初始化

uint32_t blinkInterval = 1000;//闪灯间隔，1000ms

void App_USART1_Init(void);//USART1初始化

int main(void)
{
	//设置中断优先级分组（0~4）
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//分组2
	//抢占优先级2，子优先级2
	
	App_OnBoardLED_Init();
	
	App_USART1_Init();
	
	//My_USART_SendString(USART1, "HELLO WORLD");//测试
	
	while(1)
	{
		//闪灯
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);//亮
		Delay(blinkInterval);//延迟
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);//灭
		Delay(blinkInterval);
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
}	


void App_USART1_Init(void)
{
	//1.IO引脚,Tx:PA9,Rx:PA10
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//2.初始化USART1
	//开启USART1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = 115200;//波特率
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//8bit数据位
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//1位停止位
	USART_InitStruct.USART_Parity = USART_Parity_No;//无校验位
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;//收发双向
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//不使用硬件流控
	USART_Init(USART1, &USART_InitStruct);
	USART_Cmd(USART1, ENABLE);//闭合USART1总开关
	
	//3.配置中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//闭合USART中RXNE开关
	
	//4.配置NVIC
	NVIC_InitTypeDef NVIC_InitStruct;
	
	//中断名称，见"stm32f10x.h"->IRQn
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	
	//设置抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;//范围0~3
	//这次实验只有一个中断，所以抢占和子优先级随便写
	//设置子优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//范围0~3
	
	//设置中断开关状态
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//闭合开关
	
	NVIC_Init(&NVIC_InitStruct);
}


//中断响应函数，见"startup_stm32f10x_md.s"
void USART1_IRQHandler (void)
{
	if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)//1
		//USART中RXNE接收数据寄存器非空标志位，1-非空，有数据进行中断
	{
		uint8_t dataRcvd = USART_ReceiveData(USART1);//读取数据
		if(dataRcvd == '0')
		{
			blinkInterval = 1000;//慢闪
		}
		else if(dataRcvd == '1')
		{
			blinkInterval = 200;//中闪
		}
		else if(dataRcvd == '2')
		{
			blinkInterval =50;//快闪
		}
	}
}

