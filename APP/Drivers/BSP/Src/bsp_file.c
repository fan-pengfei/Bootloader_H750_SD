#include "main.h"
#include "bsp_file.h"
#include "fatfs.h"
#include "iic_oled.h"
#include "usbd_cdc_if.h"
uint8_t text[100]="Heollo,I'am Test!,xiaofei!!aabb";
uint8_t temp_bin[1024*128];
extern uint8_t OLED_GRAM[128][8];
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
	int i,j,k,l,kb_num;
	if( f_mkdir("video")== FR_OK || f_mkdir("video")== FR_EXIST)
	{
		res=f_open(&fp_bin, "video/badapple.bin", FA_READ);
	}
	kb_num=f_size(&fp_bin)%(1024*128);
	num_a=f_size(&fp_bin);
	usb_printf("%d\r\n",kb_num);
	// 如果打开文件成功，就可以写入数据
	if (res == FR_OK)
	{
		for(i=0;i<kb_num;i++)
		{
			res = f_read(&fp_bin, temp_bin, 1024, &br);
			for(j=0;j<128;i++)
			{
				for(k=0;k<8;k++)
				{
				  for(l=0;l<128;l++)
				  {
					OLED_GRAM[l][k]=temp_bin[k*128+l];
				  }
				}
				OLED_Refresh_Gram();
//			}
		}
	}
	f_close(&fp_bin);
}
//	if( f_mkdir("video")== FR_OK || f_mkdir("video")== FR_EXIST)
//	{
//		res=f_open(&fp_bin, "video/badapple.bin", FA_READ);
//	}
//	kb_num=f_size(&fp_bin)%(1024*128);
//	num_a=f_size(&fp_bin);
//	usb_printf("%d\r\n",kb_num);
//	// 如果打开文件成功，就可以写入数据
//	if (res == FR_OK)
//	{
//		for(i=0;i<kb_num;i++)
//		{
//			res = f_read(&fp_bin, temp_bin, 1024*128, &br);
//			for(j=0;j<128;i++)
//			{
//				for(k=0;k<8;k++)
//				{
//				  for(l=0;l<128;l++)
//				  {
//					OLED_GRAM[k][j]=temp_bin[j*128*8+k*128+l];
//				  }
//				}
//				OLED_Refresh_Gram();
//			}
//		}
//		res = f_read(&fp_bin, temp_bin, f_size(&fp_bin)-kb_num*1024*128, &br);
//		kb_num=(f_size(&fp_bin)-kb_num*1024*128)%(8*128);
//		for(j=0;j<kb_num;i++)
//		{
//			for(k=0;k<8;k++)
//			{
//			  for(l=0;l<128;l++)
//			  {
//				OLED_GRAM[k][j]=temp_bin[j*128*8+k*128+l];
//			  }
//			}
//			OLED_Refresh_Gram();
//		}
//	}
//	f_close(&fp_bin);
}
//uint8_t temp[1024*100];
//extern FATFS SDFatFS;    /* File system object for SD logical drive */
//FIL fp_txt,fp_bin;
//UINT bw;
//UINT br;
//extern char SDPath[4];   /* SD logical drive path */
//void read_bin(void)
//{
//	FRESULT res;
//	uint32_t crc_temp;
//	if( f_mkdir("video")== FR_OK || f_mkdir("video")== FR_EXIST)
//	{
//		res=f_open(&fp_bin, "video/bad", FA_READ);
//	}
//	// 如果打开文件成功，就可以写入数据
//	if (res == FR_OK)
//	{
//		res = f_read(&fp_bin, temp, f_size(&fp_bin), &br);
//	}
//	crc_temp=crc32(temp,f_size(&fp_bin));
//	sprintf((char *)text,"%x",crc_temp);
//	res = f_write(&fp_txt, text, 10, &bw);
//	f_close(&fp_bin);
//	f_close(&fp_txt);
//}
