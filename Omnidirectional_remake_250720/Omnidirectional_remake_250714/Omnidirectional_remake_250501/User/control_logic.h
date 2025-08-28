#ifndef CONTROL_H_
#define CONTROL_H_
#include "driver_can.h"


void TIM14_Task(void);
void TIM13_Task(void);
uint8_t CAN2_rxCallBack(CAN_RxBuffer* rxBuffer);
uint8_t CAN1_rxCallBack(CAN_RxBuffer* rxBuffer);
#endif
