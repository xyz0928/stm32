#include "stm32f10x.h"

void App_SPI1_Init(void);//SPI引脚初始化

//SPI以主机身份收发数据
void App_SPI_MasterTransmitReceive(SPI_TypeDef *SPIx, const uint8_t *pDataTx, uint8_t *pDataRx, uint16_t Size);

int main(void)
{
	App_SPI1_Init();
	
	while(1)
	{
	}
}


void App_SPI1_Init(void)
{
	//1.初始化IO引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//AFIO模块使能
	GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);//SPI1重映射
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//主机SCK,PB3,AFPP,2MHz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//主机MISO,PB4,IPU
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//主机MOIS,PB5,AFPP
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//主机普通IO引脚向从机CS(NSS)写1/0
	//PA15,Out_PP,2MHz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//2.SPI初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//使能SPI1模块时钟
	SPI_InitTypeDef SPI_InitStruct;
	//选择SPI通信方向
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//2线全双工
								//SPI_Direction_2Lines_ReadOnly//2线只读
								//SPI_Direction_1Line_Tx//单线发送
								//SPI_Direction_1Line_Rx//单线接收
	//选择SPI模式
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;//主机,STM32
							//SPI_Mode_Slave//从机,W25Q64
	//数据宽度
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//8bit
								//SPI_DataSize_16b//16bit
	//时钟的极性
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;//高极性,1
							//SPI_CPOL_Low//低极性,0
	//时钟的相位
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;//相位1,第2边沿采集,11:MODE3
							//SPI_CPHA_1Edge//相位0,第1边沿采集,10:MODE2
	//比特位传输顺序
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//先传最高有效位7,6..1,0
							    //SPI_FirstBit_MSB//先传最低有效位0,1..6,7
	//选择波特率分频器的分频系数
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
	//时钟72MHz,波特率:72/64=1.125MHz,满足上面引脚初始化的最大输出速度
	
	//NSS选择
	//多主机：主机1主动向主机2通信，主机1IO向主机2NSS写0低电压，主机2变从机
	//单主机：NSS接高电压（软、硬）
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;//软件NSS,通过内部NSS写0低电压1高电压
						   //SPI_NSS_Hard//硬件NSS，外接3.3V
	SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);//1
	//通过内部NSS写0低电压1高电压
	SPI_Init(SPI1, &SPI_InitStruct);
}


void App_SPI_MasterTransmitReceive(SPI_TypeDef *SPIx, const uint8_t *pDataTx, uint8_t *pDataRx, uint16_t Size)
{
	//1.闭合SPI总开关
	SPI_Cmd(SPIx, ENABLE);
	
	//2.先发送第一个字节
	SPI_I2S_SendData(SPIx, pDataTx[0]);
	//I2S总线，音频，与SPI合并
	
	//3.循环Size-1次，收发Size-1个字节
	for(uint16_t i = 0; i < Size - 1; i++)
	{
		//向TDR写数据
		while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);//0
		//查询TXE发送数据寄存器空标志位，1-空
		//上面发的第一个字节进入移位寄存器中，TXE=1，写第二个字节
		SPI_I2S_SendData(SPIx, pDataTx[i + 1]);//发送第i+1个字节
		
		//从RDR读数据
		while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);//0
		//查询RXNE接收数据寄存器非空标志位，1-非空
		//接收移位寄存器中的字节进入RXNE
		pDataRx[i] = SPI_I2S_ReceiveData(SPIx);//接收第i个字节
	}
	
	//4.读取最后一个字节
	//上面for循环少读了最后一个字节
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);//0
	pDataRx[Size-1] = SPI_I2S_ReceiveData(SPIx);
	
	//5.断开SPI总开关
	SPI_Cmd(SPIx, DISABLE);
}


