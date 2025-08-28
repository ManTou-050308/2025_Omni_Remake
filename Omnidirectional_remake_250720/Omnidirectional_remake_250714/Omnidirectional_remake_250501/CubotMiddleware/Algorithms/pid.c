#include "pid.h"
#include "driver_timer.h"
#include "dr16.h"
#include "filter.h"
#include "brain.h"
#include "hardware_config.h"
#include "omnichassis.h"
#define Abs(x) ((x)>0? (x):(-(x)))
/**
  * @brief  �޷�
  */
float AmplitudeLimit(float input,float amplitude)
{
	if(input<-amplitude)
		return -amplitude;
	else if(input>amplitude)
		return amplitude;
	else return input;
}


/**
  * @brief ����PID��ʼ��
  */
void BasePID_Init(BasePID_Object* base_pid, float kp, float ki, float kd, float detach)
{
	base_pid->KiPartDetachment = detach;
	
	base_pid->Kp = kp;
	base_pid->Ki = ki;
	base_pid->Kd = kd;

	base_pid->KpPart = 0;
	base_pid->KiPart = 0;
	base_pid->KdPart = 0;
	
}

/**
  * @brief ˫��PID��ʼ��
  */
void DualPID_Init(DualPID_Object* dual_pid, float shell_kp, float shell_ki, float shell_kd, float core_kp, float core_ki, float core_kd, float detach)
{
	dual_pid->ShellPID->KiPartDetachment = detach;
	dual_pid->CorePID->KiPartDetachment = detach;
	
	dual_pid->ShellPID->Kp = shell_kp;
	dual_pid->ShellPID->Ki = shell_ki;
	dual_pid->ShellPID->Kd = shell_kd;
	
	dual_pid->CorePID->Kp = core_kp;
	dual_pid->CorePID->Kp = core_ki;	
	dual_pid->CorePID->Kp = core_kd;
	
	dual_pid->ShellPID->KpPart = 0;
	dual_pid->ShellPID->KiPart = 0;
	dual_pid->ShellPID->KdPart = 0;
	
	dual_pid->CorePID->KpPart = 0;
	dual_pid->CorePID->KiPart = 0;
	dual_pid->CorePID->KdPart = 0;

}


/**
  * @brief �������������ٶȿ���
  */
float BasePID_SpeedControl(BasePID_Object* base_pid, float target_speed, float feedback_speed)
{
	base_pid->Error = target_speed - feedback_speed;
	
	base_pid->KpPart = base_pid->Error * base_pid->Kp;
	base_pid->KiPart += base_pid->Error * base_pid->Ki;
	
	if(base_pid->Error > base_pid->KiPartDetachment)
	{
		base_pid->KiPart = 0;
	}
	else if(base_pid->Error < -(base_pid->KiPartDetachment))
	{
		base_pid->KiPart = 0;
	}

	
	return base_pid->Out;
}
/**
  * @brief ���̵������������ٶȿ���
  */
float BasePID_Speed_Omni_Control(BasePID_Object* base_pid, float target_speed, float feedback_speed)
{
	base_pid->Error = target_speed - feedback_speed;
	if((0<base_pid->Error && base_pid->Error <= 300) || (0>base_pid->Error && base_pid->Error >= -300))base_pid->Out = 0;

	base_pid->KpPart = base_pid->Error * base_pid->Kp;
	base_pid->KiPart += base_pid->Error * base_pid->Ki;
	
	if(base_pid->Error > base_pid->KiPartDetachment)
	{
		base_pid->KiPart = 0;
	}
	else if(base_pid->Error < -(base_pid->KiPartDetachment))
	{
		base_pid->KiPart = 0;
	}
	
	base_pid->Out = base_pid->KpPart + base_pid->KiPart;

	return base_pid->Out;
}


/**
  * @brief �����������ֽǶȿ���   
  */
float BasePID_AngleControl_BULEET(BasePID_Object* base_pid, float target_angle, float feedback_angle)
{
	base_pid->Error = target_angle - feedback_angle;
	base_pid->KpPart = base_pid->Error * base_pid->Kp;
	base_pid->KiPart += base_pid->Error * base_pid->Ki;
	base_pid->KdPart = base_pid->Error * base_pid->Kd;
	if(base_pid->Error > base_pid->KiPartDetachment)
	{
		base_pid->KiPart = 0;
	}
	else if(base_pid->Error < -(base_pid->KiPartDetachment))
	{
		base_pid->KiPart = 0;
	}
	base_pid->Out = base_pid->KpPart + base_pid->KiPart + base_pid->KdPart;

	return base_pid->Out;
}
/**
  * @brief �����������ֽǶȿ���   
  */

