				LCD_RS_CLR;
				SPI_WriteByte(lcddev.setxcmd);
				LCD_RS_SET;
				SPI_WriteByte(i>>8);
				SPI_WriteByte(i&0XFF);
				
				LCD_RS_CLR;
				SPI_WriteByte(lcddev.setycmd);
				LCD_RS_SET;
				SPI_WriteByte(j>>8);
				SPI_WriteByte(j&0XFF);
				
				LCD_RS_CLR;
				SPI_WriteByte(lcddev.wramcmd);; //????GRAM
				
				LCD_RS_SET;
				SPI_WriteByte(gram[i * lcddev.width + j] >> 8);
				SPI_WriteByte(gram[i * lcddev.width + j]);