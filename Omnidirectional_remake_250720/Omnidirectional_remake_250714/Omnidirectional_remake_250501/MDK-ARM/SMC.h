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
	
	float error_eps; // 误差下限
	float C;         //切换面常数
	float K;				
  float J;         // 转动惯量
	float epsilon;
	
	float C1;         //切换面常数
	float C2;         //切换面常数
	float error_integral;
	

}SMC_Object;

void SMC_Init(SMC_Object* smc, float C, float K, float error_eps, float J,float epsilon);
float SMC(float target,float feedback_angle,float feedback_anglespeed,SMC_Object* smc);
extern SMC_Object  smc_yaw;
#endif