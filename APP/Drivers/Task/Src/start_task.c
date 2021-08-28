#include "start_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

#include "usbd_cdc_if.h"
#include "spi_lcd.h"
#include "bsp_user.h"
#include "bsp_file.h"
#include "lv_conf.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_ex_conf.h"
#include "lv_demo_widgets.h"
#include "touch.h"

#define START_TASK_PRIO 1
#define START_TASK_STK_SIZE 256
TaskHandle_t Start_Task_Handler;

#define LED_TASK_PRIO 2
#define LED_TASK_STK_SIZE 256
TaskHandle_t Led_Task_Handler;

#define LCD_TASK_PRIO 2
#define LCD_TASK_STK_SIZE 1024
TaskHandle_t Lcd_Task_Handler;

void create_start_task(void)
{
    xTaskCreate((TaskFunction_t)start_task,
                (char *)"start_task",
                (uint16_t)START_TASK_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)START_TASK_PRIO,
                (TaskHandle_t *)&Start_Task_Handler);
}

void start_task(void *pvParameters)
{
    taskENTER_CRITICAL(); //进入临界区

    xTaskCreate((TaskFunction_t)led_task,
                (char *)"led_task",
                (uint16_t)LED_TASK_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)LED_TASK_PRIO,
                (TaskHandle_t *)&Led_Task_Handler);

    xTaskCreate((TaskFunction_t)lcd_task,
                (char *)"lcd_task",
                (uint16_t)LCD_TASK_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)LCD_TASK_PRIO,
                (TaskHandle_t *)&Lcd_Task_Handler);

    vTaskDelete(Start_Task_Handler); //删除开始任务
    taskEXIT_CRITICAL();             //退出临界区
}
void led_task(void *pvParameters)
{
    while (1)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        usb_printf("Task1!\r\n");
        vTaskDelay(300);
    }
}

extern uint8_t res;
extern uint8_t text[100];
uint16_t X_touch, Y_touch;

void lcd_task(void *pvParameters)
{
    uint8_t CMD_RDX = 0XD0;
    uint8_t CMD_RDY = 0X90;
    while (1)
    {
        //lcd_test();
        // lv_task_handler();
        Y_touch = TP_Read_XOY(CMD_RDY);
        X_touch = TP_Read_XOY(CMD_RDX);
        //usb_printf("Task2!%d%s\r\n", res, text);
        usb_printf("X:%d,Y:%d\r\n", X_touch, Y_touch);
        vTaskDelay(100);
    }
}
