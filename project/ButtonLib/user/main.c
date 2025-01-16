#include "stm32f10x.h"
#include "usart.h"
#include "button.h"

void App_USART_Init(void);

Button_TypeDef ButtonStruct;//按钮本身(名称)，声明变量
void App_Button_Init(void);//初始化

uint32_t count;

void button_clicked_cb(uint8_t clicks);//点击
void button_long_pressed_cb(uint8_t ticks);//长按

int main(void)
{
	App_USART_Init();
	
//	My_USART_SendString(USART1, "Hello World\r\n");
//	//发生字符串测试
	
	App_Button_Init();
	
	while(1)
	{
		//进程函数
		My_Button_Proc(&ButtonStruct);
	}
}


void App_USART_Init(void)
	//Tx:PA9  Rx:PA10  REMAP=0
{
	//1.初始化IO引脚
	//PA9
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//复用输出推挽
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	//PA10
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;//输入上拉
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//2.初始化USART
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;//收发双向
	USART_InitStruct.USART_BaudRate = 115200;//波特率
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//数据位长度
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//停止位长度
	USART_InitStruct.USART_Parity = USART_Parity_No;//无校验
	USART_Init(USART1, &USART_InitStruct);
	USART_Cmd(USART1, ENABLE);//闭合USART1总开关
}


void App_Button_Init(void)
{
	Button_InitTypeDef Button_InitStruct;
	Button_InitStruct.GPIOx = GPIOA;
	Button_InitStruct.GPIO_Pin = GPIO_Pin_0;
	//长按的时间阈值(ms)，0表示默认（1000）
	Button_InitStruct.LongPressTime = 0;
	//按住1000ms=1s，认为是长按
	
	//长按后持续出发的时间间隔，0表示默认（100）
	Button_InitStruct.LongPressTickInterval = 0;
	//长按按钮后不松开，每隔100ms触发一次
	
	//连击的最大时间间隔，0表示默认（200）
	Button_InitStruct.ClickInterval = 0;
	//2次或多次按下的时间间隔<200ms，认为是连击
	
	//回调函数
	//按下回调函数
	Button_InitStruct.button_pressed_cb = 0;
	
	//松开回调函数
	Button_InitStruct.button_released_cb = 0;
	
	//点击回调函数
	Button_InitStruct.button_clicked_cb = button_clicked_cb;
	
	//长按回调函数
	Button_InitStruct.button_long_pressed_cb = button_long_pressed_cb;
	
	My_Button_Init(&ButtonStruct, &Button_InitStruct);
}


void button_clicked_cb(uint8_t clicks)
{
	if(clicks == 1)//单击按钮
	{
		count++;//数+1
		My_USART_Printf(USART1, "%d", count);//打印
	}
	else if(clicks == 2)//双击按钮
	{
		count = 0;//归零
		My_USART_Printf(USART1, "%d", count);
	}
}


void button_long_pressed_cb(uint8_t ticks)
{
	count++;
	My_USART_Printf(USART1, "%d", count);
}




