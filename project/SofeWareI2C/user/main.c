#include "stm32f10x.h"

/*
软件的方式模拟I2C输出的波形
*/

void My_SWI2C_Init(void);
//函数，软件I2C初始化引脚PA0,PA1
//OLED:SCL<->PA0,SDA<->PA1

void scl_write(uint8_t level);
//参数：向时钟线写值：0/1

void sda_write(uint8_t level);
//参数：向数据线写值：0/1

uint8_t sda_read(void);
//读取SDA值
//返回值：高电压1，低电压0

void delay_us(uint32_t us);//微秒级延迟
//参数：延迟时间（微秒）

void SendStart(void);
//发送起始位
//SLC,SDA都处于高电压（空闲），给SDA一个低电压

void SendStop(void);
//发送停止位
//SCL处于低电压时，让SDA写0成低电压，再SCL处于高电压，SDA写1成高电压

uint8_t send_byte(uint8_t byte);
//发送一个字节
//参数：发送的内容
//返回值：接收方返回的值：0-ACK，1-NAK

uint8_t receive_byte(uint8_t ACK);
//接收1给字节
//参数：ACK=0/1
//返回值：读到的字节的数据

int My_SWI2C_SendBytes(uint8_t Addr, uint8_t *pData, uint16_t Size);
//软件I2C写
//参数：1.从机地址，靠左
//      2.要发送的数据（数组）
//      3.发送数据的数量（字节） 

int My_SWI2C_ReceiveBytes(uint8_t Addr, uint8_t *pBuffer, uint16_t Size);
//软件I2C读
//参数1.同上
//    2.接收缓冲区
//    3.接受数据的数量（字节）

int main(void)
{
	My_SWI2C_Init();
	
	uint8_t commands[] = {0x00, 0x8d, 0x14, 0xaf, 0xa5};
	
	My_SWI2C_SendBytes(0x78, commands, 5);
	
	while(1)
	{
	}
}


void My_SWI2C_Init(void)
//PA0,PA1初始化，通用输出开漏，初始为高电压
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能GPIOA时钟
	GPIO_InitTypeDef GPIO_InitStruct;//定义结构体
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;//0,1引脚参数相同
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;//通用输出开漏
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;//最大输出速度
	GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);
	GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);//初始化成高电压
	GPIO_Init(GPIOA, &GPIO_InitStruct);//初始化完成
}


void scl_write(uint8_t level)
//PA0
{
	if(level == 0)
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET);//写0
	}
	else
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);//写1
	}
}


void sda_write(uint8_t level)
//PA1
{
	if(level == 0)
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET);//写0
	}
	else
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);//写1
	}
}


uint8_t sda_read(void)
//读SDA
{
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == Bit_SET)//读1
	{
		return 1;//返回高电压
	}
	else//读0
	{
		return 0;//返回低电压
	}
}


void delay_us(uint32_t us)
//使用的stm32f103c8t6，每执行一次for循环，消耗1/8微秒
{
	uint32_t n = us * 8;
	for(uint32_t i = 0; i < n; i++);	
}


void SendStart(void)
{
	sda_write(0);//SDA写0
	delay_us(1);//延迟1us
}


void SendStop(void)
{
	scl_write(0);//scl先降低电压
	sda_write(0);//sda稍慢于scl降低电压
	//scl处于低电压时，sda写0
	delay_us(1);//延迟1us
	scl_write(1);//scl高电压
	delay_us(1);//延迟
	sda_write(1);//sda高电压
	delay_us(1);//延迟
}


uint8_t send_byte(uint8_t Byte)
{
	//发数据
	for(int8_t i = 7; i >= 0; i--)
	//循环8次，每次一个bit位，共1给字节
	{
		scl_write(0);//低电压
		//sda_write(x);如何确定sda写0/1
		if((Byte & (0x01 << i)) != 0)
			//0x01=00000001,左移i位，第i位是1，其余位是0
			//如果&结果!=0,声明byte第i位的!=0,byte!=0
		{
			sda_write(1);//高电压
		}
		else//&结果=0,byte=0
		{
			sda_write(0);//低电压
		}
		delay_us(1);
		scl_write(1);//高电压
		delay_us(1);
	}
	//读取ACK或NAK
	scl_write(0);
	sda_write(1);//释放，断开sda，上拉电阻高电压
	delay_us(1);
	scl_write(1);
	delay_us(1);
	return sda_read();//返回sda的值
	//0-ACK成功，1-NAK失败
}


uint8_t receive_byte(uint8_t ACK)
{
	uint8_t byte = 0;//初始化00000000
	//读取一个bit位，循环8次，读取1个字节
	for(int8_t i = 7; i >= 0; i--)
	{
		scl_write(0);
		sda_write(1);
		delay_us(1);
		scl_write(1);
		delay_us(1);
		if(sda_read() != 0)//读到高电压
		{
			byte |= (0x01 << i);
			//byte第i位是1
		}
	}//读完1个字节
	//发送ACK/NAK
	scl_write(0);
	sda_write(!ACK);
	//输出参数ACK=0,说明返回NAK，取反sda写1，高电压
	//    参数ACK=1         ACK，取反sda写0，低电压
	delay_us(1);
	scl_write(1);
	delay_us(1);
	return byte;
}


int My_SWI2C_SendBytes(uint8_t Addr, uint8_t *pData, uint16_t Size)
{
	//1.寻址
	SendStart();//发送起始位
	if(send_byte(Addr & 0xfe) != 0)//!=ACK
		//0xfe=11111110
	{
		SendStop();//发送停止位
		return -1;//寻址失败
	}
	
	//2.发数据
	for(uint32_t i = 0; i < Size; i++)
	{
		if(send_byte(pData[i] != 0))//!=ACK
		{
			SendStop();
			return -2;//发送的数据被拒收
		}
	}
	SendStop();
	return 0;//成功
}


int My_SWI2C_ReceiveBytes(uint8_t Addr, uint8_t *pBuffer, uint16_t Size)
{
	//1.寻址
	SendStart();//发送起始位
	if(send_byte(Addr | 0x01) != 0)
	{
		SendStop();
		return -1;//寻址失败
	}
	
	//2.读取数据
	for(uint32_t i = 0; i < Size-1; i++)
	{
		pBuffer[i] = receive_byte(1);
		            //ACK=1,返回ACK
	}
	pBuffer[Size - 1] = receive_byte(0);
	//最后一个数据，ACK=0返回NAK
	SendStop();
	return 0;//成功
}


