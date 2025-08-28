#ifndef __LOAD_H
#define __LOAD_H

#include "motor.h"
#include "pid.h"

#include "hardware_config.h"

//#define EMISSION_ANGLE 45   //<拨弹盘一发弹丸转过的角度

#define FRICTION 1 //<摩擦轮PID
#define BULLET_CON 2  //<拨弹PID选择
#define BULLET_SINGLE  3
#define BULLET_DISK_SPEED     4000//<拨弹盘转速
#define HEAT_CACHE  48//<热量缓存防止超热量
#define BULLET_DISK_SENCE 0.06778f//0.068919f//0.071733f//0.16941f //<拨弹盘积分系数
#define EMISSION_ANGLE 40   //<拨弹盘一发弹丸转过的角度
#define FINISH_SHOOT  5 //<拨弹盘结束发弹角度




/*
    --------------------------------------------------------------
    RM2025规则文件：针对17mm发射机构：  每颗弹丸热量10
    发射机构类型： 等级： 热量上限： 每秒冷却值      100ms清算一次冷却值     1ms冷却值    冷却值射频
        
    爆发优先：   
                      1        200         10        1    
                      2        250         15        1.5
                      3        300         20 			 2
											4        350         25        2.5 
                      5        400         30        3 
                      6        450         35        3.5 
                      7        500         40        4 
                      8        550         45        4.5 
                      9        600         50        5 
                      10       650         60        6       


    冷却优先：   referee2024.robot_status_t.robot_level * 5 + 35
                      1        50          40        4 								  			0.4  				4  
                      2        85          45        4.5																	4
                      3        120         50 			 5
                      4        155         55        5.5 
                      5        190         60        6 
                      6        225         65        6.5 
                      7        260         70        7 
                      8        295         75        7.5 
                      9        330         80        8 
                      10       400         80   		 8
											
											每过两级冷却射频更新
											
											把冷却值看成缓冲能量
											
											对冷却值做闭环控制 对冷却速率做控制
         
    默认：
                      1        200         40          
                      2        230         48         
                      3        260         56 
                      4        290         64         
                      5        320         72         
                      6        350         80         
                      7        380         88         
                      8        410         96         
                      9        440         104         
                      10       500         120       
    --------------------------------------------------------------

*/
typedef struct
{
	int dr16;
	int mouse
}Shoot;


//<摩擦轮信息
typedef struct
{
    Motor           motor3508;
    BasePID_Object  PID;
    float           TaretSpeed;
}Friction_Info;

//<拨弹盘信息
typedef struct 
{
    Motor motor2006;
    BasePID_Object PID[3];
    float Angle_Plate;//<拨弹盘积分出的当前角度
    float OneShoot_TargetAngle;//<拨弹盘单发目标角度
    float ContinuousShoot_TargetAngle;//<拨弹盘连发目标角度
    float ALL_Angle;//<拨弹盘总目标角度
    float ALL_Angle_Last;//<拨弹盘上次总目标角度
    int	  TaretSpeed;
		float	FeedBack_Angle;
		float Continue_ForwardAngle;
}BulletDisk_Info;

//<发射机构
typedef struct Loader_T
{
	Shoot         shoot_flag;
	Friction_Info LeftFriction;
	Friction_Info RightFriction;
	BulletDisk_Info BulletDisk;
	int8_t  Shoot_Mode;           //<发射模式
	int8_t  Shoot_Mode_Last;			//<上次发射模式
	uint64_t MouseOneShootCut; //<单发模式下的计算按下左键自加松开清零
	volatile long int ContinuousShoot_Target_Sum;      //<目标连发弹丸总数
	volatile long int ContinuousShoot_Target_Sum_Last;
	volatile long int One_Shoot_Sum;       //<目标单发总数
	volatile long int Shoot_Bullet_Sum;      //<裁判系统检测发弹总数
	volatile long int Last_Shoot_Bullet_Sum; //<上一时刻裁判系统检测发弹总数
	volatile long int ContinuousShoot_Target_Sum_Cnt;   //<发弹连发降频
	uint32_t Target_Firing_Rate;   //<决定拨弹盘目标速度
	uint8_t DaFu_Shoot_Flag;     //<打符模式标志位
	uint16_t ShootWaitTime;     //<打符打弹延迟
	uint8_t FireFlag;            //<遥控器控制打弹标志位
	uint8_t FireFlagLast;           //<遥控器控制打弹标志位上次
	uint8_t Stop_Back_Flag;      //<停止标志位
	uint8_t Friction_Status_Flag;//<摩擦轮启动标志位

	uint8_t Crowd_back_Flag;     //<反转标志位
	uint16_t Inversion_Delay;   //<退弹延迟
	uint16_t Stuck_Delay; //<卡弹延迟
	uint8_t Brain_IsFire_Last;
	uint8_t Brain_DelSmall;
	
}Loader_t;


void LoaderInit(Loader_t *loader, BasePID_Object FrictionPID, BasePID_Object BulletPID_CON,BasePID_Object BulletPID_SINGLE_Speed,BasePID_Object BulletPID_SINGLE_Angle);
float Friction_Bullet_Pid_Control(BasePID_Object *base_pid,BasePID_Object *base_pid_speed, BasePID_Object *base_pid_angle, float Feedback, int16_t target, uint8_t Motorflag);
void DR16_Control_Shoot(RC_Ctrl_t *rc_ctrl, Loader_t *loader);
void Shoot_Fire_Mode_Control(RC_Ctrl_t *rc_ctrl, Loader_t *loader,Trace_t* info_vision,VT13_t* vt13);
void BulletDisk_Shoot_control(Loader_t *loader, RC_Ctrl_t *rc_ctrl,Trace_t* info_vision);
void Friction_BulletDisk_Fire_Control(Loader_t *loader, RC_Ctrl_t *rc_ctrl);
void Loader_CanSend_OutPut(RC_Ctrl_t *rc_ctrl);
void Load_Task(void);

extern int heat_limt,ontime_heat,haet_cool;
#endif
