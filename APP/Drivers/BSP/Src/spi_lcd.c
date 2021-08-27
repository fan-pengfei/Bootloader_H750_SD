#include "spi_lcd.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "lcd_font.h"
#include "rng.h"
extern SPI_HandleTypeDef hspi1;
extern RNG_HandleTypeDef hrng;
//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;
uint8_t gram[LCD_W * LCD_H *2] = {};
uint8_t gram_old[LCD_W * LCD_H *2] = {};
//画笔颜色,背景颜色
uint16_t POINT_COLOR = 0x0000, BACK_COLOR = 0xFFFF;
uint16_t DeviceCode;
uint8_t SPI_WriteByte(uint8_t data)
{
    return HAL_SPI_Transmit(&hspi1, &data, 1, 1000);
}
void LCD_WR_REG(uint8_t data);
void Update_GRAM_fast(void) //更新显存
{
   unsigned int i, j;
	uint8_t data[2];
	LCD_SetWindows(0, 0, lcddev.width - 1, lcddev.height - 1); //窗口为全屏
    LCD_RS_SET;
	LCD_CS_CLR;
    for (i = 0; i < lcddev.height; i++)
    {
		HAL_SPI_Transmit(&hspi1, gram+2*(i*lcddev.width), 2*lcddev.width, 1000);
    }
    for (i = 0; i < 240 * 320*2; i++)
    {
        gram_old[i] = gram[i];
    }
	LCD_CS_SET;
}
void show_rng(void) //更新显存
{
	long i;
	int a;
    for (i = 0; i < 240 * 320*2; i++)
    {	
		srand((unsigned)i+HAL_GetTick());
		a = rand();
        gram[i] = a%256;
    }
	Update_GRAM_fast();
}
	
//void Update_GRAM(void) //更新显存
//{
//    unsigned int i, j;
//	uint8_t data[2];
//    LCD_RS_SET;
//    for (i = 0; i < lcddev.height; i++)
//    {
//        for (j = 0; j < lcddev.width; j++)
//        {
//            if (gram_old[i * lcddev.width + j] != gram[i * lcddev.width + j])
//            {
//				LCD_CS_CLR;
//				LCD_RS_CLR;
//				SPI_WriteByte(lcddev.setxcmd);
//				LCD_CS_SET;
//				
//				LCD_CS_CLR;
//				LCD_RS_SET;
//				data[0]=j>>8;
//				data[1]=j&0XFF;
//				HAL_SPI_Transmit(&hspi1, data, 2, 1000);
//				LCD_CS_SET;
//				
//				
//				LCD_CS_CLR;
//				LCD_RS_CLR;
//				SPI_WriteByte(lcddev.setycmd);
//				LCD_CS_SET;
//				
//				LCD_CS_CLR;
//				LCD_RS_SET;
//				data[0]=i>>8;
//				data[1]=i&0XFF;
//				HAL_SPI_Transmit(&hspi1, data, 2, 1000);
//				LCD_CS_SET;

//				LCD_CS_CLR;
//				LCD_RS_CLR;
//				SPI_WriteByte(lcddev.wramcmd);
//				LCD_CS_SET;
//				
//				LCD_CS_CLR;
//				LCD_RS_SET;
//				data[0]=gram[i * lcddev.width + j] >> 8;
//				data[1]=gram[i * lcddev.width + j];
//				HAL_SPI_Transmit(&hspi1, data, 2, 1000);
//				LCD_CS_SET;
//            }
//        }
//    }
//    for (i = 0; i < 240 * 320; i++)
//    {
//        gram_old[i] = gram[i];
//    }
//}
/*****************************************************************************
 * @name       :void LCD_WR_REG(uint8_t data)
 * @date       :2018-08-09 
 * @function   :Write an 8-bit command to the LCD screen
 * @parameters :data:Command value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_REG(uint8_t data)
{
    LCD_CS_CLR;
    LCD_RS_CLR;
    SPI_WriteByte(data);
    LCD_CS_SET;
}

/*****************************************************************************
 * @name       :void LCD_WR_DATA(uint8_t data)
 * @date       :2018-08-09 
 * @function   :Write an 8-bit data to the LCD screen
 * @parameters :data:data value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_DATA(uint8_t data)
{
    LCD_CS_CLR;
    LCD_RS_SET;
    SPI_WriteByte(data);
    LCD_CS_SET;
}

/*****************************************************************************
 * @name       :void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
 * @date       :2018-08-09 
 * @function   :Write data into registers
 * @parameters :LCD_Reg:Register address
                LCD_RegValue:Data to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
    LCD_WR_REG(LCD_Reg);
    LCD_WR_DATA(LCD_RegValue);
}

/*****************************************************************************
 * @name       :void LCD_WriteRAM_Prepare(void)
 * @date       :2018-08-09 
 * @function   :Write GRAM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void LCD_WriteRAM_Prepare(void)
{
    LCD_WR_REG(lcddev.wramcmd);
}

/*****************************************************************************
 * @name       :void Lcd_WriteData_16Bit(uint16_t Data)
 * @date       :2018-08-09 
 * @function   :Write an 16-bit command to the LCD screen
 * @parameters :Data:Data to be written
 * @retvalue   :None
******************************************************************************/
void Lcd_WriteData_16Bit(uint16_t Data)
{
    LCD_CS_CLR;
    LCD_RS_SET;
    SPI_WriteByte(Data >> 8);
    SPI_WriteByte(Data);
    LCD_CS_SET;
}

