#include "iic_oled.h"
#include "font.h"          //ͷ�ļ�
#include "main.h"
#define u8 uint8_t
extern I2C_HandleTypeDef hi2c2;
//OLED���Դ�
//��Ÿ�ʽ����.
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
//���豸д��������
void OLED_WR_CMD(uint8_t cmd)
{
	HAL_I2C_Mem_Write(&hi2c2 ,0x78,0x00,I2C_MEMADD_SIZE_8BIT,&cmd,1,0x100);
}
void WriteCmd(void)
{
	OLED_WR_CMD(0xAE); //�ر���ʾ
	OLED_WR_CMD(0xD5); //����ʱ�ӷ�Ƶ����,��Ƶ��
	OLED_WR_CMD(80);   //[3:0],��Ƶ����;[7:4],��Ƶ��
	OLED_WR_CMD(0xA8); //��������·��
	OLED_WR_CMD(0X3F); //Ĭ��0X3F(1/64) 
	OLED_WR_CMD(0xD3); //������ʾƫ��
	OLED_WR_CMD(0X00); //Ĭ��Ϊ0
	OLED_WR_CMD(0x40); //������ʾ��ʼ�� [5:0],����.							    
	OLED_WR_CMD(0x8D); //��ɱ�����
	OLED_WR_CMD(0x14); //bit2������/�ر�
	OLED_WR_CMD(0x20); //�����ڴ��ַģʽ
	OLED_WR_CMD(0x02); //[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	OLED_WR_CMD(0xA1); //���ض�������,bit0:0,0->0;1,0->127;
	OLED_WR_CMD(0xC0); //����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
	OLED_WR_CMD(0xDA); //����COMӲ����������
	OLED_WR_CMD(0x12); //[5:4]����
	OLED_WR_CMD(0x81); //�Աȶ�����
	OLED_WR_CMD(0xEF); //1~255;Ĭ��0X7F (��������,Խ��Խ��)
	OLED_WR_CMD(0xD9); //����Ԥ�������
	OLED_WR_CMD(0xf1); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_CMD(0xDB); //����VCOMH ��ѹ����
	OLED_WR_CMD(0x30); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
	OLED_WR_CMD(0xA4); //ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	OLED_WR_CMD(0xA6); //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	    						   
	OLED_WR_CMD(0xAF); //������ʾ	 
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
//���豸д����
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
//��ʼ��oled��Ļ
void OLED_Init(void)
{ 	
	HAL_Delay(200);
	WriteCmd();
}
//����
//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
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
//����OLED��ʾ    
void OLED_Display_On(void)
{
	OLED_WR_CMD(0X8D);  //SET DCDC����
	OLED_WR_CMD(0X14);  //DCDC ON
	OLED_WR_CMD(0XAF);  //DISPLAY ON
}
//�ر�OLED��ʾ     
void OLED_Display_Off(void)
{
	OLED_WR_CMD(0X8D);  //SET DCDC����
	OLED_WR_CMD(0X10);  //DCDC OFF
	OLED_WR_CMD(0XAE);  //DISPLAY OFF
}		   			 
 
void OLED_On(void)  
{  
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_CMD(0xb0+i);    //����ҳ��ַ��0~7��
		OLED_WR_CMD(0x00);      //������ʾλ�á��е͵�ַ
		OLED_WR_CMD(0x10);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)
			OLED_WR_DATA(1); 
	} //������ʾ
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
//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//������Χ��.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}
//x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,���;1,���	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//������ʾ
}
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 12/16/24
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ		 
    for(t=0;t<csize;t++)
    {   
		if(size==12)temp=asc2_1206[chr][t]; 	 	//����1206����
		else if(size==16)temp=asc2_1608[chr][t];	//����1608����
		else if(size==24)temp=asc2_2412[chr][t];	//����2412����
		else return;								//û�е��ֿ�
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
//��ʾ�ַ���
//x,y:�������  
//size:�����С 
//*p:�ַ�����ʼ��ַ 
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size)
{	
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,1);	 
        x+=size/2;
        p++;
    }  
	
}

