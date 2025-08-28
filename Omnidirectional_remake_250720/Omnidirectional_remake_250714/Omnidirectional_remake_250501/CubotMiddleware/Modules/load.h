#ifndef __LOAD_H
#define __LOAD_H

#include "motor.h"
#include "pid.h"

#include "hardware_config.h"

//#define EMISSION_ANGLE 45   //<������һ������ת���ĽǶ�

#define FRICTION 1 //<Ħ����PID
#define BULLET_CON 2  //<����PIDѡ��
#define BULLET_SINGLE  3
#define BULLET_DISK_SPEED     4000//<������ת��
#define HEAT_CACHE  48//<���������ֹ������
#define BULLET_DISK_SENCE 0.06778f//0.068919f//0.071733f//0.16941f //<�����̻���ϵ��
#define EMISSION_ANGLE 40   //<������һ������ת���ĽǶ�
#define FINISH_SHOOT  5 //<�����̽��������Ƕ�




/*
    --------------------------------------------------------------
    RM2025�����ļ������17mm���������  ÿ�ŵ�������10
    ����������ͣ� �ȼ��� �������ޣ� ÿ����ȴֵ      100ms����һ����ȴֵ     1ms��ȴֵ    ��ȴֵ��Ƶ
        
    �������ȣ�   
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


    ��ȴ���ȣ�   referee2024.robot_status_t.robot_level * 5 + 35
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
											
											ÿ��������ȴ��Ƶ����
											
											����ȴֵ���ɻ�������
											
											����ȴֵ���ջ����� ����ȴ����������
         
    Ĭ�ϣ�
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


//<Ħ������Ϣ
typedef struct
{
    Motor           motor3508;
    BasePID_Object  PID;
    float           TaretSpeed;
}Friction_Info;

//<��������Ϣ
typedef struct 
{
    Motor motor2006;
    BasePID_Object PID[3];
    float Angle_Plate;//<�����̻��ֳ��ĵ�ǰ�Ƕ�
    float OneShoot_TargetAngle;//<�����̵���Ŀ��Ƕ�
    float ContinuousShoot_TargetAngle;//<����������Ŀ��Ƕ�
    float ALL_Angle;//<��������Ŀ��Ƕ�
    float ALL_Angle_Last;//<�������ϴ���Ŀ��Ƕ�
    int	  TaretSpeed;
		float	FeedBack_Angle;
		float Continue_ForwardAngle;
}BulletDisk_Info;

//<�������
typedef struct Loader_T
{
	Shoot         shoot_flag;
	Friction_Info LeftFriction;
	Friction_Info RightFriction;
	BulletDisk_Info BulletDisk;
	int8_t  Shoot_Mode;           //<����ģʽ
	int8_t  Shoot_Mode_Last;			//<�ϴη���ģʽ
	uint64_t MouseOneShootCut; //<����ģʽ�µļ��㰴������Լ��ɿ�����
	volatile long int ContinuousShoot_Target_Sum;      //<Ŀ��������������
	volatile long int ContinuousShoot_Target_Sum_Last;
	volatile long int One_Shoot_Sum;       //<Ŀ�굥������
	volatile long int Shoot_Bullet_Sum;      //<����ϵͳ��ⷢ������
	volatile long int Last_Shoot_Bullet_Sum; //<��һʱ�̲���ϵͳ��ⷢ������
	volatile long int ContinuousShoot_Target_Sum_Cnt;   //<����������Ƶ
	uint32_t Target_Firing_Rate;   //<����������Ŀ���ٶ�
	uint8_t DaFu_Shoot_Flag;     //<���ģʽ��־λ
	uint16_t ShootWaitTime;     //<������ӳ�
	uint8_t FireFlag;            //<ң�������ƴ򵯱�־λ
	uint8_t FireFlagLast;           //<ң�������ƴ򵯱�־λ�ϴ�
	uint8_t Stop_Back_Flag;      //<ֹͣ��־λ
	uint8_t Friction_Status_Flag;//<Ħ����������־λ

	uint8_t Crowd_back_Flag;     //<��ת��־λ
	uint16_t Inversion_Delay;   //<�˵��ӳ�
	uint16_t Stuck_Delay; //<�����ӳ�
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
