#include "stm32f10x.h"

/*
void USART_Init(�������ƣ���ʼ������)
struct USART_InitTypeDef//��ʼ������
{
	uint32_t USART_BaudRate;//������,9600/115200/921600
	uint16_t USART_WordLength;//����λ����
													 //USART_WordLength_8b: 8bit
													 //USART_WordLength_9b: 9bit
	uint16_ USART_StopBits;//ֹͣλ����
	                      //USART_StopBits_0_5: 0.5bit
												//USART_StopBits_1: 1bit
												//USART_StopBits_1_5: 1.5bit
												//USART_StopBits_2: 2bit
	uint16_t USART_Parity;//У�鷽ʽ
	                      //USART_Parity_No: ��У��
												//USART_Parity_Even: żУ��
												//USART_Parity_Odd: ��У��
	unit13_t USART_Mode;//�����շ�����
	                   //USART_Mode_Tx: ������
										 //USART_Mode_Rx: ������
										 //USART_Mode_Tx | USART_Mode_Rx: �շ�˫��
}
*/

void My_USART_SendBytes(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size);
//����һ��������ʹ�ô���һ���Է��Ͷ���ֽ�
//�������������ƣ�Ҫ���͵����ݣ����飩�������ֽ����� 

int main(void)
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

	uint8_t bytesSend[] = {1,2,3,4,5};//���͵�����
	My_USART_SendBytes(USART1, bytesSend, 5);//����Size�������

/*
FlagStatus USART_GetFlagStatus(�������ƣ���ѯ�ı�־λ)
��ѯUSART�ı�־λ�ķ���ֵ��RESET=0,SET=1
*/

/*
void USART_SendData(�������ƣ�Ҫ���͵����ݣ��޷���16bitλ���ͣ�2���ֽڣ�)
��Ҫ���͵�����д�뷢�����ݼĴ�����
*/

	while(1)
	{
	}
}

void My_USART_SendBytes(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size)
{
	for(uint32_t i = 0; i < Size; i++)
	{
		//1.�ȴ��������ݼĴ���Ϊ��
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		//��ѯTXE��־λ��RESET=0,TXE=0,�ǿգ�һֱѭ��������ΪSET=1ʱ���գ�������һ������
		
		//2.Ҫ���͵�����д�뷢�����ݼĴ�����
		USART_SendData(USART1, pData[i]);
	}
	
	//3.�ȴ����ݷ������
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	//��ѯTC��־λ��TC=0=RESET,δ��ɣ�TC=1=SET,���	
}