/*****************************************************************************
 * @name       :void LCD_DrawPoint(uint16_t x,uint16_t y)
 * @date       :2018-08-09 
 * @function   :Write a pixel data at a specified location
 * @parameters :x:the x coordinate of the pixel
                y:the y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/
void LCD_DrawPoint(uint16_t x, uint16_t y)
{
    LCD_SetCursor(x, y); //设置光标位置
    Lcd_WriteData_16Bit(POINT_COLOR);
}

/*****************************************************************************
 * @name       :void LCD_Clear(uint16_t Color)
 * @date       :2018-08-09 
 * @function   :Full screen filled LCD screen
 * @parameters :color:Filled color
 * @retvalue   :None
******************************************************************************/
void LCD_Clear(uint16_t Color)
{
    unsigned int i, m;
    for (i = 0; i < lcddev.height; i++)
    {
        for (m = 0; m < lcddev.width; m++)
        {
            gram[2*(i * lcddev.width + m)] = Color>>8;
            gram[2*(i * lcddev.width + m)+1] = Color;
        }
    }
    //Update_GRAM();
	Update_GRAM_fast();
}
/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 
 * @function   :Reset LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void LCD_RESET(void)
{
    LCD_RST_CLR;
    HAL_Delay(100);
    LCD_RST_SET;
    HAL_Delay(50);
}

/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void LCD_Init(void)
{
	long i;
    LCD_RESET(); //LCD 复位
                 //*************2.8inch ILI9341初始化**********//
    LCD_WR_REG(0xCF);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC9); //C1
    LCD_WR_DATA(0X30);
    LCD_WR_REG(0xED);
    LCD_WR_DATA(0x64);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0X12);
    LCD_WR_DATA(0X81);
    LCD_WR_REG(0xE8);
    LCD_WR_DATA(0x85);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x7A);
    LCD_WR_REG(0xCB);
    LCD_WR_DATA(0x39);
    LCD_WR_DATA(0x2C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x34);
    LCD_WR_DATA(0x02);
    LCD_WR_REG(0xF7);
    LCD_WR_DATA(0x20);
    LCD_WR_REG(0xEA);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0xC0);  //Power control
    LCD_WR_DATA(0x1B); //VRH[5:0]
    LCD_WR_REG(0xC1);  //Power control
    LCD_WR_DATA(0x00); //SAP[2:0];BT[3:0] 01
    LCD_WR_REG(0xC5);  //VCM control
    LCD_WR_DATA(0x30); //3F
    LCD_WR_DATA(0x30); //3C
    LCD_WR_REG(0xC7);  //VCM control2
    LCD_WR_DATA(0XB7);
    LCD_WR_REG(0x36); // Memory Access Control
    LCD_WR_DATA(0x08);
    LCD_WR_REG(0x3A);
    LCD_WR_DATA(0x55);
    LCD_WR_REG(0xB1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x1A);
    LCD_WR_REG(0xB6); // Display Function Control
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0xA2);
    LCD_WR_REG(0xF2); // 3Gamma Function Disable
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0x26); //Gamma curve selected
    LCD_WR_DATA(0x01);
    LCD_WR_REG(0xE0); //Set Gamma
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x2A);
    LCD_WR_DATA(0x28);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x54);
    LCD_WR_DATA(0XA9);
    LCD_WR_DATA(0x43);
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0XE1); //Set Gamma
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x15);
    LCD_WR_DATA(0x17);
    LCD_WR_DATA(0x07);
    LCD_WR_DATA(0x11);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x2B);
    LCD_WR_DATA(0x56);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x3F);
    LCD_WR_DATA(0x3F);
    LCD_WR_DATA(0x0F);
    LCD_WR_REG(0x2B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x3f);
    LCD_WR_REG(0x2A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xef);
    LCD_WR_REG(0x11); //Exit Sleep
    HAL_Delay(120);
    LCD_WR_REG(0x29); //display on
	for (i = 0; i < 240 * 320; i++)
    {
        gram[i] = 0xff;
    }
    LCD_direction(USE_HORIZONTAL); //设置LCD显示方向
    LCD_Clear(WHITE);              //清全屏白色
    LCD_Clear(RED);                //清全屏白色
    LCD_Clear(WHITE);              //清全屏白色
}

