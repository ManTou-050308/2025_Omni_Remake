#ifndef __HOLD_H
#define __HOLD_H
#include "stm32h7xx_hal.h"
#include "devices.h"
#include "pid.h"

	
#include "hardware_config.h"

#define Pitch_6020 	0
#define Yaw_6020		1

struct Holder_Motor_Info
{
	float 	Target_Angle;					//目标角度
	float 	Sensitivity;
	float 	MouseSensitivity;

	float 	v1;
	float 	v2;
};

typedef struct Holder_T
{
	struct Holder_Motor_Info Pitch;
	struct Holder_Motor_Info Yaw;
	struct 
	{
		Motor motor[2];								//< 底盘电机结构体
		BasePID_Object turnPID[10];		//< 转向角度控制结构体
		BasePID_Object visionPID[4];
		float FeedbackAngle[2];
		float HolderCanAngle[2];
		BasePID_Object Feedback_Speed;
		
	}Motors6020;
	
	struct
	{
		double Vx;
		double Vy;
	}HoalderCirl;
	
	
	/*	遥控器在相对于陀螺仪零点的陀螺仪角度	*/
	float		Gryo_Yaw_Target_Angle;
	float		Gryo_Pitch_Target_Angle;
	
	/*	陀螺仪零点为所需零点在陀螺仪的角度	*/
	float		Gryo_Yaw_Zero_Angle;
	float		Gryo_Pitch_Zero_Angle;
	
	uint8_t Reset_OK_Flag;
	uint8_t ESC_Reset_Flag;


}Holder_t;




void HolderReset(Holder_t* holder);
void HodlerInit(Holder_t* holder,CanNumber canx,BasePID_Object pitch_reset_pid,BasePID_Object pitch_angle_pid,BasePID_Object pitch_speed_pid,BasePID_Object pitch_angle_vision_pid,BasePID_Object pitch_speed_vision_pid ,BasePID_Object yaw_reset_pid ,BasePID_Object yaw_angle_pid,BasePID_Object yaw_speed_pid,BasePID_Object yaw_angle_vision_pid,BasePID_Object yaw_speed_vision_pid,BasePID_Object yaw_speed_vision_single_pid,BasePID_Object yaw_angle_vision_single_pid,BasePID_Object pitch_speed_vision_single_pid,BasePID_Object pitch_angle_vision_single_pid);
void HolderGetRemoteData(Holder_t* holder, RC_Ctrl_t* rc_ctrl,VT13_t* vt13);
void Zero_Init(Holder_t* holder);
float Holder_TD(struct Holder_Motor_Info* holder_info,float Expect,float r,float h);
void Change_limit(float dev,Holder_t* holder,RC_Ctrl_t* rc_Ctrl);
extern float k_holder ;
extern float speed;;
#endif
