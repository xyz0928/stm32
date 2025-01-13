#include "stm32f10x.h"

void My_I2C_Init(void);//初始化I2C

int My_I2C_SendBytes(I2C_TypeDef *I2Cx, uint8_t Addr, uint8_t *pData, uint16_t Size);
//定义一个函数，通过I2C向从机发送多个字节
//参数：1.I2C接口名称
//      2.从机地址，A6...A0,RW#，左对齐
//      3.要发送的数据
//      4.发送数据的数量（字节）
//返回值：0-成功；1-寻址失败；2-发送的数据被拒绝

int main(void)
{
	My_I2C_Init();
	
	//OLED屏，从机地址0x78,01111000(RW#)
	uint8_t commands[] = {0x00, 0x8d, 0x14, 0xaf, 0xa5};
	//点亮OLED  1.命令流 2.3.使能电荷泵 4.打开屏幕开关 5.让屏幕全量
	
	My_I2C_SendBytes(I2C1, 0x78, commands, 5);
	
	while(1)
	{
	}
}


void My_I2C_Init(void)
{
	//1.IO引脚初始化
	//对I2C1重映射(AFIO),SCL:PB6->PB8;SDA:PB7->PB9
	//I2C2无重映射,SCL:PB10;SDA:PB11
	//SCL(时钟线),SDA(数据线)，均采用复用输出开漏模式_AF_OD
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//使能AFIO时钟
	GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);//重映射使能
	
	//PB8,PB9初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//开启GPIOB时钟
	GPIO_InitTypeDef GPIO_InitStruct;//声明结构体变量
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	//2个引脚参数一样，同时进行初始化，用或|
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;//复用输出开漏
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;//400k,选2MHz
	GPIO_Init(GPIOB, &GPIO_InitStruct);//完成
	
	//2.初始化I2C1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);//开启I2C1时钟（APB1）
	//复位
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);//施加复位型号
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);//释放复位信号
	I2C_InitTypeDef I2C_InitStruct;//声明I2C1结构体变量
	I2C_InitStruct.I2C_ClockSpeed = 400000;//波特率400kbps
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;//标准的I2C使用模式
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;//时钟信号占空比2:1
															 //I2C_DutyCycle_16_9,占空比16:9
	I2C_Init(I2C1, &I2C_InitStruct);//完成
	
	//闭合I2C1总开关
	I2C_Cmd(I2C1, ENABLE);//使能I2C1模块
}


int My_I2C_SendBytes(I2C_TypeDef *I2Cx, uint8_t Addr, uint8_t *pData, uint16_t Size)
{
	//1.等待总线空闲
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) == SET);//1
	//BUSY:总线忙标志位，0-空闲；1-忙
	
	//2.发送起始位
	I2C_GenerateSTART(I2Cx, ENABLE);//产生起始位
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_SB) == RESET);//0
	//SB:起始位发送完成标志位，0-未发送；1-发送完成
	
	//3.寻址阶段
	//清除AF标志位(寻址失败标志位，1-未收到ACK)
	I2C_ClearFlag(I2Cx, I2C_FLAG_AF);
	
	//发送地址7位+RW#
	I2C_SendData(I2Cx, Addr & 0xfe);
	//Addr是从机地址，0xfe=11111110,任何数&1=本身，0&任何数=0，最后一位是W写数据
	
	//等待从机回应
	while(1)
	{
		if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR) == SET)//1
			//ADDR:寻址成功标志位，1-寻址成功
		{
			break;//寻址成功跳出while
		}
		if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET)//1
			//AF:应答失败标志位，1-未收到ACK；说明寻址失败
		{
			I2C_GenerateSTOP(I2Cx, ENABLE);//发送停止位
			return -1;//寻址失败,跳出while
		}
	}
	
	//3.寻址成功后，必须先清除ADDR(先读SR1,后读SR2)
	I2C_ReadRegister(I2Cx, I2C_Register_SR1);
	I2C_ReadRegister(I2Cx, I2C_Register_SR2);
	
	//4.发送数据
	for(uint16_t i =0; i < Size; i++)
	{
		//等待发送数据寄存器空
		while(1)
		{
			if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET)//1
				//AF应答失败，1-未收到ACK，说明上一个数据被拒收
			{
				I2C_GenerateSTOP(I2Cx, ENABLE);//停止
				return -2;//数据拒收，跳出while
			}
			if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE) == SET)//1
				//TXE：发送数据寄存器空标志位，1-空
			{
				break;//发送数据寄存器空，跳出while循环
			}
		}
		I2C_SendData(I2Cx, pData[i]);//发送第i个数据
	}
	//等待发送完成
	while(1)
	{
		if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET)//1
			//AF应答失败
		{
			I2C_GenerateSTOP(I2Cx, ENABLE);//停止
			return -2;//数据拒收
		}
		if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == SET)//1
			//BTF:数据发送完成标志位，0-未完成，1-完成
		{
			break;
		}
	}
	//5.数据发送完成，发送停止位
	I2C_GenerateSTOP(I2Cx, ENABLE);
	return 0;//数据发送成功
}
