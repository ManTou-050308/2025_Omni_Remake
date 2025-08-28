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
//		uint8_t Enable;    						//ʹ��״̬			
//		uint8_t isRefereeUpdating; 				//����ϵͳ�Ƿ��ڸ�������
//		uint8_t DriveMode;						//����ģʽ
//		uint8_t ChassisState;					//����״̬
//		uint8_t SinRollingFlag;					//�Ƿ��������
//		uint8_t RollingFlag;					//�Ƿ�����
//		uint8_t SpeedUp;						//�����Ƿ����
//		uint8_t SpeedUp_Last;					//�ϴε����Ƿ����
//	}Flag;

//	struct 
//	{
//		Motor motor[4];							//���̵���ṹ��
//		BasePID_Object pid[4];				//�ٶȿ���PID����
//		BasePID_Object FollowPID;			//���̸���PID����
//	}Motors3508;

//	struct 
//	{
//		Motor motor[4];							//���̵���ṹ��
//		BasePID_Object pid[4];				//ת��Ƕȿ��ƽṹ��
//	}Motors6020;

//	struct 
//	{
//		int32_t Vx;					//ǰ���˶����ٶ�
//		int32_t Vy;		  			//�����˶����ٶ�
//		int32_t Omega;				//��ת�Ľ��ٶ�
//		int32_t RollingOmega;		//�����Ľ��ٶȣ�ֻģ����ʾ��С����λ����
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
//		int32_t ModuleOfSpeed;		//�ٶ�������ģֵ
//		float AngleOfSpeed;			//�ٶ������ĽǶ�
//		float K_OmeToSpeed;
//		float MovingRolling;		//����������ƽ�Ƶķ���(û��)
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
//		float K_OnlyRolling;//ֻ����ʱ����Ŀ���ٶȵı��������Ŷ��ƺ���׼
//		float K_MovingRolling;//����ƽ��ʱ�ı���
//		float K_Turning;//ת��ʱ�ı���
//		float Power_Plus;//Ŀ�깦������
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
