#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Key.h"
#include "Servo.h"

uint8_t button_num;
uint16_t angle;

int main(void)
{
	Key_Init();
	OLED_Init();
	Servo_Init();
	OLED_ShowString(1, 1, "angle:");
	while (1)
	{
		button_num = Key_GetNum();
		if(button_num == 1)
		{
			angle += 30;
			if(angle > 180)
			{
				angle = 0;
			}
		}
		Servo_SetAngle(angle);
		OLED_ShowNum(1, 7, angle, 3);
	}
}
