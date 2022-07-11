#include "stm32f10x.h"
#include "piezo.h"

void led_init() {
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  
}

void led_control(int control_value, int light) {      
  static int preState = 0, curState = 0;
  if (control_value == 1) { // ON
    GPIO_SetBits(GPIOB, GPIO_Pin_7);
    GPIO_SetBits(GPIOB, GPIO_Pin_8);
    GPIO_SetBits(GPIOB, GPIO_Pin_9);
  }
  else if (control_value == 2) { // OFF
    GPIO_ResetBits(GPIOB, GPIO_Pin_7);
    GPIO_ResetBits(GPIOB, GPIO_Pin_8);
    GPIO_ResetBits(GPIOB, GPIO_Pin_9);
  }
  else { //AUTO
    if (light >= 80) { 
      curState = 1;
      GPIO_ResetBits(GPIOB, GPIO_Pin_7);
      GPIO_ResetBits(GPIOB, GPIO_Pin_8);
      GPIO_ResetBits(GPIOB, GPIO_Pin_9);
    }    
    else if (50 <= light && light < 75) {
      curState = 2;
      GPIO_SetBits(GPIOB, GPIO_Pin_7);
      GPIO_ResetBits(GPIOB, GPIO_Pin_8);
      GPIO_ResetBits(GPIOB, GPIO_Pin_9);
    }
    else if (25 <= light && light < 50) {
      curState = 3;
      GPIO_SetBits(GPIOB, GPIO_Pin_7);
      GPIO_SetBits(GPIOB, GPIO_Pin_8);
      GPIO_ResetBits(GPIOB, GPIO_Pin_9);
    }
    else {
      curState = 4;
      GPIO_SetBits(GPIOB, GPIO_Pin_7);
      GPIO_SetBits(GPIOB, GPIO_Pin_8);
      GPIO_SetBits(GPIOB, GPIO_Pin_9);
    }      
  }
  
  if (curState != preState) {
    preState = curState;
    piezo_alarm();
  }
}