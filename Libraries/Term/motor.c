#include "stm32f10x.h"

void motor_init() {
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
  
  GPIO_InitTypeDef GPIO_InitStructure;  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  
  
  GPIO_SetBits(GPIOB, GPIO_Pin_10);
}

void motor_control(int control_value, float temperature) {
  if (control_value == 1) { GPIO_ResetBits(GPIOB, GPIO_Pin_10); }
  else if (control_value == 2) { GPIO_SetBits(GPIOB, GPIO_Pin_10); }
  else {
    if (temperature < 28.0) { GPIO_SetBits(GPIOB, GPIO_Pin_10); }
    else { GPIO_ResetBits(GPIOB, GPIO_Pin_10); }
  }
}