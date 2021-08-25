#include "iic_oled.h"
#include "font.h"          //头文件
#include "main.h"
#define u8 uint8_t
extern I2C_HandleTypeDef hi2c2;
//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 		   
volatile uint8_t OLED_GRAM[128][8];
volatile uint8_t old_OLED_GRAM[128][8];
//向设备写控制命令
void OLED_WR_CMD(uint8_t cmd)
{
	HAL_I2C_Mem_Write(&hi2c2 ,0x78,0x00,I2C_MEMADD_SIZE_8BIT,&cmd,1,0x100);
}
void WriteCmd(void)
{
	OLED_WR_CMD(0xAE); //关闭显示
	OLED_WR_CMD(0xD5); //设置时钟分频因子,震荡频率
	OLED_WR_CMD(80);   //[3:0],分频因子;[7:4],震荡频率
	OLED_WR_CMD(0xA8); //设置驱动路数
	OLED_WR_CMD(0X3F); //默认0X3F(1/64) 
	OLED_WR_CMD(0xD3); //设置显示偏移
	OLED_WR_CMD(0X00); //默认为0
	OLED_WR_CMD(0x40); //设置显示开始行 [5:0],行数.							    
	OLED_WR_CMD(0x8D); //电荷泵设置
	OLED_WR_CMD(0x14); //bit2，开启/关闭
	OLED_WR_CMD(0x20); //设置内存地址模式
	OLED_WR_CMD(0x02); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	OLED_WR_CMD(0xA1); //段重定义设置,bit0:0,0->0;1,0->127;
	OLED_WR_CMD(0xC0); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	OLED_WR_CMD(0xDA); //设置COM硬件引脚配置
	OLED_WR_CMD(0x12); //[5:4]配置
	OLED_WR_CMD(0x81); //对比度设置
	OLED_WR_CMD(0xEF); //1~255;默认0X7F (亮度设置,越大越亮)
	OLED_WR_CMD(0xD9); //设置预充电周期
	OLED_WR_CMD(0xf1); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_CMD(0xDB); //设置VCOMH 电压倍率
	OLED_WR_CMD(0x30); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
	OLED_WR_CMD(0xA4); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	OLED_WR_CMD(0xA6); //设置显示方式;bit0:1,反相显示;0,正常显示	    						   
	OLED_WR_CMD(0xAF); //开启显示	 
}
void show_bad_apple(void)
{
  int i,j,k;
  for(i=0;i<6566;i++)
  {
    for(j=0;j<8;j++)
    {
      for(k=0;k<128;k++)
      {
//       OLED_GRAM[k][j]=bad_apple[i*128*8+j*128+k];
      }
    }
    OLED_Refresh_Gram();
//    HAL_Delay(50);
  }
}
//向设备写数据
void OLED_WR_DATA(uint8_t data)
{
	HAL_I2C_Mem_Write(&hi2c2 ,0x78,0x40,I2C_MEMADD_SIZE_8BIT,&data,1,0x100);
}
void OLED_Refresh_Gram(void)
{
	uint8_t i,n;	
	for(i=0;i<8;i++)  
	{  
		for(n=0;n<128;n++)
		{
      		if((OLED_GRAM[n][i])!=(old_OLED_GRAM[n][i]))
			{
				OLED_Set_Pos(n,i);
				OLED_WR_DATA(OLED_GRAM[n][i]); 
      		}
		}
	}   
	for(i=0;i<8;i++)  
	{     
		for(n=0;n<128;n++)
		{
			old_OLED_GRAM[n][i]=OLED_GRAM[n][i];
		}
	}   
}
//初始化oled屏幕
void OLED_Init(void)
{ 	
	HAL_Delay(200);
	WriteCmd();
}
//清屏
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)
{
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_CMD(0xb0+i);
		OLED_WR_CMD (0x00); 
		OLED_WR_CMD (0x10); 
		for(n=0;n<128;n++)
			OLED_WR_DATA(0);
	} 
}
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_CMD(0X8D);  //SET DCDC命令
	OLED_WR_CMD(0X14);  //DCDC ON
	OLED_WR_CMD(0XAF);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_CMD(0X8D);  //SET DCDC命令
	OLED_WR_CMD(0X10);  //DCDC OFF
	OLED_WR_CMD(0XAE);  //DISPLAY OFF
}		   			 
 
void OLED_On(void)  
{  
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_CMD(0xb0+i);    //设置页地址（0~7）
		OLED_WR_CMD(0x00);      //设置显示位置—列低地址
		OLED_WR_CMD(0x10);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)
			OLED_WR_DATA(1); 
	} //更新显示
}
unsigned int oled_pow(uint8_t m,uint8_t n)
{
	unsigned int result=1;	 
	while(n--)result*=m;    
	return result;
}
void OLED_Set_Pos(uint8_t x, uint8_t y) 
{ 	
	OLED_WR_CMD(0xb0+y);
	OLED_WR_CMD(((x&0xf0)>>4)|0x10);
	OLED_WR_CMD(x&0x0f);
} 
//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}
//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//更新显示
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 12/16/24
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
	chr=chr-' ';//得到偏移后的值		 
    for(t=0;t<csize;t++)
    {   
		if(size==12)temp=asc2_1206[chr][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[chr][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[chr][t];	//调用2412字体
		else return;								//没有的字库
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}
//显示字符串
//x,y:起点坐标  
//size:字体大小 
//*p:字符串起始地址 
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size)
{	
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,1);	 
        x+=size/2;
        p++;
    }  
	
}

