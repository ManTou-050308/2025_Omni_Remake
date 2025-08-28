#ifndef __PID_H_
#define __PID_H_

#include "stm32h7xx_hal.h"
#include "referee.h"


extern int Part_Flag;
/**
  * @brief  ����PID 
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
  * @brief  ˫��PID
  */
typedef struct 
{
	BasePID_Object* ShellPID;
	BasePID_Object* CorePID;
}DualPID_Object;
 

/**
  * @brief  		�޷�
  * @param[in]	input		������
  * @param[in]	output	�����
  */
float AmplitudeLimit(float input,float amplitude);


/**
  * @brief ����PID��ʼ��
  */
void BasePID_Init(BasePID_Object* base_pid, float kp, float ki, float kd, float detach);


/**
  * @brief ˫��PID��ʼ��
  */
void DualPID_Init(DualPID_Object* dual_pid, float shell_kp, float shell_ki, float shell_kd, float core_kp, float core_ki, float core_kd, float detach);



/**
  * @brief �������������ٶȿ���
  */
float BasePID_SpeedControl(BasePID_Object* base_pid, float target_speed, float feedback_speed);

/**
  * @brief ���̵������������ٶȿ���
  */
float BasePID_Speed_Omni_Control(BasePID_Object* base_pid, float target_speed, float feedback_speed);
	
/**
  * @brief �������������ٶȿ���, ���ٶȲ�����IMU���ݣ��������������нǶȿ���
  */
float BasePID_AngleControlWithoutIMU(BasePID_Object* base_pid, float target_angle, float feedback_angle, float feedback_angle_speed);


/**
  * @brief ��������΢�ֽǶȿ���, ���ٶȲ�����IMU���ݣ��������������нǶȿ���
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

