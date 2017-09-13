/**
  ******************************************************************************
  * @file    Project/main.c 
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    18-November-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 


/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "lcd_4bit.h"
#include "millis.h"


/* Private defines -----------------------------------------------------------*/
#define True 1
#define False 0


#define Button GPIO_PIN_1
#define Nmos GPIO_PIN_4
#define A0 ADC1_CHANNEL_5
#define A1 ADC1_CHANNEL_6
 
float Capacity = 0.0; // Capacity in mAh
float Res_Value = 10.0;  // Resistor Value in Ohm
float Vcc = 5.1; // Voltage of Arduino 5V pin ( Mesured by Multimeter )
float Current = 0.0; // Current in Amp
float mA=0;         // Current in mA
float Bat_Volt = 0.0;  // Battery Voltage 
float Res_Volt = 0.0;  // Voltage at lower end of the Resistor 
float Bat_High = 4.3; // Battery High Voltage
float Bat_Low  = 2.75; // Discharge Cut Off Voltage
unsigned long previousMillis = 0; // Previous time in ms
unsigned long millisPassed = 0;  // Current time in ms
float sample1 =0;
float sample2= 0;
int x = 0;
int row = 0;
uint8_t Button_Pressed = False;
uint8_t Button_Pressed_Count = 0;
uint8_t Measurement_Complete = False;
uint8_t Check_Complete =False;
uint8_t Battery_Ok = False;


int i;




/* Private function prototypes -----------------------------------------------*/
unsigned int Get_ADC_Value(ADC1_Channel_TypeDef ADC_Channel);
void Digit_Extract_Decimal(int32_t num);

/* Private functions ---------------------------------------------------------*/

unsigned int Get_ADC_Value(ADC1_Channel_TypeDef ADC_Channel)
{
        uint16_t temp = 0;
	ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, ADC_Channel,ADC1_ALIGN_RIGHT);
	ADC1_StartConversion();
	while((ADC1_GetFlagStatus(ADC1_FLAG_EOC)!=RESET));
	temp=ADC1_GetConversionValue();
        ADC1_ClearFlag(ADC1_FLAG_EOC);
        return temp;

}


void Digit_Extract_Decimal(int32_t num) {
	char str[10];
	int i = 0;
	if (num < 0) {
		Lcd_Data('-');
		num *= -1;
	}
	do str[i++] = num % 10 + '0';
	while ((num /= 10) > 0);
	for (i--; i >= 0; i--)
		Lcd_Data(str[i]);
}



void LCD_DisplayFloatNumber(float float_value)

{

	uint32_t temp_value=0;

	/* Dirty hack to support the floating point by extracting the integer and fractional part.

      1.Type cast the number to int to get the integer part.

	  2.Display the extracted integer part followed by a decimal point(.).

	  3.Later the integer part is made zero by subtracting with the extracted integer value.

	  4.Finally the fractional part is multiplied by 100000 to support 6-digit precision */



	temp_value = (uint32_t) float_value;

	Digit_Extract_Decimal(temp_value);

	Lcd_Data('.');

	float_value = (float_value - temp_value);
        
    
        if(float_value<0.1)
        {
        //   if(float_value<0.01)
       // {
       //   	Lcd_Data('0');
       //         Lcd_Data('0');
       // }
        //   else
        //   {
            
          	Lcd_Data('0');
        //   }
        }
        
       
        float_value=float_value*100;
        temp_value = (uint32_t) float_value;

	Digit_Extract_Decimal(temp_value);

}


INTERRUPT_HANDLER(EXTI_PORTA_IRQHandler, 3)
{
  if(GPIO_ReadInputPin(GPIOA,Button)==0)
  {
    Button_Pressed_Count++;
    if(Button_Pressed_Count>2)
    {
      Button_Pressed_Count=0;
      Check_Complete=False;
    }
  }
}


int main()
{

 CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); //f_master = HSI = 16Mhz
 Millis_Init();
 Lcd_Init();
 GPIO_Init(GPIOD,Nmos,GPIO_MODE_OUT_PP_LOW_FAST);
 GPIO_Init(GPIOD,GPIO_PIN_5,GPIO_MODE_IN_FL_NO_IT);  //Adc1 GPIO
 GPIO_Init(GPIOD,GPIO_PIN_6,GPIO_MODE_IN_FL_NO_IT);  //Adc2 GPIO
 
 
 GPIO_Init(GPIOA,Button,GPIO_MODE_IN_PU_NO_IT);  //Button Interrupt
 //EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOA, EXTI_SENSITIVITY_FALL_LOW);

 
