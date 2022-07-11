#include "stm32f10x.h"

void piezo_init() {
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); 
  
  GPIO_InitTypeDef GPIO_InitStructure;  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  
}

void piezo_alarm() {
  GPIO_SetBits(GPIOB, GPIO_Pin_5);
  for (int i = 0; i < 100000; ++i);
  GPIO_ResetBits(GPIOB, GPIO_Pin_5);
}