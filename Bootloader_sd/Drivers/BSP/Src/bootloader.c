#include "bootloader.h"
#include "fatfs.h"
#include "sdmmc.h"
#include "usart.h"
#include "stdio.h"
#include "qspi_w25q64.h"
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
FIL fp_txt,fp_bin,fp_old_bin;
uint32_t crc_temp_new,crc_temp_old;
UINT bw;
UINT br;
extern char SDPath[4];   /* SD logical drive path */
void read_bin(void)
{
	FRESULT res;
	if( f_mkdir("sd_bin")== FR_OK || f_mkdir("sd_bin")== FR_EXIST)
	{
		res=f_open(&fp_bin, "sd_bin/demo.bin", FA_READ);
		
		// 如果打开文件成功，就可以写入数据
		if (res == FR_OK)
		{
			QSPI_W25Qxx_ReadBuffer(temp,0x00,f_size(&fp_bin));
			crc_temp_old=crc32(temp,f_size(&fp_bin));
			res = f_read(&fp_bin, temp, f_size(&fp_bin), &br);
			crc_temp_new=temp[0x1c]*256*256*256+temp[0x1c+1]*256*256+temp[0x1c+2]*256+temp[0x1c+3];
		}
		if(crc_temp_new!=crc_temp_old)
		{
			if(f_size(&fp_bin)<=64*1024)
			{
				QSPI_W25Qxx_BlockErase_64K(0x00);
			}
			else if((f_size(&fp_bin)<=128*1024)&&(f_size(&fp_bin)>64*1024))
			{
				QSPI_W25Qxx_BlockErase_64K(0x00);
				QSPI_W25Qxx_BlockErase_64K(1024*64);
			}
			else if((f_size(&fp_bin)<=64*3*1024)&&(f_size(&fp_bin)>64*2*1024))
			{
				QSPI_W25Qxx_BlockErase_64K(0x00);
				QSPI_W25Qxx_BlockErase_64K(1024*64);
				QSPI_W25Qxx_BlockErase_64K(1024*64*2);
			}
			else if((f_size(&fp_bin)<=64*4*1024)&&(f_size(&fp_bin)>64*3*1024))
			{
				QSPI_W25Qxx_BlockErase_64K(0x00);
				QSPI_W25Qxx_BlockErase_64K(1024*64);
				QSPI_W25Qxx_BlockErase_64K(1024*64*2);
				QSPI_W25Qxx_BlockErase_64K(1024*64*3);
			}
			temp[0x1c]=0x00;
			temp[0x1c+1]=0x00;
			temp[0x1c+2]=0x00;
			temp[0x1c+3]=0x00;
			QSPI_W25Qxx_WriteBuffer(temp,0x00,f_size(&fp_bin));
		}
	}
	f_close(&fp_bin);
	//f_close(&fp_txt);

}
