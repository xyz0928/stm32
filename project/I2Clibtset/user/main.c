#include "stm32f10x.h"
#include "i2c.h"//硬件I2C
#include "si2c.h"

//void My_I2C1_Init(void);//硬件I2C

SI2C_TypeDef si2c;//全局变量，软件I2C

void My_OnBoardLED_Init(void);

int main(void)
{
	//My_I2C1_Init();//硬件I2C
	//My_I2C_SendBytes(I2C1, 0x78, commands, 5);
	
	My_OnBoardLED_Init();
	uint8_t rcvd;
	
	//数据发送，点亮屏幕
	uint8_t commands[] = {0x00, 0x8d, 0x14, 0xaf, 0xa5};
	
	si2c.SCL_GPIOx = GPIOB;//软件I2C
	si2c.SCL_GPIO_Pin = GPIO_Pin_6;//SCL随意接在没有被使用的引脚
	si2c.SDA_GPIOx = GPIOB;
	si2c.SDA_GPIO_Pin = GPIO_Pin_7;//SDA随意接在没有被使用的引脚
	My_SI2C_Init(&si2c);
	//软I2C发送数据
	My_SI2C_SendBytes(&si2c, 0x78, commands, 5);
	//软I2C接收数据
	My_SI2C_ReceiveBytes(&si2c, 0x78, &rcvd, 1);
	
	//接收数据
	//My_I2C_ReceiveBytes(I2C1, 0X78, &rcvd, 1);
	if((rcvd & (0x01 << 6)) == 0)
	{
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);//亮
	}
	else
	{
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);//灭
	}
	
	while(1)
	{
	}
}


//void My_I2C1_Init(void)//硬件I2C	
//{
//	//PB6,PB7,REMAP=0
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//	GPIO_InitTypeDef GPIO_InitStruct;
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;//复用输出开漏
//	GPIO_Init(GPIOB, &GPIO_InitStruct);
//	
//	//I2C1初始化
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
//	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);//施加复位信号
//	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);//释放复位信号
//	I2C_InitTypeDef I2C_InitStruct;
//	I2C_InitStruct.I2C_ClockSpeed = 400000;//400kbps
//	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;//标准模式
//	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;//占空比2:1
//	I2C_Init(I2C1, &I2C_InitStruct);
//	I2C_Cmd(I2C1, ENABLE);//闭合I2C1总开关
//}


void My_OnBoardLED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	GPIO_WriteBit(GPIOC, GPIO_Pin_13,Bit_SET);//默认PC13灭
}	





