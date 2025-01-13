#include "stm32f10x.h"

void My_I2C_Init(void);//初始化I2C

int My_I2C_SendBytes(I2C_TypeDef *I2Cx, uint8_t Addr, uint8_t *pData, uint16_t Size);
//定义一个函数，通过I2C向从机发送多个字节
//参数：1.I2C接口名称
//      2.从机地址，A6...A0,RW#，左对齐
//      3.要发送的数据
//      4.发送数据的数量（字节）
//返回值：0-成功；-1-寻址失败；-2-发送的数据被拒绝

int My_I2C_ReceiveBytes(I2C_TypeDef *I2Cx, uint8_t Addr, uint8_t *pBuffer, uint16_t Size);
//定义函数，通过I2C从从机读取多个字节
//参数：1.2.同上
//      3.接受缓冲区（数组）
//      4.要接收数据的数量（字节）
//返回值：0-读取成功；-1-寻址失败

void My_OnBoardLED(void);
//板载LED，PC13，输出开漏

int main(void)
{
	My_I2C_Init();
	
	My_OnBoardLED();//初始化板载LED
	
	//OLED屏(从机)地址0x78,01111000(RW#)
	uint8_t commands[] = {0x00, 0x8d, 0x14, 0xaf, 0xa5};
	My_I2C_SendBytes(I2C1, 0x78, commands, 5);
	//作用：点亮OLED  1.命令流 2.3.使能电荷泵 4.打开屏幕开关 5.让屏幕全量
	
	//接收的字节
	uint8_t rcvd;
	
	My_I2C_ReceiveBytes(I2C1, 0x78, &rcvd, 1);
	if((rcvd & (0x01<<6)) == 0)
	//0x01=00000001,左移6位:01000000
	//任何数&1=任何数，任何数&0=0
	//从OLED读1个字节8bits:D7 D6...D1 D0
	//只有D6=0，屏幕亮；D6=1，屏幕灭
	{
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);//0,亮
	}
	else
	{
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);//1,灭
	}
	
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


int My_I2C_ReceiveBytes(I2C_TypeDef *I2Cx, uint8_t Addr, uint8_t *pBuffer, uint16_t Size)
{
	//1.发送起始位
	I2C_GenerateSTART(I2Cx, ENABLE);//产生起始位
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_SB) == RESET);//0
	//SB:起始位发送完成标志位，1-完成，0-未发送
	
	//2.寻址
	//清除AF标志位
	I2C_ClearFlag(I2Cx, I2C_FLAG_AF);
	
	//发送7位地址+RW#
	I2C_SendData(I2Cx, Addr | 0x01);
	//0x01=00000001,任何数|0=任何数，任何数|1=1，最后一位是主机读数据，写1
	
	//等待从机应答
	while(1)
	{
		if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR) == SET)//1
			//寻址成功
		{
			break;
		}
		if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET)//1
			//应答失败标志位
		{
			I2C_GenerateSTOP(I2Cx, ENABLE);//停止
			return -1;//寻址失败
		}
	}
	//3.接收数据
	//接收1个字节
	if(Size == 1)
	{
		//清除ADDR，寻址完成后必须清除（先清除SR1,后SR2）
		I2C_ReadRegister(I2Cx, I2C_Register_SR1);
		I2C_ReadRegister(I2Cx, I2C_Register_SR2);
		
		//ACK写0,STOP写1
		I2C_AcknowledgeConfig(I2Cx, DISABLE);//0,发NAK
		I2C_GenerateSTOP(I2Cx, ENABLE);//1,发送停止位
		
		//等待接收数据寄存器非空，有数据
		while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET);//0
		//RXNE:接收数据寄存器非空标志位，0-空，1-非空
		
		//读取数据
		pBuffer[0] = I2C_ReceiveData(I2Cx);//放在第一个
	}
	//接收2个字节
	else if(Size == 2)
	{
		//清除ADDR
		I2C_ReadRegister(I2Cx, I2C_Register_SR1);
		I2C_ReadRegister(I2Cx, I2C_Register_SR2);
		
		//ACK=1
		I2C_AcknowledgeConfig(I2Cx, ENABLE);//1,发ACK
		
		//等待RXNE=1非空,有数据
		while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET);//0
		
		//读取第一个字节
		pBuffer[0] = I2C_ReceiveData(I2Cx);
		
		//ACK=0,STOP=1
		I2C_AcknowledgeConfig(I2Cx, DISABLE);//ACK=0,发NAK
		I2C_GenerateSTOP(I2Cx, ENABLE);//STOP=1,发送停止位
		
		//等待RXNE非空
		while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET);//0
		
		//读取第二个字节
		pBuffer[1] = I2C_ReceiveData(I2Cx);
	}
	//读取多个字节
	else
	{
		//清除ADDR
		I2C_ReadRegister(I2Cx, I2C_Register_SR1);
		I2C_ReadRegister(I2Cx, I2C_Register_SR2);
		
		//ACK=1
		I2C_AcknowledgeConfig(I2Cx, ENABLE);//1
		
		for(uint16_t i = 0; i < Size - 1; i++)
		//size个字节，循环size-1次
		{
			//等待RXNE=1非空
			while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET);//0空，0->1
			
			//读取数据size-1个字节
			pBuffer[i] = I2C_ReceiveData(I2Cx);
		}
		//ACK=0,STOP=1
		I2C_AcknowledgeConfig(I2Cx, DISABLE);//0,发NAK
		I2C_GenerateSTOP(I2Cx, ENABLE);//1,发送停止位
		
		//等待RXNE=1非空
		while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET);
		
		//读取最后一个数据
		pBuffer[Size - 1] = I2C_ReceiveData(I2Cx);
	}
	return 0;//接收成功
}


void My_OnBoardLED(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//开启GPIOC时钟
	GPIO_InitTypeDef GPIO_InitStruct;//声明结构体变量
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;//通用输出开漏
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
}


