#include "stm32f10x.h"

void App_SYSCLK_Init(void);

int main(void)
{
	App_SYSCLK_Init();
	
	//使用HSI 8MHz，PC13闪
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_Inits;
	GPIO_Inits.GPIO_Pin = GPIO_Pin_13;
	GPIO_Inits.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Inits.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_Inits);
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);//亮
	while(1)
	{
		//延迟500ms=0.5s
		//假设每个for循环消耗6个时钟周期，时钟频率8MHz，周期、频率互为倒数
		//  0.5s/((1/8e6Hz)*6)=666,666个循环
		//在执行main代码前会执行"startup_stm32f10x_md.s"->"Reset_Handler"->"SystemInit"默认达到最高频率
		//但HSI8M->AHB8M->Cortex-M3内核只需要8MHz
		//需要加;注释掉  ;LDR R0, =SystemInit  ;BLX     R0
		for(uint32_t i = 0; i < 666666; i++)
		{}//延迟500ms
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);//灭
		for(uint32_t i = 0; i < 666666; i++)
		{}//延迟500ms，达到一亮一灭1s	
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);//亮
	}
	
//	//片上外设的开关和复位 
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//开启AHB上DMA1时钟
//	
//	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);//施加复位信号
//	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);//释放复位信号，2步完成复位
}


//使用HSE 8MHz，把SYSCLK,HCLK,PCLK1,PCLK2达到最高频率,PC13快闪
void App_SYSCLK_Init(void)
{
	//开启Flash指令预取缓冲区，要在SYSCLK<=8MHz时进行
	//一开始就要写
	FLASH_PrefetchBufferCmd(ENABLE);
	//Cortex-M3内核（CPU）<- 缓冲区 <- Flash(写程序，速度<24MHz)
	
	//设置Flash访问延迟
	FLASH_SetLatency(FLASH_Latency_2);//等待2个周期,SYSCLK<=72MHz
	               //FLASH_Latency_0//不等待,SYSCLK<=24MHz
	               //FLASH_Latency_1//等待1个周期,SYSCLK<=48MHz
	
	//1.开启HSE
	RCC_HSEConfig(RCC_HSE_ON);//开启HSE（默认关），会消耗一定时间
	//获取RCC状态
	while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);//等待HSE就绪
	
	//2.配置并开启锁相环
	//配置锁相环参数
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
	//锁相环的输入选择HSE（Div2是HSE/2），倍乘系数9
	RCC_PLLCmd(ENABLE);//开启锁相环（默认关），会消耗一定时间
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);//等待锁相环就绪
	
	//3.配置AHB,APB1,APB2分频器的分频系数
	//AHB
	RCC_HCLKConfig(RCC_SYSCLK_Div1);//HCLK = SYSCLK / 1
	//APB1
	RCC_PCLK1Config(RCC_HCLK_Div2);//PCLK1 = HCLK / 2
	//APB2
	RCC_PCLK2Config(RCC_HCLK_Div1);//PCLK2 = HCLK / 1
	
	//4.配置SYSCLK来源，需要一定时间
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);//来源为锁相环
	               //RCC_SYSCLKSource_HSE//来源HSE
	               //RCC_SYSCLKSource_HSI//来源HSI
	//获取SYSCLK来源
	while(RCC_GetSYSCLKSource() != 0X08);//等待来源切换完成
	//返回值：0x00-HSI来源
	//        0x04-HSE
	//        0x08-PLL
}



