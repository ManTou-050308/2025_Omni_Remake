#ifndef OMNICHASSIS_H_
#define OMNICHASSIS_H_

#include "stm32h7xx.h"
#include "devices.h"
#include "pid.h"
#include "holder.h"
#include "dr16.h"
#include "vt13.h"
#include "ins.h"
#include "hardware_config.h"



/**
  * @brief  全向轮底盘结构体
  */
typedef struct Omni_Chassis_T
{
	struct 
	{
		uint8_t Enable;    						//使能状态			
		uint8_t isRefereeUpdating; 		//裁判系统是否在更新数据
		uint8_t DriveMode;						//操作模式
		uint8_t ChassisState;					//底盘状态
		uint8_t ChassisFollow_Flag;		//底盘跟随
		uint8_t ChassisFollow_Flag_Last;
		uint8_t RollingFlag;					//是否自旋
		uint8_t SlopeFlag;						//是否上坡
		uint8_t RollingFlagLast;       //上次旋转标志位
	}Flag;

	struct 
	{
		Motor motor[4];								//底盘电机结构体
		BasePID_Object RunPID[4];			//速度控制PID参数
		BasePID_Object FollowPID;			//底盘跟随PID参数
		float initial_give_power[4];	//初始轮子消耗功率
		float scaled_give_power[4];		//缩放以后的功率
	}Motors3508;

	struct 
	{
		double Vx;;							//前后运动的速度
		double Vy;		  				//左右运动的速度
		double Omega;					//旋转的角速度
		double V;
		double RollingOmega;		//自旋的角速度，只模糊表示大小，单位不明
		double ModuleOfSpeed;
		double now_Vx;
		double now_Vy;
		double now_V;
		double Last_V;
	}Movement;

	struct 
	{
		float Rc_Ctrl_ModuleOfSpeed;	 //Rc比例速度模长
		float Rc_Ctrl_AngleOfSpeed;			//Rc比例速度角度
	}RcVectors; //遥控器向量
	
	struct
	{
		float 		PowerLimitLast;		//上帧功率限制
		float 		SpeedPowerLimit;	//此帧功率限制
		uint8_t		SpeedUp;					//超电模式	
		uint8_t		SpeedUpLast;	
		uint8_t		PowerToSurperCap;	//给超电的
		
		float max_power;
		float now_power;
		float target_require_power_sum;
		double scaling_ratio;
	}Power;
	
	struct
	{
		float Current[4];
		float Target[4];
		float step[4];
	}COM;

}Omni_Chassis_t;

typedef enum 
{
	UP    = 0x00U,
	DOWN  = 0x01U,
	LEFT  = 0X02U,
	RIGHT = 0x03U
}Offest;

/**
  * @brief  全向轮底盘初始化
  */
void 	 OmniChassisInit(Omni_Chassis_t* chassis, BasePID_Object pid_run, BasePID_Object pid_follow, CanNumber canx);

float  OmniPowertoSpeed(float target_power);
void	OmniChassis_RemoteControl(Omni_Chassis_t* chassis, float canAngle, RC_Ctrl_t* rc_ctrl, Holder_t* holder,VT13_t* vt13);
void 	 OmniChassisSpeed_Power_Control(Omni_Chassis_t* chassis);
void   VectorSolve(float vx, float vy, float* vectorAngle, float* vectorModule);
void 	 OmniChassisMotionControl(Omni_Chassis_t* chassis,RC_Ctrl_t* rc_rtrl,Super_Cap_t* super_cap, Holder_t* holder);
void	OmniChassis_Control(Omni_Chassis_t* chassis, RC_Ctrl_t* rc_ctrl,Holder_t* holder,Super_Cap_t* super_cap,VT13_t* vt13);
float  OmniDampedFeedforward(float speed);
float  Slope_Com(Omni_Chassis_t* chassis,Holder_t* holder,Attitude_t* attitude);
void Offset_Com(Offest direction,Omni_Chassis_t* chassis,int Com);
extern float Angle_Dev_Pitch;
int Ramp(int current, int target, int step);
void OmniChassis_R_State(Omni_Chassis_t* chassis);
# endif
