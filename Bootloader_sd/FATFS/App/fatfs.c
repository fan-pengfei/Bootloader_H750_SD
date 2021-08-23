/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "fatfs.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */

/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */
}

/* USER CODE BEGIN Application */
uint8_t fatfs_test(void)
{
  int i;
	uint32_t btw = 0;
	static uint8_t step = 0;
    /** 使用中文�?体编码方式可以写入中�? */
	char test_buf[] = {"hhhhh\n"};
	
    /** 使用状�?�机的方式完成以下操�? */	
	switch(step)
	{
		case 0:
			if((f_mount(&SDFatFS, (TCHAR const*)SDPath, 1)==0))
			{
				step++;
			}
			break;
			
		case 1: 
			if( f_mkdir("sd_test")== FR_OK || f_mkdir("sd_test")== FR_EXIST)
			{
				step++;
			}
			break;	

		case 2: 
			if( f_open(&SDFile, "sd_test/test.txt", FA_CREATE_ALWAYS | FA_WRITE | FA_READ)==FR_OK )
			{
				step++;
			}
			break;	
			
		case 3:
		if(i <=100)
		{
			if(f_write(&SDFile,&test_buf[0],6,&btw) == FR_OK)
			{
				i++;
				f_sync(&SDFile);
			}
			
		}
		else
		{
			f_close(&SDFile);
			step++;
		}			
		break;
		
		default:
			
			break;	
	}
	
	if(step > 3)
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
