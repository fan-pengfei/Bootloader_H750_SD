#include "touch.h"
#include "stdlib.h"
#include "math.h"
//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//����Ӳ������Ƭ��STM32F103RBT6,����ԭ��MiniSTM32������,��Ƶ72MHZ
//QDtech-TFTҺ������ for STM32 IOģ��
//xiao��@ShenZhen QDtech co.,LTD
//��˾��վ:www.qdtft.com
//�Ա���վ��http://qdtech.taobao.com
//��˾�ṩ����֧�֣��κμ������⻶ӭ��ʱ����ѧϰ
//�̻�(����) :+86 0755-23594567
//�ֻ�:15989313508���빤��
//����:goodtft@126.com
//Skype:QDtech2008
//����֧��QQ:3002773612
//��������QQȺ:324828016
//��������:2013/5/13
//�汾��V2.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������ȫ�����Ӽ������޹�˾ 2013-2019
//All rights reserved
/****************************************************************************************************

//====================================��Դ����==========================================//
//VCC        ---�� 5V��3.3V��Դ
//GND        ---�� ��

//===============================Һ���������߽���======================================//
//CS	       ---��PB11	//Һ����Ƭѡ�ź�
//RESET	     ---��PB12	//Һ������λ�ź�
//DC/RS	     ---��PB10	//�Ĵ���/����ѡ���ź�
//SDI(MOSI)	 ---��PB15	//SPI����д����
//SCK	       ---��PB13	//SPI����ʱ���ź�
//LED	       ---��PB9	 //��������ź�(�ߵ�ƽ����)�������Ҫ���ƿɽ�VCC����
//SDO(MISO)  ---��PB14 //SPI���ź��������ȡ�ɲ���

//===============================����������========================================//
//��ʹ�ô�������ģ�鱾������������ɲ�����  
//T_CLK	     ---��PC0	 //����SPI����ʱ��
//T_CS	     ---��PC13 //����ICƬѡ
//T_DIN	     ---��PC3	//����SPI��������
//T_DO	     ---��PC2	//����SPI�������
//T_IRQ	     ---��PC1	//�������ж��ź�

*************************************************************************************************/

_m_tp_dev tp_dev =
    {
        TP_Init,
        //	TP_Scan,
        //	TP_Adjust,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
};
//Ĭ��Ϊtouchtype=0������.
uint8_t CMD_RDX = 0XD0;
uint8_t CMD_RDY = 0X90;

//SPIд����
//������ICд��1byte����
//num:Ҫд�������
void TP_Write_Byte(uint8_t num)
{
    uint8_t count = 0;
    for (count = 0; count < 8; count++)
    {
        if (num & 0x80)
            TDIN(1);
        else
            TDIN(0);
        num <<= 1;
        TCLK(0);
        TCLK(1); //��������Ч
    }
}
//SPI������
//�Ӵ�����IC��ȡadcֵ
//CMD:ָ��
//����ֵ:����������
uint16_t TP_Read_AD(uint8_t CMD)
{
    uint8_t count = 0;
    uint16_t Num = 0;
    TCLK(0);            //������ʱ��
    TDIN(0);            //����������
    TCS(0);             //ѡ�д�����IC
    TP_Write_Byte(CMD); //����������
    Delay_us(6);       //ADS7846��ת��ʱ���Ϊ6us
    TCLK(0);
    Delay_us(1);
    TCLK(1); //��1��ʱ�ӣ����BUSY
    TCLK(0);
    for (count = 0; count < 16; count++) //����16λ����,ֻ�и�12λ��Ч
    {
        Num <<= 1;
        TCLK(0); //�½�����Ч
        TCLK(1);
        if (DOUT)
            Num++;
    }
    Num >>= 4; //ֻ�и�12λ��Ч.
    TCS(1);    //�ͷ�Ƭѡ
    return (Num);
}
//��ȡһ������ֵ(x����y)
//������ȡREAD_TIMES������,����Щ������������,
//Ȼ��ȥ����ͺ����LOST_VAL����,ȡƽ��ֵ
//xy:ָ�CMD_RDX/CMD_RDY��
//����ֵ:����������
#define READ_TIMES 5 //��ȡ����
#define LOST_VAL 1   //����ֵ
uint16_t TP_Read_XOY(uint8_t xy)
{
    uint16_t i, j;
    uint16_t buf[READ_TIMES];
    uint16_t sum = 0;
    uint16_t temp;
    for (i = 0; i < READ_TIMES; i++)
        buf[i] = TP_Read_AD(xy);
    for (i = 0; i < READ_TIMES - 1; i++) //����
    {
        for (j = i + 1; j < READ_TIMES; j++)
        {
            if (buf[i] > buf[j]) //��������
            {
                temp = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }
    sum = 0;
    for (i = LOST_VAL; i < READ_TIMES - LOST_VAL; i++)
        sum += buf[i];
    temp = sum / (READ_TIMES - 2 * LOST_VAL);
    return temp;
}
//��ȡx,y����
//��Сֵ��������100.
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
uint8_t TP_Read_XY(uint16_t *x, uint16_t *y)
{
    uint16_t xtemp, ytemp;
    xtemp = TP_Read_XOY(CMD_RDX);
    ytemp = TP_Read_XOY(CMD_RDY);
    //if(xtemp<100||ytemp<100)return 0;//����ʧ��
    *x = xtemp;
    *y = ytemp;
    return 1; //�����ɹ�
}
//����2�ζ�ȡ������IC,�������ε�ƫ��ܳ���
//ERR_RANGE,��������,����Ϊ������ȷ,�����������.
//�ú����ܴ�����׼ȷ��
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
#define ERR_RANGE 50 //��Χ
uint8_t TP_Read_XY2(uint16_t *x, uint16_t *y)
{
    uint16_t x1, y1;
    uint16_t x2, y2;
    uint8_t flag;
    flag = TP_Read_XY(&x1, &y1);
    if (flag == 0)
        return (0);
    flag = TP_Read_XY(&x2, &y2);
    if (flag == 0)
        return (0);
    if (((x2 <= x1 && x1 < x2 + ERR_RANGE) || (x1 <= x2 && x2 < x1 + ERR_RANGE)) //ǰ�����β�����+-50��
        && ((y2 <= y1 && y1 < y2 + ERR_RANGE) || (y1 <= y2 && y2 < y1 + ERR_RANGE)))
    {
        *x = (x1 + x2) / 2;
        *y = (y1 + y2) / 2;
        return 1;
    }
    else
        return 0;
}
//��������ʼ��
//����ֵ:0,û�н���У׼
//       1,���й�У׼
//��������ʼ��
//����ֵ:0,û�н���У׼
//       1,���й�У׼
uint8_t TP_Init(void)
{
    TP_Read_XY(&tp_dev.x, &tp_dev.y); //��һ�ζ�ȡ��ʼ��
    return 1;
}


