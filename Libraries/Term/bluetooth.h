#ifndef BLUETOOTH_INIT
#define BLUETOOTH_INIT

void bluetooth_init(void);
void USART1_IRQHandler(void);
void sendStateView(int light, char temperature[10], int humidity);
void sendMainView(void);
void sendSelectView(void);

#endif
