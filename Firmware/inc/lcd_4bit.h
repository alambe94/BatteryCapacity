/*
 * lcd_4bit.h
 *
 *  Created on: 12-Apr-2016
 *      Author: medprime4
 */


#ifndef LCD_4BIT_H_
#define LCD_4BIT_H_
#include "stm8s.h"
#include "stm8s_gpio.h"

#define RS GPIO_PIN_4  //GPIOC
#define EN GPIO_PIN_3  //GPIOB


#define D7 GPIO_PIN_7  //GPIOC
#define D6 GPIO_PIN_6  //GPIOC
#define D5 GPIO_PIN_5  //GPIOC
#define D4 GPIO_PIN_4  //GPIOC

#define Lcd_Port GPIOC

void Lcd_Enable(void);
void Lcd_Data(unsigned char c);
void Lcd_Cmd(unsigned char c);
void Lcd_Init();
void Lcd_String(char *p);

#endif /* LCD_4BIT_H_ */
