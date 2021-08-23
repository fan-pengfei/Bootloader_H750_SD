#include "bootloader.h"
#include "fatfs.h"
#include "sdmmc.h"
#include "usart.h"
#include "stdio.h"
uint32_t crc32(unsigned char *buf,uint32_t size)
{
	uint32_t i, crc;
	crc = 0xFFFFFFFF;
	for (i = 0; i < size; i++)
	{
		crc = crc32tab[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
	}
	return crc^0xFFFFFFFF;
}
uint8_t temp[1024*200];
uint8_t text[100];
extern FATFS SDFatFS;    /* File system object for SD logical drive */
FIL fp_txt,fp_bin;
UINT bw;
UINT br;
extern char SDPath[4];   /* SD logical drive path */
void read_bin(void)
{
	FRESULT res;
	uint32_t crc_temp;
	if( f_mkdir("sd_bin")== FR_OK || f_mkdir("sd_bin")== FR_EXIST)
	{
		res=f_open(&fp_txt, "sd_bin/demo.txt", FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
		res=f_open(&fp_bin, "sd_bin/demo.bin", FA_READ);
	}
	// 如果打开文件成功，就可以写入数据
	if (res == FR_OK)
	{
		res = f_read(&fp_bin, temp, f_size(&fp_bin), &br);
	}
	crc_temp=crc32(temp,f_size(&fp_bin));
	sprintf((char *)text,"%x",crc_temp);
	res = f_write(&fp_txt, text, 10, &bw);
	f_close(&fp_bin);
	f_close(&fp_txt);
}