ADC1_DeInit();
ADC1_SchmittTriggerConfig(ADC1_SCHMITTTRIG_CHANNEL6, DISABLE);
ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,ADC1_CHANNEL_5,ADC1_PRESSEL_FCPU_D6,ADC1_EXTTRIG_TIM,DISABLE,ADC1_ALIGN_RIGHT,ADC1_SCHMITTTRIG_CHANNEL5,DISABLE); 


 Lcd_Cmd(0x80);
 Lcd_String("Capacity Meter!");
 /*delay_ms(700);
 uint8_t i;
  for(i=0; i<8; i++)
  {
    Lcd_Cmd(0x18); 
    delay_ms(700);

  }
 
  for(i=0; i<8; i++)
  {
    Lcd_Cmd(0x1C); 
    delay_ms(700);
  }
 */


 while(1) 
 {
   
   
   if(GPIO_ReadInputPin(GPIOA,Button)==0)
  {
    while(GPIO_ReadInputPin(GPIOA,Button)==0);
    Button_Pressed_Count++;
    if(Button_Pressed_Count>2)
    {
      Button_Pressed_Count=1;
      Check_Complete=False;
      Measurement_Complete=False;
      Capacity=0;

    }
    
    if(Battery_Ok==False)
    {
      Button_Pressed_Count=1;
      Check_Complete=False;
      Measurement_Complete=False;
      Capacity=0;
    }
  }
   
   
  // if(Button_Pressed_Count==1 && Check_Complete==False )
        if(Button_Pressed_Count==1 )
   {
Check_Complete=True;
    GPIO_WriteLow(GPIOD,Nmos);

//************ Measuring Battery Voltage ***********
for(i=0;i< 100;i++)
  {
   sample1=sample1+Get_ADC_Value(A0); 
   delay_ms(1);
  }
  sample1=sample1/100; 
  Bat_Volt = sample1 *Vcc/ 1024.0; 
  sample1=0;
  

  // *********  Measuring Resistor Voltage ***********
   for( i=0;i< 100;i++)
  {
   sample2=sample2+Get_ADC_Value(A1); 
   delay_ms(1);
  }
  sample2=sample2/100;
  Res_Volt = sample2 * Vcc/ 1024.0;
  sample2=0;
   
  if ( Bat_Volt < 1.0){
    GPIO_WriteLow(GPIOD,Nmos);
    Lcd_Cmd(0x80);                   // set position
    Lcd_String("No Battery!     "); 
    Lcd_Cmd(0xC0);                   // set position
    Lcd_String("             "); 
    Battery_Ok=False;
   }

  
   else if ( Bat_Volt > Bat_High){
    GPIO_WriteLow(GPIOD,Nmos);
    Lcd_Cmd(0x80);                   // set position
    Lcd_String("Bat OverCharged!"); 
    Lcd_Cmd(0xC0); 
    LCD_DisplayFloatNumber( Bat_Volt );  // display Battery Voltage in Volt
    Lcd_String(" v            "); 
    Battery_Ok=False;

   }
   else if(Bat_Volt < Bat_Low){
    GPIO_WriteLow(GPIOD,Nmos);
    Lcd_Cmd(0x80);                   // set position
    Lcd_String("Bat Low         "); 
    Lcd_Cmd(0xC0); 
    LCD_DisplayFloatNumber( Bat_Volt );  // display Battery Voltage in Volt
    Lcd_String(" v           "); 
    Battery_Ok=False;

   }
   else if(Bat_Volt >= Bat_Low && Bat_Volt < Bat_High  ){
    GPIO_WriteLow(GPIOD,Nmos);
    Lcd_Cmd(0x80);                   // set position
    Lcd_String("Bat Ok          "); 
    Lcd_Cmd(0xC0); 
    LCD_DisplayFloatNumber( Bat_Volt );  // display Battery Voltage in Volt
    Lcd_String(" v            ");    
    Battery_Ok=True;
    previousMillis = millis();
}

}

   if(Button_Pressed_Count==2 && Battery_Ok==True)
   {
     
     if(Bat_Volt < Bat_Low)
    {
      GPIO_WriteLow(GPIOD,Nmos);
      Measurement_Complete=True;
      Lcd_Cmd(0x80); 
      Lcd_String("Battery Capacity");
      Lcd_Cmd(0xC0); 
      LCD_DisplayFloatNumber( Capacity*1000 );  // display Battery Voltage in Volt
      Lcd_String(" mAh         ");
      
    }
     
     
    if(Measurement_Complete==False) 
    {
    GPIO_WriteHigh(GPIOD,Nmos);
    
    for( i=0;i< 100;i++)
  {
   sample1=sample1+Get_ADC_Value(A0); //read the voltage from the divider circuit
   delay_ms(1);
  }
  sample1=sample1/100; 
  Bat_Volt = sample1 *Vcc/ 1024.0; 
  sample1=0;
  

  // *********  Measuring Resistor Voltage ***********
   for( i=0;i< 100;i++)
  {
   sample2=sample2+Get_ADC_Value(A1); //read the voltage from the divider circuit
   delay_ms(1);
  }
  sample2=sample2/100;
  Res_Volt = sample2 * Vcc/ 1024.0;
  sample2=0;
    
    millisPassed = millis() - previousMillis;
    Current = (Bat_Volt - Res_Volt) / Res_Value;
    mA = Current ;
    Capacity = Capacity + mA * (millisPassed / 3600000.0); // 1 Hour = 3600000ms
    previousMillis = millis();
     
    Lcd_Cmd(0x80); 
    Lcd_String("  V  ");
    Lcd_String("  I  ");
    Lcd_String(" Ah ");
    
    Lcd_Cmd(0xC0); 
    LCD_DisplayFloatNumber( Bat_Volt );  // display Battery Voltage in Volt
    Lcd_String("  "); 
    LCD_DisplayFloatNumber( mA );        // display Battery Voltage in Volt
    Lcd_String("  "); 
    LCD_DisplayFloatNumber( Capacity );  // display capacity in mAh
   
    
    }
}


 }
 

}





#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