/*****************************************************************************
 * @name       :void LCD_SetWindows(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd)
 * @date       :2018-08-09 
 * @function   :Setting LCD display window
 * @parameters :xStar:the bebinning x coordinate of the LCD display window
								yStar:the bebinning y coordinate of the LCD display window
								xEnd:the endning x coordinate of the LCD display window
								yEnd:the endning y coordinate of the LCD display window
 * @retvalue   :None
******************************************************************************/
void LCD_SetWindows(uint16_t xStar, uint16_t yStar, uint16_t xEnd, uint16_t yEnd)
{
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(xStar >> 8);
    LCD_WR_DATA(0x00FF & xStar);
    LCD_WR_DATA(xEnd >> 8);
    LCD_WR_DATA(0x00FF & xEnd);

    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(yStar >> 8);
    LCD_WR_DATA(0x00FF & yStar);
    LCD_WR_DATA(yEnd >> 8);
    LCD_WR_DATA(0x00FF & yEnd);

    LCD_WriteRAM_Prepare(); //开始写入GRAM
}

/*****************************************************************************
 * @name       :void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
 * @date       :2018-08-09 
 * @function   :Set coordinate value
 * @parameters :Xpos:the  x coordinate of the pixel
								Ypos:the  y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
    LCD_SetWindows(Xpos, Ypos, Xpos, Ypos);
}

/*****************************************************************************
 * @name       :void LCD_direction(uint8_t direction)
 * @date       :2018-08-09 
 * @function   :Setting the display direction of LCD screen
 * @parameters :direction:0-0 degree
                          1-90 degree
													2-180 degree
													3-270 degree
 * @retvalue   :None
******************************************************************************/
void LCD_direction(uint8_t direction)
{
    lcddev.setxcmd = 0x2A;
    lcddev.setycmd = 0x2B;
    lcddev.wramcmd = 0x2C;
    switch (direction)
    {
    case 0:
        lcddev.width = LCD_W;
        lcddev.height = LCD_H;
        LCD_WriteReg(0x36, (1 << 3) | (0 << 6) | (0 << 7)); //BGR==1,MY==0,MX==0,MV==0
        break;
    case 1:
        lcddev.width = LCD_H;
        lcddev.height = LCD_W;
        LCD_WriteReg(0x36, (1 << 3) | (0 << 7) | (1 << 6) | (1 << 5)); //BGR==1,MY==1,MX==0,MV==1
        break;
    case 2:
        lcddev.width = LCD_W;
        lcddev.height = LCD_H;
        LCD_WriteReg(0x36, (1 << 3) | (1 << 6) | (1 << 7)); //BGR==1,MY==0,MX==0,MV==0
        break;
    case 3:
        lcddev.width = LCD_H;
        lcddev.height = LCD_W;
        LCD_WriteReg(0x36, (1 << 3) | (1 << 7) | (1 << 5)); //BGR==1,MY==1,MX==0,MV==1
        break;
    default:
        break;
    }
}

/*******************************************************************
 * @name       :void GUI_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
 * @date       :2018-08-09 
 * @function   :draw a point in LCD screen
 * @parameters :x:the x coordinate of the point
                y:the y coordinate of the point
								color:the color value of the point
 * @retvalue   :None
********************************************************************/
void GUI_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    LCD_SetCursor(x, y); //设置光标位置
    Lcd_WriteData_16Bit(color);
}
void LCD_ShowChar(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t num, uint8_t size, uint8_t mode)
{
    uint8_t temp;
    uint8_t pos, t;
    uint16_t colortemp = POINT_COLOR;

    num = num - ' ';                                      //得到偏移后的值
    LCD_SetWindows(x, y, x + size / 2 - 1, y + size - 1); //设置单个文字显示窗口
    if (!mode)                                            //非叠加方式
    {
        for (pos = 0; pos < size; pos++)
        {
            if (size == 12)
                temp = lcd_asc2_1206[num][pos]; //调用1206字体
            else
                temp = lcd_asc2_1608[num][pos]; //调用1608字体
            for (t = 0; t < size / 2; t++)
            {
                if (temp & 0x01)
                    Lcd_WriteData_16Bit(fc);
                else
                    Lcd_WriteData_16Bit(bc);
                temp >>= 1;
            }
        }
    }
    else //叠加方式
    {
        for (pos = 0; pos < size; pos++)
        {
            if (size == 12)
                temp = lcd_asc2_1206[num][pos]; //调用1206字体
            else
                temp = lcd_asc2_1608[num][pos]; //调用1608字体
            for (t = 0; t < size / 2; t++)
            {
                POINT_COLOR = fc;
                if (temp & 0x01)
                    LCD_DrawPoint(x + t, y + pos); //画一个点
                temp >>= 1;
            }
        }
    }
    POINT_COLOR = colortemp;
    LCD_SetWindows(0, 0, lcddev.width - 1, lcddev.height - 1); //恢复窗口为全屏
}
void LCD_ShowString(uint16_t x, uint16_t y, uint8_t size, uint8_t *p, uint8_t mode, uint16_t point_color, uint16_t back_color)
{
    while ((*p <= '~') && (*p >= ' ')) //判断是不是非法字符!
    {
        if (x > (lcddev.width - 1) || y > (lcddev.height - 1))
            return;
        LCD_ShowChar(x, y, point_color, back_color, *p, size, mode);
        x += size / 2;
        p++;
    }
}
