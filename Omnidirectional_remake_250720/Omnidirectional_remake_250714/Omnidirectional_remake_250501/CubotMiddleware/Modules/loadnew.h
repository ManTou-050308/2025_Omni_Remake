#ifndef __LOAD_H
#define __LOAD_H

#include "motor.h"
#include "pid.h"


/*
    --------------------------------------------------------------
    RM2025�����ļ������17mm���������  ÿ�ŵ�������10
    ����������ͣ� �ȼ��� �������ޣ� ÿ����ȴֵ      100ms����һ����ȴֵ
        
    �������ȣ�   
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


    ��ȴ���ȣ�
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
//typedef struct 
//{

//}Rate_Info;

////<Ħ������Ϣ
//typedef struct
//{
//    Motor motor3508;
//    BasePID_Object PID;
//    float TaretSpeed;
//}Friction_Info;

////<��������Ϣ
//typedef struct 
//{
//    Motor motor2006;
//    BasePID_Object PID;
//}BulletDisk_Info;

////<�������
//typedef struct Loader_T
//{
//	Friction_Info LeftFriction;
//	Friction_Info RightFriction;
//	BulletDisk_Info BulletDisk;
//	
//	int8_t State_Mode; //�������״̬
//	
//	int8_t  Shoot_Mode;           //<����ģʽ
//	uint8_t MouseOneShootCut; //<����ģʽ�µļ��㰴������Լ��ɿ�����
//	volatile long int ContinuousShoot_Target_Sum;      //<Ŀ��������������
//	volatile long int One_Shoot_Sum;       //<Ŀ�굥������
//	volatile long int Shoot_Bullet_Sum;      //<����ϵͳ��ⷢ������
//	volatile long int Last_Shoot_Bullet_Sum; //<��һʱ�̲���ϵͳ��ⷢ������
//	volatile long int ContinuousShoot_Target_Sum_Cnt;   //<����������Ƶ
//	int16_t ShootRate;      //<��Ƶ�ٶ�
//	int16_t Target_Firing_Rate;   //<����������Ŀ���ٶ�
//	uint8_t DaFu_Shoot_Flag;     //<���ģʽ��־λ
//	uint16_t ShootWaitTime;     //<������ӳ�
//	uint8_t FireFlag;            //<ң�������ƴ򵯱�־λ
//	uint8_t FireFlagLast;           //<ң�������ƴ򵯱�־λ�ϴ�
//	uint8_t Stop_Back_Flag;      //<ֹͣ��־λ
//	uint8_t Friction_Status_Flag;//<Ħ����������־λ
//	uint8_t SuperHeatMode;      //<������ģʽ
//	int16_t Forward_Reverse_Flag; //<��ͬ���ת�ķ���ͬ�������߼�ֻ֧����ת�������Ҫһ����־λ���ñ������ɸ�ֵ��ʱδ�õ��ȱ���
//	uint8_t Crowd_back_Flag;     //<��ת��־λ
//	uint16_t Inversion_Delay;   //<�˵��ӳ�
//	uint16_t Stuck_Delay; //<�����ӳ�
//	uint8_t Refree_StartFlag;//����ϵͳ��������־λ
//	uint8_t Brain_IsFire_Last;
//	uint8_t Brain_DelSmall;
//	
//}Loader_t;
#endif
