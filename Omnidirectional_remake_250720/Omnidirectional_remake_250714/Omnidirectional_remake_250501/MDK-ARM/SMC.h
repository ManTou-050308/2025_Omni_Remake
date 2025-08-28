#ifndef _SMC_H
#define _SMC_H
#include "stm32h7xx_hal.h"


/**
  * @brief  SMC 
  */
typedef struct 
{	
	float error;
	float error_last;
	float dref;
	float ddref;
	float ref_last;
	float s;
	float out;
	
	float error_eps; // �������
	float C;         //�л��泣��
	float K;				
  float J;         // ת������
	float epsilon;
	
	float C1;         //�л��泣��
	float C2;         //�л��泣��
	float error_integral;
	

}SMC_Object;

void SMC_Init(SMC_Object* smc, float C, float K, float error_eps, float J,float epsilon);
float SMC(float target,float feedback_angle,float feedback_anglespeed,SMC_Object* smc);
extern SMC_Object  smc_yaw;
#endif