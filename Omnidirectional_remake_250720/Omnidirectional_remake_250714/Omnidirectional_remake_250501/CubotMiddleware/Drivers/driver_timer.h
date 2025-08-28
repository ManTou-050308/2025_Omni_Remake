#ifndef DRIVERTIMER_H
#define DRIVERTIMER_H
#include "tim.h"


typedef void (*TIM_ElapsedCallback)(void); 


typedef struct
{
	TIM_HandleTypeDef* 	Handle;
	int32_t ClockTime;
	TIM_ElapsedCallback		ElapCallback;		//< ��ʱ������ж�
	uint16_t HolderTime;
	uint16_t ErrorTime;
	uint32_t UI_Time;
	uint32_t Step_Time;
	uint32_t Brain_Time;
}TIM_Object;

/**
  * @brief ��ʼ����ʱ�����û��ص�
  */
void TIMx_Init(TIM_HandleTypeDef* handle, TIM_ElapsedCallback callback);


/**
  * @brief     ������ʱ������ж�
  */
void TIM_Open(TIM_Object* tim);



extern TIM_Object tim14;


extern TIM_Object tim13;
#endif

