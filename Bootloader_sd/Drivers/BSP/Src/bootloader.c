#include "bootloader.h"
#include "fatfs.h"
#include "sdmmc.h"
#include "usart.h"
#include "stdio.h"
#include "qspi_w25q64.h"
uint32_t crc32(uint32_t crcInit,uint8_t *buf, uint32_t size)
{
    uint32_t crc = crcInit;
    while(size > 0)
    {
        crc = crc32tab[*buf ^ (uint8_t)crc] ^ (crc >> 8);
        buf++;
        size--;
    }
    crc = crc ^ 0xFFFFFFFFU;
    return crc;
}
uint8_t temp[1024*64];
uint8_t text[100];
extern FATFS SDFatFS;    /* File system object for SD logical drive */
uint32_t crc_temp_new,crc_temp_old=0;
UINT bw;
UINT br;
extern char SDPath[4];   /* SD logical drive path */
void read_bin(void)
{
	FRESULT res;
	FIL fp_txt,fp_bin;
	uint16_t i=0;
    uint32_t num_64kb = 0;
	if( f_mkdir("sd_bin")== FR_OK || f_mkdir("sd_bin")== FR_EXIST)
	{
		res=f_open(&fp_bin, "sd_bin/demo.bin", FA_READ);
		num_64kb=f_size(&fp_bin)/(uint32_t)(1024*64);
		if (res == FR_OK)
		{
			QSPI_W25Qxx_ReadBuffer(temp, 0x00, 0x20);
			crc_temp_old = temp[0x1c]*256*256*256+temp[0x1c+1]*256*256+temp[0x1c+2]*256+temp[0x1c+3];
			res = f_read(&fp_bin, temp, 0x20, &br);
			crc_temp_new=temp[0x1c]*256*256*256+temp[0x1c+1]*256*256+temp[0x1c+2]*256+temp[0x1c+3];
		}
		 if(crc_temp_new != crc_temp_old)
		 {
			 f_lseek(&fp_bin,0);
			 for(i=0;i<num_64kb;i++)
			 {
				 res = f_read(&fp_bin, temp, 64*1024, &br);
				 QSPI_W25Qxx_BlockErase_64K(num_64kb*64*1024);
				 QSPI_W25Qxx_WriteBuffer(temp, num_64kb*64*1024, 64*1024);
			 }
			 if(f_size(&fp_bin)%(1024*64)!= 0)
			 {
				 res = f_read(&fp_bin, temp, f_size(&fp_bin)%(1024*64), &br);
				 QSPI_W25Qxx_BlockErase_64K(num_64kb*64*1024);
				 QSPI_W25Qxx_WriteBuffer(temp, num_64kb*64*1024, f_size(&fp_bin)%(1024*64));
			 }
		 }
	}
	f_close(&fp_bin);
	if( f_mkdir("log")== FR_OK || f_mkdir("log")== FR_EXIST)
	{
		res=f_open(&fp_txt, "log/log.txt", FA_READ | FA_WRITE| FA_OPEN_ALWAYS);
	}
	
	if (res == FR_OK)
	{	
		sprintf((char *)text,"new:0x%x,old:0x%x\n",crc_temp_new,crc_temp_old);
		f_lseek(&fp_txt,f_size(&fp_txt));//
		res = f_write(&fp_txt, text, 30, &bw);
	}
	f_close(&fp_txt);
	HAL_Delay(100);

}