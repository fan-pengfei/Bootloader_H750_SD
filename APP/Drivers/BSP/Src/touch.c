#include "touch.h"
#include "stdlib.h"
#include "math.h"
//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//测试硬件：单片机STM32F103RBT6,正点原子MiniSTM32开发板,主频72MHZ
//QDtech-TFT液晶驱动 for STM32 IO模拟
//xiao冯@ShenZhen QDtech co.,LTD
//公司网站:www.qdtft.com
//淘宝网站：http://qdtech.taobao.com
//我司提供技术支持，任何技术问题欢迎随时交流学习
//固话(传真) :+86 0755-23594567
//手机:15989313508（冯工）
//邮箱:goodtft@126.com
//Skype:QDtech2008
//技术支持QQ:3002773612
//技术交流QQ群:324828016
//创建日期:2013/5/13
//版本：V2.0
//版权所有，盗版必究。
//Copyright(C) 深圳市全动电子技术有限公司 2013-2019
//All rights reserved
/****************************************************************************************************

//====================================电源接线==========================================//
//VCC        ---接 5V或3.3V电源
//GND        ---接 地

//===============================液晶屏控制线接线======================================//
//CS	       ---接PB11	//液晶屏片选信号
//RESET	     ---接PB12	//液晶屏复位信号
//DC/RS	     ---接PB10	//寄存器/数据选择信号
//SDI(MOSI)	 ---接PB15	//SPI总线写数据
//SCK	       ---接PB13	//SPI总线时钟信号
//LED	       ---接PB9	 //背光控制信号(高电平点亮)如果不需要控制可接VCC常亮
//SDO(MISO)  ---接PB14 //SPI读信号如无需读取可不接

//===============================触摸屏接线========================================//
//不使用触摸或者模块本身不带触摸，则可不连接  
//T_CLK	     ---接PC0	 //触摸SPI总线时钟
//T_CS	     ---接PC13 //触摸IC片选
//T_DIN	     ---接PC3	//触摸SPI总线输入
//T_DO	     ---接PC2	//触摸SPI总线输出
//T_IRQ	     ---接PC1	//触摸屏中断信号

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
//默认为touchtype=0的数据.
uint8_t CMD_RDX = 0XD0;
uint8_t CMD_RDY = 0X90;

//SPI写数据
//向触摸屏IC写入1byte数据
//num:要写入的数据
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
        TCLK(1); //上升沿有效
    }
}
//SPI读数据
//从触摸屏IC读取adc值
//CMD:指令
//返回值:读到的数据
uint16_t TP_Read_AD(uint8_t CMD)
{
    uint8_t count = 0;
    uint16_t Num = 0;
    TCLK(0);            //先拉低时钟
    TDIN(0);            //拉低数据线
    TCS(0);             //选中触摸屏IC
    TP_Write_Byte(CMD); //发送命令字
    Delay_us(6);       //ADS7846的转换时间最长为6us
    TCLK(0);
    Delay_us(1);
    TCLK(1); //给1个时钟，清除BUSY
    TCLK(0);
    for (count = 0; count < 16; count++) //读出16位数据,只有高12位有效
    {
        Num <<= 1;
        TCLK(0); //下降沿有效
        TCLK(1);
        if (DOUT)
            Num++;
    }
    Num >>= 4; //只有高12位有效.
    TCS(1);    //释放片选
    return (Num);
}
//读取一个坐标值(x或者y)
//连续读取READ_TIMES次数据,对这些数据升序排列,
//然后去掉最低和最高LOST_VAL个数,取平均值
//xy:指令（CMD_RDX/CMD_RDY）
//返回值:读到的数据
#define READ_TIMES 5 //读取次数
#define LOST_VAL 1   //丢弃值
uint16_t TP_Read_XOY(uint8_t xy)
{
    uint16_t i, j;
    uint16_t buf[READ_TIMES];
    uint16_t sum = 0;
    uint16_t temp;
    for (i = 0; i < READ_TIMES; i++)
        buf[i] = TP_Read_AD(xy);
    for (i = 0; i < READ_TIMES - 1; i++) //排序
    {
        for (j = i + 1; j < READ_TIMES; j++)
        {
            if (buf[i] > buf[j]) //升序排列
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
//读取x,y坐标
//最小值不能少于100.
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
uint8_t TP_Read_XY(uint16_t *x, uint16_t *y)
{
    uint16_t xtemp, ytemp;
    xtemp = TP_Read_XOY(CMD_RDX);
    ytemp = TP_Read_XOY(CMD_RDY);
    //if(xtemp<100||ytemp<100)return 0;//读数失败
    *x = xtemp;
    *y = ytemp;
    return 1; //读数成功
}
//连续2次读取触摸屏IC,且这两次的偏差不能超过
//ERR_RANGE,满足条件,则认为读数正确,否则读数错误.
//该函数能大大提高准确度
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
#define ERR_RANGE 50 //误差范围
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
    if (((x2 <= x1 && x1 < x2 + ERR_RANGE) || (x1 <= x2 && x2 < x1 + ERR_RANGE)) //前后两次采样在+-50内
        && ((y2 <= y1 && y1 < y2 + ERR_RANGE) || (y1 <= y2 && y2 < y1 + ERR_RANGE)))
    {
        *x = (x1 + x2) / 2;
        *y = (y1 + y2) / 2;
        return 1;
    }
    else
        return 0;
}
//触摸屏初始化
//返回值:0,没有进行校准
//       1,进行过校准
//触摸屏初始化
//返回值:0,没有进行校准
//       1,进行过校准
uint8_t TP_Init(void)
{
    TP_Read_XY(&tp_dev.x, &tp_dev.y); //第一次读取初始化
    return 1;
}


