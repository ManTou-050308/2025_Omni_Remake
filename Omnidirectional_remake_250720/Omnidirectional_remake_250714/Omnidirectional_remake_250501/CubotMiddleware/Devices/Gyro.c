//#include "Gyro.h"
//#include "mpu6050.h"
//#include "driver_timer.h"
//#include "kalman.h"
//#include "hardware_config.h"
//#include "filter.h"
//#include "i2c.h"





//void Gyro_Init(GYRO_DATA_t* gyro,uint8_t Choose_Gyro)
//{
//	
//	gyro->Choose_Gyro = Choose_Gyro;
//	
//	if(Choose_Gyro == Choose_MPU6050_IN)
//	{
//		MPU6050_PowerOn(Choose_Gyro);
//		MPU6050_Init(&hi2c2, &mpu6050_data_in);
//	}
//	else if(Choose_Gyro == Choose_MPU6050_OUT)
//	{
//		MPU6050_PowerOn(Choose_Gyro);
//		MPU6050_Init(&hi2c4, &mpu6050_data_out);
//	}
//	else if(Choose_Gyro == Choose_BMI088_IN)
//	{
//		SPI_Init();
//		BMI088_Init();
//	}

//}


//void Gyro_Get_Data(GYRO_DATA_t* gyro)
//{
////	if(tim14.ClockTime >= 1000)
////	{
//		if(gyro->Choose_Gyro == Choose_MPU6050_IN)
//		{
//			MPU6050_Read(&mpu6050_data_in,&hi2c2);
//			Gyro_Data_Copy(&mpu6050_data_in, &bmi088_data, gyro);
//		}
//		else if(gyro->Choose_Gyro == Choose_MPU6050_OUT)
//		{
//			MPU6050_Read(&mpu6050_data_out,&hi2c4);
//			Gyro_Data_Copy(&mpu6050_data_out, &bmi088_data, gyro);
//		}
//		else if(gyro->Choose_Gyro == Choose_BMI088_IN)
//		{
//			BMI088_Read(&bmi088_data);
//			Gyro_Data_Copy(&mpu6050_data_in, &bmi088_data, gyro);
//		}
////	
////	}
//}


//void Gyro_Data_Copy(MPU6050_DATA_t* mpu6050, BMI088_DATA_t* bmi088, GYRO_DATA_t* gyro)
//{
//	if(gyro->Choose_Gyro == Choose_MPU6050_IN || gyro->Choose_Gyro == Choose_MPU6050_OUT)
//	{
//		if(mpu6050->Gyro[0] >= mpu6050->Dead_Zone || mpu6050->Gyro[0] <= -mpu6050->Dead_Zone)
//			gyro_data.Angel.Roll += mpu6050->Gyro[0] * mpu6050->Sens[0];
//		
//		if(mpu6050->Gyro[1] >= mpu6050->Dead_Zone || mpu6050->Gyro[1] <= -mpu6050->Dead_Zone)
//			gyro_data.Angel.Pitch += mpu6050->Gyro[1] * mpu6050->Sens[1];
//		
//		if(mpu6050->Gyro[2] >= mpu6050->Dead_Zone || mpu6050->Gyro[2] <= -mpu6050->Dead_Zone)
//			gyro_data.Angel.Yaw += mpu6050->Gyro[2] * mpu6050->Sens[2];
//		
//		gyro_data.Gyro.Roll 	= mpu6050->Gyro[0];
//		gyro_data.Gyro.Pitch 	= mpu6050->Gyro[1];
//		gyro_data.Gyro.Yaw 		= mpu6050->Gyro[2];
//		
//		gyro_data.Accel.Roll 	= mpu6050->Accel[0];
//		gyro_data.Accel.Pitch = mpu6050->Accel[1];
//		gyro_data.Accel.Yaw 	= mpu6050->Accel[2];		
//	}
//	else if(gyro->Choose_Gyro == Choose_BMI088_IN)
//	{
//		if(bmi088->Gyro[0] >= bmi088->Dead_Zone || bmi088->Gyro[0] <= -bmi088->Dead_Zone)
//			gyro_data.Angel.Roll += bmi088->Gyro[0] * bmi088->Sens[0];
//		
//		if(bmi088->Gyro[1] >= bmi088->Dead_Zone || bmi088->Gyro[1] <= -bmi088->Dead_Zone)
//			gyro_data.Angel.Pitch += bmi088->Gyro[1] * bmi088->Sens[1];
//		
//		if(bmi088->Gyro[2] >= bmi088->Dead_Zone || bmi088->Gyro[2] <= -bmi088->Dead_Zone)
//			gyro_data.Angel.Yaw += bmi088->Gyro[2] * bmi088->Sens[2];
//		
//		gyro_data.Gyro.Roll 	= bmi088->Gyro[0];
//		gyro_data.Gyro.Pitch 	= bmi088->Gyro[1];
//		gyro_data.Gyro.Yaw 		= bmi088->Gyro[2];
//		
//		gyro_data.Accel.Roll 	= bmi088->Accel[0];
//		gyro_data.Accel.Pitch = bmi088->Accel[1];
//		gyro_data.Accel.Yaw 	= bmi088->Accel[2];
//	}
//	
//}


//void Gyro_Reset(GYRO_DATA_t* gyro)
//{
//	//>下电
//	MPU6050_PowerOff(gyro->Choose_Gyro);
//	HAL_Delay(50);

//	//>上电初始化
//	Gyro_Init(gyro,gyro->Choose_Gyro);

//}


