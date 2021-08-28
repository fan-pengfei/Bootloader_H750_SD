#include "iic_oled.h"
#include "usbd_cdc_if.h"
#include "bsp_file.h"
#include "spi_lcd.h"
#include "bsp_user.h"
#include "stdio.h"
#include "tim.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "start_task.h"
#include "fatfs.h"
#include "lv_conf.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_ex_conf.h"
#include "lv_demo_widgets.h"
#include "touch.h"
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim6;
uint8_t res;
extern FATFS SDFatFS; 
void bsp_init(void)
{
    int i;
    HAL_TIM_Base_Start_IT(&htim2);
    HAL_TIM_Base_Start(&htim6);  
    LCD_Init();
    TP_Init();
    res = f_mount(&SDFatFS, "0:", 1);
	lv_init();
    lv_port_disp_init();
    lv_demo_widgets();
    file_test();
    file_Read_test();
    //    while(1)
    //	{
    //        lcd_test();
    //        usb_printf("Task2!\r\n");
    //        HAL_Delay(100);
    //	}
    create_start_task();   //创建开始任务
    vTaskStartScheduler(); //开启任务调度
}

void lcd_test(void)
{
    static int i = 0;
    uint8_t text_lcd[10];
    i++;
    sprintf((char *)text_lcd, "num:%d", i);
    LCD_ShowString(0, 0, 16, text_lcd, 0, RED, WHITE);
}
void Delay_us(uint16_t us) 
{
    uint16_t startCnt = __HAL_TIM_GET_COUNTER(&htim6);
    while ((__HAL_TIM_GET_COUNTER(&htim6) - startCnt) <= us);
}