#ifndef __HEAT_CONTROL_H
#define __HEAT_CONTROL_H
#include "stm32h7xx_hal.h"
typedef struct 
{
	float shoot_recover;
	volatile float on_time_heat;//实时热量
	uint8_t flag_referee;
	uint8_t cooling_times;
	uint8_t fire_mode;
	uint8_t over_heat;	
	uint8_t heat_status;
	uint8_t coolvalue_100;
	
	int Speed_Bride ;//边界条件
	uint64_t Heat_cnt;
	float Last_SpeedRPM_Left;
	float Last_SpeedRPM_Right;
	float Heat_limt; //热量上限
	float Last_Heat_limt;
	float Heat_Consumption;//热量消耗
	float Last_Level;
	uint64_t Number_100;
	uint64_t Last_Number_100;
}Heat_Info;


extern  Heat_Info Muzzle;

uint8_t IfOverHeat(void);
float Heat_Control();
#define FIRE_ONE_ONETIME 1
#define FIRE_THREE_ONETIME 3
#endif

