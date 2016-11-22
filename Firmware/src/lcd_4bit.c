#include "lcd_4bit.h"
#include "millis.h"


void Lcd_Enable(void) {
  GPIO_WriteHigh(Lcd_Port,EN);
  delay_ms(1);
  GPIO_WriteLow(Lcd_Port,EN);
  delay_ms(1);

}

void Lcd_Data(unsigned char c) {
	unsigned char temp;
	temp = c;
  GPIO_WriteHigh(GPIOB,RS);
  delay_ms(1);
	GPIO_Write(Lcd_Port,(((temp) & 0xF0) | (GPIO_ReadInputData(Lcd_Port) & 0x0F)));
	Lcd_Enable();
	GPIO_Write(Lcd_Port,(((temp)<<4 & 0xF0) | (GPIO_ReadInputData(Lcd_Port) & 0x0F)));
	Lcd_Enable();

}

void Lcd_Cmd(unsigned char c) {
	unsigned char temp;
	temp = c;
  GPIO_WriteLow(GPIOB,RS);
  delay_ms(1);
	GPIO_Write(Lcd_Port,(((temp) & 0xF0) | (GPIO_ReadInputData(Lcd_Port) & 0x0F)));
	Lcd_Enable();
	GPIO_Write(Lcd_Port,(((temp)<<4 & 0xF0) | (GPIO_ReadInputData(Lcd_Port) & 0x0F)));
	Lcd_Enable();
}

void Lcd_Init() {
GPIO_Init(Lcd_Port,GPIO_PIN_7,GPIO_MODE_OUT_PP_LOW_FAST);
GPIO_Init(Lcd_Port,GPIO_PIN_6,GPIO_MODE_OUT_PP_LOW_FAST);
GPIO_Init(Lcd_Port,GPIO_PIN_5,GPIO_MODE_OUT_PP_LOW_FAST);
GPIO_Init(Lcd_Port,GPIO_PIN_4,GPIO_MODE_OUT_PP_LOW_FAST);

GPIO_Init(Lcd_Port,GPIO_PIN_3,GPIO_MODE_OUT_PP_LOW_FAST); //EN
GPIO_Init(GPIOB,GPIO_PIN_4,GPIO_MODE_OUT_PP_LOW_FAST); //RS
 

	delay_ms(15);
	Lcd_Cmd(0x30);
	delay_ms(10);
	Lcd_Cmd(0x30);
	delay_ms(1);
	Lcd_Cmd(0x30);
	delay_ms(1);
	Lcd_Cmd(0x02);
	delay_ms(1);
	Lcd_Cmd(0x28);
	delay_ms(1);
	Lcd_Cmd(0x0C);
	delay_ms(1);
	Lcd_Cmd(0x01);
	delay_ms(1);
	Lcd_Cmd(0x06);
	delay_ms(1);

}

void Lcd_String(char *p) {
	while (*p)
		Lcd_Data(*p++);
}

