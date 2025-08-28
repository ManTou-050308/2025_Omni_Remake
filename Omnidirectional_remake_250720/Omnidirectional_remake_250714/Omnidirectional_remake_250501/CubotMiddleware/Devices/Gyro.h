#ifndef GYRO_H
#define GYRO_H
//#include "stm32h7xx_hal.h"
//#include "mpu6050.h"
//#include "bmi088.h"


//#define abs(x) ((x)>0? (x):(-(x)))




typedef struct
{
	uint8_t Choose_Gyro;

    Gyro_RPY_t Accel;

    Gyro_RPY_t Gyro;
	
	Gyro_RPY_t Angel;

	float q0;
	float q1;
	float q2;
	float q3;
	
}GYRO_DATA_t;



//void Gyro_Init(GYRO_DATA_t* gyro,uint8_t Choose_Gyro);
//void MPU_Init(void);
//void Gyro_Data_Copy(MPU6050_DATA_t* mpu6050, BMI088_DATA_t* bmi088, GYRO_DATA_t* gyro);
//void Gyro_Get_Data(GYRO_DATA_t* gyro);
//void Gyro_Reset(GYRO_DATA_t* gyro);


#endif
