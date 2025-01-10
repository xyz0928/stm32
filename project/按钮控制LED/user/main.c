#include "stm32f10x.h"

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//����ʱ�ӣ�Aģ��
	
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };//��������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;//����PA0����LED
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;//�������ٶ�
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;//���ģʽ��ͨ������
	GPIO_Init(GPIOA, &GPIO_InitStruct);//��ʼ��PA0
	
	//�Ӱ�ť���ƣ���������ģʽ���ӵ�PA1�͵�
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;//��ť������1
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;//��������ģʽ
	                                    //IPD:��������
	                                    //_IN_FLOATING:���븡��
	                                    //AIN:ģ��ģʽ
	GPIO_Init(GPIOA, &GPIO_InitStruct);//����GPIO_Init(),��ɳ�ʼ��PA1
	
	//д������LED
	//GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);//PA0д1������LED
	
	while(1)
	{
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == Bit_RESET)//PA1��ť����
		{
			GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);//PA0д1��LED�Ƶ���
		}
		else//�ɿ���ť
		{
			GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET);//PA0д0��LED��
		}
	}
}
