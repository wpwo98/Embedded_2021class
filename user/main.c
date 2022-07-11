#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "core_cm3.h"
#include "misc.h"
#include "lcd.h"
#include "touch.h"

#include "adc_dma.h"
#include "bluetooth.h"
#include "led.h"
#include "motor.h"
#include "waterpump.h"
#include "piezo.h"


int color[12] = {WHITE,CYAN,BLUE,RED,MAGENTA,LGRAY,GREEN,YELLOW,BROWN,BRRED,GRAY};

extern volatile uint16_t ADC_values[8]; // 0 조도센서, 1 토양습도센서, 2 온도센서
extern uint16_t preWord, curWord, flag; // 블루투스

int control_values[3];  // 0 Auto, 1 On, 2 Off / 0 LED, 1 Wind, 2 Water
char control_str[3][10] = {"Auto", "On  ", "Off "};
uint16_t posX, posY, conX, conY;





// 모든 모듈 초기화 함수 작성
void Init() {
  adc_dma_init();
  led_init();
  waterpump_init();
  motor_init();
  piezo_init();
  bluetooth_init();     
  
  LCD_Init();
  Touch_Configuration();
  Touch_Adjust();
  LCD_Clear(WHITE);
}

void Delay(void) {
  int i;  
  for (i = 0; i < 500000; i++) {}
}

void Delay2(void) {
  int i;  
  for (i = 0; i < 1000; i++) {}
}

int main(void)
{  
  SystemInit();  
  Init();
  
  LCD_ShowString(20, 50, "[ Smart Flowerpot ]", BLACK, WHITE);
  LCD_ShowString(20, 70, "Light:", BLACK, WHITE);
  LCD_ShowString(20, 90, "temperature:", BLACK, WHITE);
  LCD_ShowString(20, 110, "Humidity:", BLACK, WHITE);
  
  
  LCD_ShowString(30, 205, "LED", BLACK, WHITE);
  LCD_DrawRectangle(20, 200, 120, 230);  
  LCD_ShowString(30, 245, "WindMotor", BLACK, WHITE);
  LCD_DrawRectangle(20, 240, 120, 270);  
  LCD_ShowString(30, 285, "WaterPump", BLACK, WHITE);
  LCD_DrawRectangle(20, 280, 120, 310);
 
  
  int light, temperature, humidity;  
  char temperature_str[10];    
  while (1) {
    light = humidity = temperature = 0;
    for (int i = 0; i < 40; ++i) {
      light += ADC_values[0];
      temperature += ADC_values[2];
      humidity += ADC_values[1];      
      Delay2();
    }
    light /= 40;
    temperature /= 40;
    humidity /= 40;
    
    
    // ADC값 변환
    if (light > 4080) { light = 0; }  // 조도센서 min 3850, max 4180     
    else if (light < 3850) { light = 100; }
    else { light = 100.0 - ((float)light - 3850.0) *100.0 / 330.0; }
    sprintf(temperature_str, "%.1f", temperature * 0.0896);    //온도센서 0.089곱하여 사용    
    humidity = (float)humidity  * 100.0 / 4000.0;     // 토양습도센서 min 0, max 4000     
    
    
    
    //  input값을 사용하여  output device 컨트롤
    led_control(control_values[0], light); 
    motor_control(control_values[1], temperature * 0.0896);
    waterpump_control(control_values[2], humidity); 
       
    
    for (int i = 0; i < 10; ++i) {
      // BlueTooth
      if (flag) {
        if (preWord == '1') {
          if (curWord == '1') {
            sendStateView(light, temperature_str, humidity);
            sendMainView();
          }
          else if ('2' <= curWord && curWord <= '4') {
            sendSelectView();
            preWord = curWord;
          }
        }
        else if (curWord != '4') {        
          if (preWord == '2') { control_values[0] = curWord - '1'; }
          else if (preWord == '3') { control_values[1] = curWord - '1'; }
          else if (preWord == '4') { control_values[2] = curWord - '1'; }        
          preWord = '1';
          sendMainView();
        }
        flag = 0;
      }
      
      // TFT-LCD         
      if (!T_INT) {       
        Touch_GetXY(&posX, &posY, 0); 
        Convert_Pos(posX, posY,&conX,&conY);      
        if ((20 <= conX && conX <= 120) && (200 <= conY && conY <= 230)) { control_values[0] = (control_values[0] + 1) % 3; }
        else if ((20 <= conX && conX <= 120) && (240 <= conY && conY <= 270)) { control_values[1] = (control_values[1] + 1) % 3; }
        else if ((20 <= conX && conX <= 120) && (280 <= conY && conY <= 310)) { control_values[2] = (control_values[2] + 1) % 3; }      
      }    
      
      LCD_ShowNum(70, 70, light, 3, BLACK, WHITE);
      LCD_ShowString(120, 90, temperature_str, BLACK, WHITE);
      LCD_ShowNum(90, 110, humidity, 3, BLACK, WHITE);    
      LCD_ShowString(135, 205, control_str[control_values[0]], BLACK, WHITE);
      LCD_ShowString(135, 245, control_str[control_values[1]], BLACK, WHITE);
      LCD_ShowString(135, 285, control_str[control_values[2]], BLACK, WHITE);
      
      Delay();    
    }
  }  
  return 0;
}