float BasePID_HoldControl(BasePID_Object* base_pid, float target_angle, float feedback_angle, float feedback_angle_speed)
{
	base_pid->Error = target_angle - feedback_angle;
	if(tim14.ErrorTime == 20000&&base_pid->Error>3.5f){
		base_pid->Error=3.5f;
	}
	if(tim14.ErrorTime == 20000&&base_pid->Error<-3.5){
		base_pid->Error=-3.5;
	}
	base_pid->KpPart = base_pid->Error * base_pid->Kp;
	base_pid->KiPart += base_pid->Error * base_pid->Ki;
	if(tim14.HolderTime < 15000){
	base_pid->KiPart = 0;
	}
	if(base_pid->Error > base_pid->KiPartDetachment)
	{
		base_pid->KiPart = 0;
	}
	else if(base_pid->Error < -(base_pid->KiPartDetachment))
	{
		base_pid->KiPart = 0;
	}
	
	base_pid->KdPart = (-1) * base_pid->Kd * feedback_angle_speed;
	base_pid->Out = base_pid->KpPart + base_pid->KiPart + base_pid->KdPart;
	
	return base_pid->Out;
}


/**
  * @brief �����������ֽǶȿ���   
  */
float BasePID_AngleControl(BasePID_Object* base_pid, float target_angle, float feedback_angle, float feedback_angle_speed)
{
	base_pid->Error = target_angle - feedback_angle;
	base_pid->KpPart = base_pid->Error * base_pid->Kp;
	base_pid->KiPart += base_pid->Error * base_pid->Ki;

	if(base_pid->Error > base_pid->KiPartDetachment)
	{
		base_pid->KiPart = 0;
	}
	else if(base_pid->Error < -(base_pid->KiPartDetachment))
	{
		base_pid->KiPart = 0;
	}
	
	base_pid->KdPart = (-1) * base_pid->Kd * feedback_angle_speed;
	base_pid->Out = base_pid->KpPart + base_pid->KiPart + base_pid->KdPart;
	return base_pid->Out;
}

/**
  * @brief �����������ֽǶ����ֵ�Ĳ�ֵ��ΪĿ�����   
  */
float BasePID_AngleControl_Error_Target(BasePID_Object* base_pid, float target_angle, float feedback_angle, float feedback_angle_speed)
{

	base_pid->Error = target_angle - feedback_angle;
	
	base_pid->KpPart = base_pid->Error * base_pid->Kp;
	base_pid->KiPart += base_pid->Error * base_pid->Ki;	
	
	if(tim14.HolderTime < 15000){
	base_pid->KiPart = 0;
	}

	if(base_pid->Error > base_pid->KiPartDetachment)
	{
		base_pid->KiPart = 0;
	}
	else if(base_pid->Error < -(base_pid->KiPartDetachment))
	{
		base_pid->KiPart = 0;
	}


	base_pid->KdPart = (-1) * base_pid->Kd * feedback_angle_speed;
	base_pid->Out = base_pid->KpPart + base_pid->KiPart + base_pid->KdPart;

	return base_pid->Out;
}


/**
  * @brief Yaw�����������ֽǶȿ��� + ĩ���𵴾�ֵ����   
	* @note ���⴦���������۵��� ĩ��kd���µ��� 
  */
float BasePID_Yaw_Dim_AngleControl(BasePID_Object* base_pid, float target_angle, float feedback_angle, float feedback_angle_speed)
{
	base_pid->Error = target_angle - feedback_angle;
	base_pid->KpPart = base_pid->Error * base_pid->Kp;
	base_pid->KiPart += base_pid->Error * base_pid->Ki;
	if(tim14.HolderTime < 15000){
	base_pid->KiPart = 0;
	}
	
	if(base_pid->Error > base_pid->KiPartDetachment)
	{
		base_pid->KiPart = 0;
	}
	else if(base_pid->Error < -(base_pid->KiPartDetachment))
	{		
		base_pid->KiPart = 0;
	}
	
	base_pid->KdPart = (-1) * base_pid->Kd * feedback_angle_speed;
	base_pid->Out = base_pid->KpPart + base_pid->KiPart + base_pid->KdPart;	

	base_pid->LastError = base_pid->Error;
	
	return base_pid->Out;
}

/**
  * @brief ���� ΢С��������
  */
