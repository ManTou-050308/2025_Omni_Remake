//#ifndef __CHASSIS_H__
//#define __CHASSIS_H__

//#include "stm32h7xx.h"
//#include "pid.h"
//#include "motor.h"
//#include "dr16.h"


//#define vx_Sence 0.04f
//#define vy_Sence 0.06f

//typedef struct 
//{
//	struct
//	{
//		uint8_t Enable;    						//使能状态			
//		uint8_t isRefereeUpdating; 				//裁判系统是否在更新数据
//		uint8_t DriveMode;						//操作模式
//		uint8_t ChassisState;					//底盘状态
//		uint8_t SinRollingFlag;					//是否变速自旋
//		uint8_t RollingFlag;					//是否自旋
//		uint8_t SpeedUp;						//底盘是否加速
//		uint8_t SpeedUp_Last;					//上次底盘是否加速
//	}Flag;

//	struct 
//	{
//		Motor motor[4];							//底盘电机结构体
//		BasePID_Object pid[4];				//速度控制PID参数
//		BasePID_Object FollowPID;			//底盘跟随PID参数
//	}Motors3508;

//	struct 
//	{
//		Motor motor[4];							//底盘电机结构体
//		BasePID_Object pid[4];				//转向角度控制结构体
//	}Motors6020;

//	struct 
//	{
//		int32_t Vx;					//前后运动的速度
//		int32_t Vy;		  			//左右运动的速度
//		int32_t Omega;				//旋转的角速度
//		int32_t RollingOmega;		//自旋的角速度，只模糊表示大小，单位不明
//		int32_t	DeltaVx;
//		int32_t	DeltaVy;
//		int16_t	DeltaOmega;
//		uint8_t MoveFlag;
//		struct
//		{
//			float Vx;
//			float Vy;
//			float Omega;
//		}Sensitivity;
//		
//		int32_t ModuleOfSpeed;		//速度向量的模值
//		float AngleOfSpeed;			//速度向量的角度
//		float K_OmeToSpeed;
//		float MovingRolling;		//调节自旋中平移的方向(没用)
//		
//	}Movement;

//	struct 
//	{
//		float Rc_Ctrl_ModuleOfSpeed;
//		float Rc_Ctrl_AngleOfSpeed;		
//		int32_t Vx[4];
//		int32_t Vy[4];
//		float Angle[4];	
//		float BestAngle[4];
//		int16_t TargetEcd[4];
//		float FeedbackAngle[4];
//		float SpeedNo[4];
//		uint8_t SpeedChangeFlag[4];
//	}Vectors;
//	
//	struct
//	{
//		BasePID_Object PowerPID_Vmax;
//		uint8_t SuperCapIsOnline;
//		float SpeedPowerLimit;
//		float SpeedPowerLimit_initial;
//		uint8_t PowerLimitChangeFlag;
//		uint32_t PowerLimitLast;
//		float K_PowerPlus;
//		float K_OnlyRolling;//只自旋时对于目标速度的比例，根号二似乎不准
//		float K_MovingRolling;//自旋平移时的比例
//		float K_Turning;//转弯时的比例
//		float Power_Plus;//目标功率增幅
//		float UpLimit;
//		float DownLimit;
//		float PowerToSurperCap;
//	}Power;
//	
//}SwerveChassis;

//void Chassis_Init(SwerveChassis* chassis ,CanNumber canx,BasePID_Object run_pid, BasePID_Object turn_pid);
//void Chassis_Speedcal(SwerveChassis* chassis , RC_Ctrl* rc_ctrl);
//void VectorCal(float vx, float vy, float* vectorangle ,float* vectormodule ,int id);
//float GetBestAngle(float targetangle ,float nowangle,uint8_t * flagofspeedchange);
//void SwerveChassisMotionControl(SwerveChassis* chassis);
//void Chassis_Control(SwerveChassis* chassis,RC_Ctrl* rc_ctrl);
//float EcdtoAngle(int16_t offset, int16_t ecd);

//double getabs(double t);

//#endif
