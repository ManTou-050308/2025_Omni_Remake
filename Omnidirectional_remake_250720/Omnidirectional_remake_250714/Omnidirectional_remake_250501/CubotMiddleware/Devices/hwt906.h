#ifndef _HWT906_H_
#define _HWT906_H_


#include "stm32h7xx_hal.h"
#include "hardware_config.h"
//#include "ins.h"




typedef struct {

    Gyro_RPY_t Accel;

    Gyro_RPY_t Gyro;
	
	Gyro_RPY_t Angel;

    Gyro_RPY_t GyroOffset;

	float q0;
	float q1;
	float q2;
	float q3;
	
    float gNorm;
		
}HWT906_DATA_t;


uint8_t Gyro_callback(uint8_t * recBuffer, uint16_t len);

extern Attitude_t *hwt906_data;
//extern HWT906_DATA_t hwt906_data;


#endif
