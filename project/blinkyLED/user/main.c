#include "stm32f10x.h"
#include "delay.h"//Delay()����ͷ�ļ�

int main(void)
{
	//1.����GPIOxʱ��
	//Ҫ���ĸ�ʱ�ӾͿ�����Ӧ��GPIOx��ENABLE�ǿ���ʱ�ӵ���˼
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	//2.��ʼ��IO���ţ�PC13,ͨ�������©ģʽ���������ٶ�2MHz
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };//����ṹ��
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;//����PC13����
	                                      //���ű��GPIO_Pin_0...GPIO_Pin_15
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;//�������ٶ�2MHz,�����Ը�����Ҫ����5\10MHz
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;//ģʽѡ��ͨ�������©
													  //GPIO_Mode_Out_PP��ͨ���������
													  //GPIO_Mode_AF_PP�������������
													 //GPIO_Mode_AF_OD�����������©
	GPIO_Init(GPIOC, &GPIO_InitStruct);//��ʼ��IO���ţ��Զ�д0������
	
//	//������Ĵ���д��
//	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);//д1������
//	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);//д0������
	
	while(1)//��˸
	{
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);//д0����
		Delay(100);//�ӳ�100ms
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);//д1����
		Delay(100);
	}
}