float BasePID_AngleControlFollow(BasePID_Object* base_pid, float target_angle, float feedback_angle, float feedback_angle_speed)
{
	base_pid->Error = target_angle - feedback_angle;
	
	base_pid->KpPart = base_pid->Error * base_pid->Kp;
	base_pid->KiPart += base_pid->Error * base_pid->Ki;
	base_pid->KdPart = (-1) * base_pid->Kd * feedback_angle_speed;
	
	base_pid->Out = base_pid->KpPart + base_pid->KiPart + base_pid->KdPart;
	
	return base_pid->Out;
}


/**
  * @brief pitch�ǶȻ�  ˫���⻷
  */
float BasePID_PitchAngleControl(BasePID_Object* base_pid, float target_angle, float feedback_angle)
{
	base_pid->Error = target_angle - feedback_angle;
	base_pid->KpPart = base_pid->Error * base_pid->Kp;
	base_pid->KiPart += base_pid->Error * base_pid->Ki;
	
	if(base_pid->Error > base_pid->KiPartDetachment||Rc_Ctrl.isOnline == 0)
	{
		base_pid->KiPart = 0;
	}
	else if(base_pid->Error < -(base_pid->KiPartDetachment)||Rc_Ctrl.isOnline == 0)
	{
		base_pid->KiPart = 0;
	}
	
	base_pid->KdPart = (-1) * base_pid->Kd * (base_pid->Error - base_pid->LastError);
	base_pid->LastError = base_pid->Error;
	base_pid->Out = base_pid->KpPart + base_pid->KiPart + base_pid->KdPart;
	
	return base_pid->Out;
}

/**
  * @brief pitch�ٶȻ�  ˫���ڻ�
  */
float BasePID_PitchSpeedControl(BasePID_Object* base_pid, float target_speed, float feedback_speed)
{
	base_pid->Error = target_speed - feedback_speed;

	base_pid->KpPart = base_pid->Error * base_pid->Kp;
	base_pid->KiPart += base_pid->Error * base_pid->Ki;
	if(base_pid->Error > base_pid->KiPartDetachment||Rc_Ctrl.isOnline == 0)
	{
		base_pid->KiPart = 0;
	}
	else if((base_pid->Error < -base_pid->KiPartDetachment)||Rc_Ctrl.isOnline == 0)
	{
		base_pid->KiPart = 0;
	}
	
	base_pid->KdPart = (-1) * base_pid->Kd * (base_pid->Error - base_pid->LastError);
	base_pid->LastError = base_pid->Error;
	base_pid->Out = base_pid->KpPart + base_pid->KiPart + base_pid->KdPart;

	return base_pid->Out;
}


/**
  * @brief yaw�ǶȻ�  ˫���⻷(����)
	* @note  ΢������
  */
float BasePID_YawAngleControl(BasePID_Object* base_pid, float target_angle, float feedback_angle)
{
	static float feedback_angle_Last;
	base_pid->Error = target_angle - feedback_angle;
	base_pid->KpPart = base_pid->Error * base_pid->Kp;
	base_pid->KiPart += base_pid->Error * base_pid->Ki;

	if(base_pid->Error > base_pid->KiPartDetachment||Rc_Ctrl.isOnline == 0)
	{
		base_pid->KiPart = 0;
	}
	else if(base_pid->Error < -(base_pid->KiPartDetachment)||Rc_Ctrl.isOnline == 0)
	{
		base_pid->KiPart = 0;
	}
	
	base_pid->KdPart = (-1) * base_pid->Kd * (feedback_angle - feedback_angle_Last);
	base_pid->LastError = base_pid->Error;
	base_pid->Out = base_pid->KpPart + base_pid->KiPart + base_pid->KdPart;
	
//	base_pid->Out = LPFilter(base_pid->Out ,&LPF_yaw_angle);
	feedback_angle_Last = feedback_angle;
	return base_pid->Out;
}
/**
  * @brief yaw�ǶȻ�  ˫���⻷(�Ӿ�)
  */
float BasePID_Yaw_VISION_AngleControl(BasePID_Object* base_pid, float target_angle, float feedback_angle)
{
	base_pid->Error = target_angle - feedback_angle;
	base_pid->KpPart = base_pid->Error * base_pid->Kp;
	

//	if(base_pid->Error > base_pid->KiPartDetachment||Rc_Ctrl.isOnline == 0)
//	{
//		base_pid->KiPart = 0;
//	}
//	else if(base_pid->Error < -(base_pid->KiPartDetachment)||Rc_Ctrl.isOnline == 0)
//	{
//		base_pid->KiPart = 0;
//	}
	if( myABS(base_pid->Error)< 0.1 || Rc_Ctrl.isOnline == 0)	base_pid->KiPart = 0;
	if(myABS(base_pid->Error)> 0.5 ) base_pid->KiPart += base_pid->Error * base_pid->Ki;
	
	

	
	base_pid->KdPart = (-1) * base_pid->Kd * (base_pid->Error - base_pid->LastError);	
	base_pid->LastError = base_pid->Error;
	base_pid->Out = base_pid->KpPart + base_pid->KiPart + base_pid->KdPart;
	
//	base_pid->Out = LPFilter(base_pid->Out ,&LPF_yaw_angle);
	return base_pid->Out;
}

