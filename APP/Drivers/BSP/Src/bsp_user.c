#include "iic_oled.h"
#include "usbd_cdc_if.h"
#include "bsp_file.h"
#include "spi_lcd.h"
#include "bsp_user.h"
#include "stdio.h"
#include "tim.h" 

extern TIM_HandleTypeDef htim2;
void bsp_init(void)
{
	HAL_TIM_Base_Start_IT(&htim2);
	LCD_Init();
}
uint8_t text_lcd[10];
void lcd_test(void)
{
	static int i=0;
	i++;
	sprintf((char *)text_lcd,"num:%d",i);
	LCD_ShowString(0,0,16,text_lcd,0,RED,WHITE);
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    static unsigned int ms=0;
    if (htim == (&htim2))
    {
        ms++;
		if(ms==1000)
		{
			ms=0;
		}
		if(ms%500==0)
		{
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
			//lcd_test();
			//usb_printf("Hello,Xiaofei!!!\r\nI am app!\r\n");
		}
    }
}
