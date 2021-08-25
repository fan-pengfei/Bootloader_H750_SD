#include "main.h"
#include "fatfs.h"
#include "quadspi.h"
#include "sdmmc.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"
#include "qspi_w25q64.h"
#include "bootloader.h"
#include "usbd_core.h"
typedef  void (*pFunction)(void);
pFunction JumpToApplication;
extern FATFS SDFatFS; 
FIL fp;
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
extern 	uint32_t crc_temp_new,crc_temp_old;;
void SystemClock_Config(void);
int main(void)
{
	SCB_EnableICache();
	SCB_EnableDCache();
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_SDMMC1_SD_Init();
	MX_FATFS_Init();
	MX_USART1_UART_Init();
	QSPI_W25Qxx_Init();
	HAL_Delay(100);
	f_mount(&SDFatFS, "0:", 1);
	if(f_open(&fp, "debug.txt", FA_READ)!=FR_NO_FILE)//debug
	{
		f_close(&fp);
        HAL_Delay(100);
        MX_USB_DEVICE_Init();
	    HAL_Delay(100);
        //QSPI_W25Qxx_Test();
        while(1)
        {
			
			printf("\r\nBoot_loader running!\r\n");
			printf("��ѱ�Ҫ�ļ��ŵ�ָ��·����\r\n");
		    HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
		    HAL_Delay(100);
            if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15)==0)
            {
				read_bin();
				printf("crc:%x,%x\r\n",crc_temp_new,crc_temp_old);
                HAL_PCD_MspDeInit(&hpcd_USB_OTG_FS);
				HAL_SD_MspDeInit(&hsd1);
				FATFS_UnLinkDriver("0:");
				f_mount(NULL, "0:", 1);
				SCB_DisableICache();		// �ر�ICache
				SCB_DisableDCache();		// �ر�Dcache
				SysTick->CTRL = 0;		// �ر�SysTick
				SysTick->LOAD = 0;		// ��������ֵ
				SysTick->VAL = 0;			// �������ֵ
				JumpToApplication = (pFunction) (*(__IO uint32_t*) (0x8012000 + 4));	// ������ʼ��ַ
				__set_MSP(*(__IO uint32_t*) 0x8012000);	// ��������ջָ��
				JumpToApplication();			// ִ����ת
            }
        }
	}
    if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15)==0)
    {
        while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15)==0);
        HAL_Delay(100);
        MX_USB_DEVICE_Init();
	    HAL_Delay(100);
        //QSPI_W25Qxx_Test();
        while(1)
        {
			
			printf("\r\nBoot_loader running!\r\n");
			printf("��ѱ�Ҫ�ļ��ŵ�ָ��·����\r\n");
		    HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
		    HAL_Delay(100);
            if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15)==0)
            {
				read_bin();
				printf("crc:%x,%x\r\n",crc_temp_new,crc_temp_old);
                HAL_PCD_MspDeInit(&hpcd_USB_OTG_FS);
				HAL_SD_MspDeInit(&hsd1);
				FATFS_UnLinkDriver("0:");
				f_mount(NULL, "0:", 1);
				SCB_DisableICache();		// �ر�ICache
				SCB_DisableDCache();		// �ر�Dcache
				SysTick->CTRL = 0;		// �ر�SysTick
				SysTick->LOAD = 0;		// ��������ֵ
				SysTick->VAL = 0;			// �������ֵ
				JumpToApplication = (pFunction) (*(__IO uint32_t*) (0x8012000 + 4));	// ������ʼ��ַ
				__set_MSP(*(__IO uint32_t*) 0x8012000);	// ��������ջָ��
				JumpToApplication();			// ִ����ת
            }
        }
    }
    else
    {
	    HAL_Delay(100);
        //QSPI_W25Qxx_Test();
	    read_bin();
		printf("crc:%x,%x\r\n",crc_temp_new,crc_temp_old);
        HAL_Delay(100);
		HAL_SD_MspDeInit(&hsd1);
		FATFS_UnLinkDriver("0:");
		f_mount(NULL, "0:", 1);
        SCB_DisableICache();		// �ر�ICache
        SCB_DisableDCache();		// �ر�Dcache
        SysTick->CTRL = 0;		// �ر�SysTick
        SysTick->LOAD = 0;		// ��������ֵ
        SysTick->VAL = 0;			// �������ֵ
        JumpToApplication = (pFunction) (*(__IO uint32_t*) (0x8012000 + 4));	// ������ʼ��ַ
        __set_MSP(*(__IO uint32_t*) 0x8012000);	// ��������ջָ��
        JumpToApplication();			// ִ����ת
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Macro to configure the PLL clock source
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
      Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_SDMMC
                              |RCC_PERIPHCLK_USB|RCC_PERIPHCLK_QSPI;
  PeriphClkInitStruct.PLL2.PLL2M = 25;
  PeriphClkInitStruct.PLL2.PLL2N = 180;
  PeriphClkInitStruct.PLL2.PLL2P = 2;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 5;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_0;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOMEDIUM;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.PLL3.PLL3M = 25;
  PeriphClkInitStruct.PLL3.PLL3N = 192;
  PeriphClkInitStruct.PLL3.PLL3P = 2;
  PeriphClkInitStruct.PLL3.PLL3Q = 4;
  PeriphClkInitStruct.PLL3.PLL3R = 2;
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_0;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
  PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_D1HCLK;
  PeriphClkInitStruct.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL2;
  PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL3;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable USB Voltage detector
  */
  HAL_PWREx_EnableUSBVoltageDetector();
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
