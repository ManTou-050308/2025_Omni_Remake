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
  * @brief  ȫ���ֵ��̽ṹ��
  */
typedef struct Omni_Chassis_T
{
	struct 
	{
		uint8_t Enable;    						//ʹ��״̬			
		uint8_t isRefereeUpdating; 		//����ϵͳ�Ƿ��ڸ�������
		uint8_t DriveMode;						//����ģʽ
		uint8_t ChassisState;					//����״̬
		uint8_t ChassisFollow_Flag;		//���̸���
		uint8_t ChassisFollow_Flag_Last;
		uint8_t RollingFlag;					//�Ƿ�����
		uint8_t SlopeFlag;						//�Ƿ�����
		uint8_t RollingFlagLast;       //�ϴ���ת��־λ
	}Flag;

	struct 
	{
		Motor motor[4];								//���̵���ṹ��
		BasePID_Object RunPID[4];			//�ٶȿ���PID����
		BasePID_Object FollowPID;			//���̸���PID����
		float initial_give_power[4];	//��ʼ�������Ĺ���
		float scaled_give_power[4];		//�����Ժ�Ĺ���
	}Motors3508;

	struct 
	{
		double Vx;;							//ǰ���˶����ٶ�
		double Vy;		  				//�����˶����ٶ�
		double Omega;					//��ת�Ľ��ٶ�
		double V;
		double RollingOmega;		//�����Ľ��ٶȣ�ֻģ����ʾ��С����λ����
		double ModuleOfSpeed;
		double now_Vx;
		double now_Vy;
		double now_V;
		double Last_V;
	}Movement;

	struct 
	{
		float Rc_Ctrl_ModuleOfSpeed;	 //Rc�����ٶ�ģ��
		float Rc_Ctrl_AngleOfSpeed;			//Rc�����ٶȽǶ�
	}RcVectors; //ң��������
	
	struct
	{
		float 		PowerLimitLast;		//��֡��������
		float 		SpeedPowerLimit;	//��֡��������
		uint8_t		SpeedUp;					//����ģʽ	
		uint8_t		SpeedUpLast;	
		uint8_t		PowerToSurperCap;	//�������
		
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
  * @brief  ȫ���ֵ��̳�ʼ��
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
