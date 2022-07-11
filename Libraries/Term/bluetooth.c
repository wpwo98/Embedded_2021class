#include <stdio.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"

void bluetooth_init(void);
void sendStateView(int light, char temperature[10], int humidity);
void sendMainView(void);
void sendSelectView(void);
void USART1_IRQHandler(void);
void delay1(void);
void delay2(void);
void sendDataUART1(uint16_t data);
void sendStringUSART1(const char *str);


extern int control_values[3];  // 0 Auto, 1 On, 2 Off / 0 LED, 1 Wind, 2 Water
extern char control_str[3][10];

uint16_t flag, preWord, curWord;


void bluetooth_init(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  
   USART_InitTypeDef USART1_InitStructure;
   
   /* UART1 pin setting */
   GPIO_InitTypeDef GPIO_InitStructure;
  //TX
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  //RX
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   

   // Initialize the USART1 using the structure 'USART_InitTypeDef' and the function 'USART_Init'
   USART1_InitStructure.USART_BaudRate = 9600;
   USART1_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART1_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
   USART1_InitStructure.USART_Parity = USART_Parity_No;
   USART1_InitStructure.USART_StopBits = USART_StopBits_1;
   USART1_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_Init(USART1, &USART1_InitStructure);

   // Enable the USART1 peripheral
   USART_Cmd(USART1, ENABLE);
   // Enable the USART1 RX interrupts using the function 'USART_ITConfig' and the argument value 'Receive Data register not empty interrupt'
   USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // USART RX만 Interrupt 설정
   
   NVIC_InitTypeDef NVIC_InitStructure;
  
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    
  // UART1
  // 'NVIC_EnableIRQ' is only required for USART setting
  NVIC_EnableIRQ(USART1_IRQn);
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // TODO
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  

  while (!flag) ;  
  sendDataUART1((uint16_t)65); 
  sendDataUART1((uint16_t)84); 
  sendDataUART1((uint16_t)43); 
  sendDataUART1((uint16_t)66); 
  sendDataUART1((uint16_t)84); 
  sendDataUART1((uint16_t)83); 
  sendDataUART1((uint16_t)67); 
  sendDataUART1((uint16_t)65); 
  sendDataUART1((uint16_t)78); 
  sendDataUART1((uint16_t)13); 
}




void sendStateView(int light, char temperature[10], int humidity) {
  char light_str[10], humidity_str[10];
  sprintf(light_str, "%d\n", light);
  sprintf(humidity_str, "%d\n", humidity);
    
  sendStringUSART1("-------------------------------------------------------------\n");
  sendStringUSART1("[Current State]\n");
  sendStringUSART1("Light: "); 
  sendStringUSART1(light_str);  
  sendStringUSART1("Temperature: "); 
  sendStringUSART1(temperature); 
  sendStringUSART1("\nHumidity: "); 
  sendStringUSART1(humidity_str);
    
  sendStringUSART1("\nLED: ");    
  sendStringUSART1(control_str[control_values[0]]); 
  sendStringUSART1("\nWindMotor: ");    
  sendStringUSART1(control_str[control_values[1]]); 
  sendStringUSART1("\nWaterPump: ");    
  sendStringUSART1(control_str[control_values[2]]); 
  sendStringUSART1("\n-------------------------------------------------------------\n");
}

void sendMainView() {      
  sendStringUSART1("-------------------------------------------------------------\n");
  sendStringUSART1("[Smart Flowerpot]\n");
  sendStringUSART1("1. State View\n"); 
  sendStringUSART1("2. Select LED Control\n"); 
  sendStringUSART1("3. Select WindMotor Control\n"); 
  sendStringUSART1("4. Select WaterPump Control\n"); 
  sendStringUSART1("-------------------------------------------------------------\n");
}

void sendSelectView() {
  sendStringUSART1("-------------------------------------------------------------\n");
  sendStringUSART1("[Select Control State]\n");
  sendStringUSART1("1. Auto\n"); 
  sendStringUSART1("2. On\n"); 
  sendStringUSART1("3. Off\n");   
  sendStringUSART1("-------------------------------------------------------------\n");
}


void USART1_IRQHandler() {  
  uint16_t word;
  if(USART_GetITStatus(USART1,USART_IT_RXNE)!= RESET){
    // the most recent received data by the USART1 peripheral
    word = USART_ReceiveData(USART1);
    if (!flag && word == 75) { flag = 1;}
    if (49 <= word && word <= 52) { // 1~4 입력만 받음      
      curWord = word;
      flag = 1;
    }
    // clear 'Read data register not empty' flag
    USART_ClearITPendingBit(USART1,USART_IT_RXNE);
  }
}

void delay1(void) {
  int i;  
  for (i = 0; i < 50000; i++) {}
}

void delay2(void) {
  int i;  
  for (i = 0; i < 50000; i++) {}
}

void sendDataUART1(uint16_t data) {
  /* Wait till TC is set */
  // while ((USART1->SR & USART_SR_TC) == 0);
  USART_SendData(USART1, data);
  delay1();
}

void sendStringUSART1(const char *str)
{
    while (*str)
    {
        USART_SendData(USART1, *str++);
        delay2();
    }
}