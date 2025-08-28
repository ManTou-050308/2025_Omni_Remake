#ifndef __PID_H_
#define __PID_H_

#include "stm32h7xx_hal.h"
#include "referee.h"


extern int Part_Flag;
/**
  * @brief  单环PID 
  */
typedef struct 
{	
	float Kp, Ki, Kd, End_Kp, End_Ki, End_Kd;
	float Error;
	float KpPart, KiPart, KdPart;
	float Out;
	float KiPartDetachment;
	float LastError;
	float LastlastError;
	float ErrorToError;
}BasePID_Object;


/**
  * @brief  双环PID
  */
typedef struct 
{
	BasePID_Object* ShellPID;
	BasePID_Object* CorePID;
}DualPID_Object;
 

/**
  * @brief  		限幅
  * @param[in]	input		输入量
  * @param[in]	output	输出量
  */
float AmplitudeLimit(float input,float amplitude);


/**
  * @brief 单环PID初始化
  */
void BasePID_Init(BasePID_Object* base_pid, float kp, float ki, float kd, float detach);


/**
  * @brief 双环PID初始化
  */
void DualPID_Init(DualPID_Object* dual_pid, float shell_kp, float shell_ki, float shell_kd, float core_kp, float core_ki, float core_kd, float detach);



/**
  * @brief 单环比例积分速度控制
  */
float BasePID_SpeedControl(BasePID_Object* base_pid, float target_speed, float feedback_speed);

/**
  * @brief 底盘单环比例积分速度控制
  */
float BasePID_Speed_Omni_Control(BasePID_Object* base_pid, float target_speed, float feedback_speed);
	
/**
  * @brief 单环比例积分速度控制, 角速度不依靠IMU数据，仅靠编码器进行角度控制
  */
float BasePID_AngleControlWithoutIMU(BasePID_Object* base_pid, float target_angle, float feedback_angle, float feedback_angle_speed);


/**
  * @brief 单环比例微分角度控制, 角速度不依靠IMU数据，仅靠编码器进行角度控制
  */
float BasePID_AngleControl(BasePID_Object* base_pid, float target_angle, float feedback_angle, float feedback_angle_speed);
float BasePID_AngleControlFollow(BasePID_Object* base_pid, float target_angle, float feedback_angle, float feedback_angle_speed);
float BasePID_HoldControl(BasePID_Object* base_pid, float target_angle, float feedback_angle, float feedback_angle_speed);
float BasePID_PitchAngleControl(BasePID_Object* base_pid, float target_angle, float feedback_angle);
float BasePID_PitchSpeedControl(BasePID_Object* base_pid, float target_speed, float feedback_speed);
float BasePID_YawAngleControl(BasePID_Object* base_pid, float target_angle, float feedback_angle);
float BasePID_YawSpeedControl(BasePID_Object* base_pid, float target_speed, float feedback_speed);
float BasePID_PowerControl_Vmax(BasePID_Object* base_pid,float target_power);
float BasePID_Yaw_Dim_AngleControl(BasePID_Object* base_pid, float target_angle, float feedback_angle, float feedback_angle_speed);
float BasePID_AngleControl_Error_Target(BasePID_Object* base_pid, float target_angle, float feedback_angle, float feedback_angle_speed);
uint32_t BasePID_Slope_EcdControl( BasePID_Object* base_pid,int16_t target_ecd,int16_t raw_ecd);
float BasePID_AngleControl_BULEET(BasePID_Object* base_pid, float target_angle, float feedback_angle);
float BasePID_Yaw_VISION_AngleControl(BasePID_Object* base_pid, float target_angle, float feedback_angle);;

#endif

