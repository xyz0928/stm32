#include "stm32f10x.h"

void My_USART1_Init(void);//�����ʼ����������ʼ��IO���ź�USART1����

void My_OnBoardLED_Init(void);//���庯����ʼ������LED�ƣ�PC13

int main(void)
{
	
	My_USART1_Init();//���ú�������ʼ��
	My_OnBoardLED_Init();

	while(1)
	{
		//1.�ȴ��������ݼĴ���RDR�ǿ�
		while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);//RXNE=0,�գ�һֱѭ����ֱ��RXNE=1�ǿգ����к������
		
		//2.�����ݴӽ������ݼĴ����ж�ȡ
		uint8_t byteRcvd = USART_ReceiveData(USART1);
		
		//3.�����ݽ��д���
		if(byteRcvd == '0')//�����ַ�Ϊ0
		{
			GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);//��ƣ���©�ӷ�1��
		}
		else if(byteRcvd == '1')
		{
			GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);//����
		}
	}
}


void My_OnBoardLED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;//�����ṹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//����GPIOCʱ��
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;//ͨ�������©ģʽ
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;//�������ٶ�
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);//PC13д1����
}


void My_USART1_Init(void)
{
//	//����USART1���ţ�Ĭ����PA9,PA10,REMAP=0
//	//USART1_Tx  PA9,��������ģʽ
//	GPIO_InitTypeDef GPIO_InitStruct;//����PA9��IO���Žṹ��ָ��
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//����GPIOAʱ��
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;//����9
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//��������ģʽ
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;//�������ٶ�10MHz
//	GPIO_Init(GPIOA, &GPIO_InitStruct);//��ʼ�����PA9
//
//	//USART_Rx  PA10,��������ģʽ
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//����GPIOAʱ��
//	GPIO_InitTypeDef GPIO_InitStruct1;//����PA10�����Žṹ��
//	GPIO_InitStruct1.GPIO_Pin = GPIO_Pin_10;
//	GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_IPU;//��������ģʽ
//	GPIO_Init(GPIOA, &GPIO_InitStruct1);

	//USART1������ӳ�䣬REMAP=1����PB6,PB7
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//����AFIOģ�飨�������裩ʱ��
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);//USART��ӳ��
	GPIO_InitTypeDef GPIO_InitStruct;//����ṹ��ָ��
	
	//PB6����USB TO TTL ��RXD
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//����GPIOBʱ��
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);//��ʼ�����PB6����
	
	//PB7����USB TO TTL ��TXD
	//���GND��GEN
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStruct);//��ʼ�����PB7����
	
	//���ڳ�ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//����USART1ʱ��
	USART_InitTypeDef USART_InitStruct;//���崮��USART�ṹ��ָ��
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;//˫��
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//8λ����λ
	USART_InitStruct.USART_Parity = USART_Parity_No;//��У��
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//1λֹͣλ
	USART_Init(USART1, &USART_InitStruct);//���ڳ�ʼ�����
	
	USART_Cmd(USART1, ENABLE);//USARTģ��ʹ��
	                //DISABLE,��ֹ
}





