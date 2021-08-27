#include "start_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

#include "usbd_cdc_if.h"
#include "spi_lcd.h"
#include "bsp_user.h"
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
void lcd_task(void *pvParameters)
{
    while (1)
    {
        lcd_test();
        usb_printf("Task2!\r\n");
        vTaskDelay(3);
    }
}
