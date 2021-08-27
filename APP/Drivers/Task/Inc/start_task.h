#ifndef __START_TASK_H
#define __START_TASK_H

#include "main.h"

void create_start_task(void);
void start_task(void *pvParameters);
void led_task(void *pvParameters);
void lcd_task(void *pvParameters);
#endif

