#ifndef __LOAD_H
#define __LOAD_H

#include "motor.h"
#include "pid.h"


/*
    --------------------------------------------------------------
    RM2025规则文件：针对17mm发射机构：  每颗弹丸热量10
    发射机构类型： 等级： 热量上限： 每秒冷却值      100ms清算一次冷却值
        
    爆发优先：   
                      1        200         10            
                      2        250         15         
                      3        300         20 
                      4        350         25         
                      5        400         30         
                      6        450         35         
                      7        500         40         
                      8        550         45         
                      9        600         50         
                      10       650         60                


    冷却优先：
                      1        50          40          
                      2        85          45         
                      3        120         50 
                      4        155         55         
                      5        190         60         
                      6        225         65         
                      7        260         70         
                      8        295         75         
                      9        330         80         
                      10       400         80   
											
         
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
//typedef struct 
//{

//}Rate_Info;

////<摩擦轮信息
//typedef struct
//{
//    Motor motor3508;
//    BasePID_Object PID;
//    float TaretSpeed;
//}Friction_Info;

////<拨弹盘信息
//typedef struct 
//{
//    Motor motor2006;
//    BasePID_Object PID;
//}BulletDisk_Info;

////<发射机构
//typedef struct Loader_T
//{
//	Friction_Info LeftFriction;
//	Friction_Info RightFriction;
//	BulletDisk_Info BulletDisk;
//	
//	int8_t State_Mode; //发射机构状态
//	
//	int8_t  Shoot_Mode;           //<发射模式
//	uint8_t MouseOneShootCut; //<单发模式下的计算按下左键自加松开清零
//	volatile long int ContinuousShoot_Target_Sum;      //<目标连发弹丸总数
//	volatile long int One_Shoot_Sum;       //<目标单发总数
//	volatile long int Shoot_Bullet_Sum;      //<裁判系统检测发弹总数
//	volatile long int Last_Shoot_Bullet_Sum; //<上一时刻裁判系统检测发弹总数
//	volatile long int ContinuousShoot_Target_Sum_Cnt;   //<发弹连发降频
//	int16_t ShootRate;      //<降频速度
//	int16_t Target_Firing_Rate;   //<决定拨弹盘目标速度
//	uint8_t DaFu_Shoot_Flag;     //<打符模式标志位
//	uint16_t ShootWaitTime;     //<打符打弹延迟
//	uint8_t FireFlag;            //<遥控器控制打弹标志位
//	uint8_t FireFlagLast;           //<遥控器控制打弹标志位上次
//	uint8_t Stop_Back_Flag;      //<停止标志位
//	uint8_t Friction_Status_Flag;//<摩擦轮启动标志位
//	uint8_t SuperHeatMode;      //<超热量模式
//	int16_t Forward_Reverse_Flag; //<不同电机转的方向不同，拨弹逻辑只支持正转，因此需要一个标志位将该变的量变成负值暂时未用到先保留
//	uint8_t Crowd_back_Flag;     //<反转标志位
//	uint16_t Inversion_Delay;   //<退弹延迟
//	uint16_t Stuck_Delay; //<卡弹延迟
//	uint8_t Refree_StartFlag;//裁判系统开比赛标志位
//	uint8_t Brain_IsFire_Last;
//	uint8_t Brain_DelSmall;
//	
//}Loader_t;
#endif
