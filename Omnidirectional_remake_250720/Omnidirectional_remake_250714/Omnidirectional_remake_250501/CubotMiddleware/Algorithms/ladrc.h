#ifndef _LADRC_H
#define _LADRC_H
#include "stdint.h"

#define square(a) ((a)*(a))
#define cube(a) ((a)*(a)*(a))
/**
   *@Brief  ����ΪLADRCϵͳ����
   *@WangShun  2022-07-03  ע��
   */
typedef struct LADRC
{
    double v1,v2;         //�������ֵ
    double r;             //�ٶ�����
    double h;             //���ֲ���
    double z1,z2,z3;      //�۲������
		double w0,beta;				//�����������
    double wc,b0,u;    //�۲������� ���������� ϵͳ���� ���������
		double Kp,Kd;
		double u0;
		float Ki;
		float Kipart;
		float KiPartDetachment;
}LADRC_NUM;

/*
	wu = 2*3.1415/Pu;
    ku = 4*h/3.1415*a;

	wc = 2.3997wu - 0.4731;
	w0 = 0.7332wu + 3.5070;
	b0 = 3.6105wu + 4.8823;
*/
typedef struct Auto_Tuning 
{
	float Pu; //�̵�ʵ���������
	float a;  //�̵�ʵ�������ֵ
	float h;  //ָ�������ֵ
	float Wu; //ϵͳ�ٽ�Ƶ��
	float Kp; //ϵͳ�ٽ��ֵ
}AuTu;

/**
   *@Brief  ����Ϊ��Ҫ�����Ĳ���
   *@WangShun  2022-07-03  ע��
   */
extern LADRC_NUM  M6020_Angle[4];
extern LADRC_NUM  M3508_Speed; 
extern LADRC_NUM  M6020_Angle_Vision;
extern LADRC_NUM  M6020_Angle_Yaw;
extern LADRC_NUM   M2006_Speed; 

/**
   *@Brief  ����ΪLADRC��غ���
   *@WangShun  2022-07-03  ע��
   */
void LADRC_Init(LADRC_NUM *LADRC_TYPE,float h,float r,float kp,float kd,float b0,float beta,float ki,float kiPartDetachment);
void LADRC_REST(LADRC_NUM *LADRC_TYPE1);
void LADRC_TD(LADRC_NUM *LADRC_TYPE1,float Expect);
void LADRC_ESO(LADRC_NUM *LADRC_TYPE1,double y,double u);
void LADRC_LF_Angle(LADRC_NUM *LADRC_TYPE1,double Measure,double DevMeasure);
void LADRC_LF_Speed(LADRC_NUM *LADRC_TYPE1,double Measure);
void LADRC_Loop_Angle(LADRC_NUM *LADRC_TYPE1,double Expect_Value,double RealTimeOut,double Measure,double DevMeasure);
void LADRC_Loop_Speed(LADRC_NUM *LADRC_TYPE1,double Expect_Value,double RealTimeOut/*output*/,double Measure/*����*/);
void LADRC_LF_Angle_Vision(LADRC_NUM *LADRC_TYPE,double Measure,double DevMeasure);
void LADRC_Loop_Angle_Vision(LADRC_NUM *	LADRC_TYPE,double Expect_Value,double RealTimeOut/*output*/,double Measure/*????*/,double DevMeasure);
#endif