/**
  * @brief yaw�ٶȻ�  ˫���ڻ�
  */
float BasePID_YawSpeedControl(BasePID_Object* base_pid, float target_speed, float feedback_speed)
{
	base_pid->Error = target_speed - feedback_speed;
	base_pid->KpPart = base_pid->Error * base_pid->Kp;
	

	if(base_pid->Error > base_pid->KiPartDetachment || Rc_Ctrl.isOnline == 0)
	{
	//	base_pid->KiPart = 0;
	}
	else if(base_pid->Error < -base_pid->KiPartDetachment || Rc_Ctrl.isOnline == 0)
	{
	//	base_pid->KiPart = 0;
	}
	else base_pid->KiPart += base_pid->Error * base_pid->Ki;
	
if(base_pid->Error == 0) base_pid->KiPart = 0;
	
	base_pid->KdPart = (-1) * base_pid->Kd * (base_pid->Error - base_pid->LastError);
	base_pid->LastError = base_pid->Error;
	base_pid->Out = base_pid->KpPart + base_pid->KiPart + base_pid->KdPart;

	return base_pid->Out;
}

/**
  * @brief pitch�ǶȻ�  ˫���⻷ ����ʽPID
  */
int32_t BasePID_increment_PitchAngleControl(BasePID_Object* base_pid, float target_angle, float feedback_angle)
{
	base_pid->Error = target_angle - feedback_angle;
	base_pid->KpPart=(base_pid->Error-base_pid->LastError)*base_pid->Kp;
	base_pid->KiPart=base_pid->Error * base_pid->Ki;
	base_pid->KdPart= base_pid->Kd * (base_pid->Error - 2*base_pid->LastError+base_pid->LastlastError);

	base_pid->LastlastError=base_pid->LastError;
	base_pid->LastError = base_pid->Error;

	base_pid->Out = base_pid->KpPart + base_pid->KiPart + base_pid->KdPart;
	
	return base_pid->Out;
}


/**
  * @brief ���ʿ���PID
  */
float BasePID_PowerControl_Vmax(BasePID_Object* base_pid,float target_power)
{
	base_pid->Error = target_power-referee2024.power_heat_data_t.chassis_power;
	base_pid->KpPart = base_pid->Error*base_pid->Kp;
	
	if(base_pid->Error > base_pid->KiPartDetachment||Rc_Ctrl.isOnline == 0)
	{
		base_pid->KiPart = 0;
	}
	else if(base_pid->Error < -base_pid->KiPartDetachment||Rc_Ctrl.isOnline == 0)
	{
		base_pid->KiPart = 0;
	}
	
	base_pid->KiPart += base_pid->Error * base_pid->Ki;
	base_pid->KdPart = base_pid->Kd * (base_pid->Error -base_pid->LastError);

	base_pid->LastError = base_pid->Error;

	base_pid->Out = base_pid->KpPart + base_pid->KiPart + base_pid->KdPart;
	
	return base_pid->Out;
}

uint32_t BasePID_Slope_EcdControl( BasePID_Object* base_pid,int16_t target_ecd,int16_t raw_ecd)
{
	base_pid->Error = target_ecd - raw_ecd;
  base_pid->KpPart = base_pid->Error*base_pid->Kp;
	
	if(base_pid->Error > base_pid->KiPartDetachment||Rc_Ctrl.isOnline == 0)
	{
		base_pid->KiPart = 0;
	}
	else if(base_pid->Error < -base_pid->KiPartDetachment||Rc_Ctrl.isOnline == 0)
	{
		base_pid->KiPart = 0;
	}
	
	base_pid->KiPart += base_pid->Error * base_pid->Ki;
	base_pid->KdPart = base_pid->Kd * (base_pid->Error -base_pid->LastError);

	base_pid->LastError = base_pid->Error;

	base_pid->Out = base_pid->KpPart + base_pid->KiPart + base_pid->KdPart;
	
	return base_pid->Out;


}


