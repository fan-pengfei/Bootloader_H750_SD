#include "main.h"
#include "bsp_file.h"
#include "fatfs.h"
#include "iic_oled.h"
#include "usbd_cdc_if.h"
uint8_t text[100]="Heollo,I'am Test!,xiaofei!!aabb";
uint8_t temp_bin[1024*128];
extern volatile uint8_t OLED_GRAM[128][8];
extern FATFS SDFatFS;    /* File system object for SD logical drive */
FIL fp_txt,fp_bin;
UINT bw;
UINT br;
uint32_t num_a;
void file_test(void)
{
	FRESULT res;
	if( f_mkdir("video")== FR_OK || f_mkdir("video")== FR_EXIST)
	{
		res=f_open(&fp_txt, "video/test.txt", FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
	}
	// 如果打开文件成功，就可以写入数据
	if (res == FR_OK)
	{
		res = f_write(&fp_txt, text, 50, &bw);
	}
	f_close(&fp_txt);
}
void read_bin(void)
{
	FRESULT res;
	int i,j,k,n;
	if( f_mkdir("video")== FR_OK || f_mkdir("video")== FR_EXIST)
	{
		res=f_open(&fp_bin, "video/badapple.bin", FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
	}		
	usb_printf("size:%d\r\n",f_size(&fp_bin));
	for(n=0;n<f_size(&fp_bin)%(8*1024);n++)
	{
		res = f_read(&fp_bin, temp_bin, 8*1024, &br);
		for(k=0;k<8;k++)
		{
			for(i=0;i<8;i++)  
			{     
				for(j=0;j<128;j++)
				{
					OLED_GRAM[j][i]=temp_bin[1024*k+i*128+j];
				}
			}
			OLED_Refresh_Gram();
		}
	}
	f_close(&fp_bin);
}
