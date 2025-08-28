#include "hardware_config.h"

#include "vt13.h"
#include "dr16.h"
#include "motor.h"
#include "brain.h"
#include "heat_control.h"

#include "load.h"

#include "driver_timer.h"

#define continue_speed  0
#define single_speed    1
#define single_angle    2
#define sence          90
#define continue_load   1
#define single_load     2

void Load_Task(void)
{
		Shoot_Fire_Mode_Control(&Rc_Ctrl,&Loader,&Vision_Info,&VT13);
		Friction_BulletDisk_Fire_Control(&Loader,&Rc_Ctrl);

}
/**
 * @brief ���������ʼ��
 */

void LoaderInit(Loader_t *loader, BasePID_Object FrictionPID, BasePID_Object BulletPID_CON,BasePID_Object BulletPID_SINGLE_Speed,BasePID_Object BulletPID_SINGLE_Angle)
{
	MotorInit(&loader->LeftFriction.motor3508, 0, Motor3508, CAN1, 0x201);
	MotorInit(&loader->RightFriction.motor3508, 0, Motor3508, CAN1, 0x202);
	MotorInit(&loader->BulletDisk.motor2006,loader->BulletDisk.motor2006.Data.RawEcd, Motor2006, CAN2, 0x207);
	
	BasePID_Init(&loader->LeftFriction.PID, FrictionPID.Kp, FrictionPID.Ki, FrictionPID.Kd, FrictionPID.KiPartDetachment);
	BasePID_Init(&loader->RightFriction.PID, FrictionPID.Kp, FrictionPID.Ki, FrictionPID.Kd, FrictionPID.KiPartDetachment);
	
	BasePID_Init(&loader->BulletDisk.PID[continue_speed], BulletPID_CON.Kp, BulletPID_CON.Ki, BulletPID_CON.Kd, BulletPID_CON.KiPartDetachment);
	BasePID_Init(&loader->BulletDisk.PID[single_speed], BulletPID_SINGLE_Speed.Kp, BulletPID_SINGLE_Speed.Ki, BulletPID_SINGLE_Speed.Kd, BulletPID_SINGLE_Speed.KiPartDetachment);
	BasePID_Init(&loader->BulletDisk.PID[single_angle], BulletPID_SINGLE_Angle.Kp, BulletPID_SINGLE_Angle.Ki, BulletPID_SINGLE_Angle.Kd, BulletPID_SINGLE_Angle.KiPartDetachment);
	
	loader->Crowd_back_Flag = 0;			 //��ʼ����ת
	loader->One_Shoot_Sum = 0;
	loader->ContinuousShoot_Target_Sum = 0;
	loader->ContinuousShoot_Target_Sum_Cnt = 0;
	Muzzle.heat_status = 1;
	loader->Friction_Status_Flag = 1;  //Ħ����������־λ
	loader->Target_Firing_Rate = 8500;//1000; //������ת��
}

// referee2024.robot_status_t.shooter_barrel_cooling_value  
// referee2024.robot_status_t.shooter_barrel_heat_limit

// ������Ƶ���Ĳ������ٶ� 
// �ȼ����ı���Ƶ 
// ��ǹ�ܻ�ȡ��������������Ƶ �����������ٶ� ��ֹ����������������û����� ���³�����
// �Բ����̵Ŀ����߼��ǿ���Ŀ�������û�п���ʵ�ʵĴ���
/**
 * @brief ��Ƶ����
 */

void ChoosingFireRate(Loader_t *loader,Referee2024 *referee,float Hz_Cnt)
{
	float BulletPerSecond_Limit;//��ʹ��������������£�ÿms������ܴ���ĵ�����ȡ������ȴ�ٶȣ�
	float Heat_Remain;
	
	BulletPerSecond_Limit = referee->robot_status_t.shooter_barrel_cooling_value / 100.0f;
	Heat_Remain  =  (referee->robot_status_t.shooter_barrel_heat_limit -  referee2024.power_heat_data_t.shooter_17mm_1_barrel_heat)/5.5;

	
	loader->Target_Firing_Rate = (BulletPerSecond_Limit + Heat_Remain ) * 40 * 90 * 60/360 ;//* 45 * 90 * 60 / 360 *1.15;
	if(Vision_Info.Hit_Mode == 1) loader->Target_Firing_Rate = 9000;
//	loader->Target_Firing_Rate = 3000
	if(loader->Target_Firing_Rate < 1000) loader->Target_Firing_Rate = 0;
	if(loader->Target_Firing_Rate > 8500 || loader->Target_Firing_Rate < 0 || referee->robot_status_t.shooter_barrel_heat_limit > 4000) loader->Target_Firing_Rate = 8500;
}


float max;
/**
 * @brief PID����ѡ��Ħ���ֻ��ǲ�����
 */
float Friction_Bullet_Pid_Control(BasePID_Object *base_pid,BasePID_Object *base_pid_speed, BasePID_Object *base_pid_angle, float Feedback, int16_t target, uint8_t Motorflag)
{
	if (Motorflag == FRICTION)
	{
		base_pid->Error = target - Feedback;

		base_pid->KpPart = base_pid->Error * base_pid->Kp;

		if (base_pid->Error < base_pid->KiPartDetachment && base_pid->Error > -base_pid->KiPartDetachment) // ��������
			base_pid->KiPart += base_pid->Error * base_pid->Ki;

		if (target == 0)
			base_pid->KiPart = 0;
		if (base_pid->KiPart > 1000)
			base_pid->KiPart = 1000;
		if (base_pid->KiPart < -1000)
			base_pid->KiPart = -1000;

		base_pid->Out = base_pid->KpPart + base_pid->KiPart;

		if (base_pid->Out > 16384)
			base_pid->Out = 16384;
		if (base_pid->Out < -16384)
			base_pid->Out = -16384;
		return base_pid->Out;
	}
	if (Motorflag == BULLET_CON)
	{

		base_pid->Error = target - Feedback;
		if(myABS(base_pid->Error) < 500 ) base_pid->Error = 0;
		
		base_pid->KpPart = base_pid->Error * base_pid->Kp;
		if (target == 0)
			base_pid->KiPart = 0;
		if (base_pid->KiPart > 1200)
			base_pid->KiPart = 1000;
		if (base_pid->KiPart < -1200)
			base_pid->KiPart = -1000;
		base_pid->KdPart = base_pid->Error * base_pid->Kd;
		base_pid->Out = base_pid->KpPart + base_pid->KiPart + base_pid->KdPart;

		return base_pid->Out;
	}
	if (Motorflag == BULLET_SINGLE) //�Ƕ�
	{
			base_pid->Out = BasePID_AngleControl_BULEET(base_pid_angle,target,Feedback);

		return base_pid->Out;
	}
	
	return 0;
}


/**
 * @brief ң�������ƴ�
 */
void DR16_Control_Shoot(RC_Ctrl_t *rc_ctrl, Loader_t *loader)
{
	if(rc_ctrl->rc.s2 == 1)	loader->FireFlag = rc_ctrl->rc.s1;
	else {
		loader->FireFlag = 0;
	}
//	if(loader->FireFlagLast == 0 && loader->FireFlag !=3)  
	//�ϣ�1		�У�3	�£�2
}

void Shoot_Mode_Change(RC_Ctrl_t *rc_ctrl, Loader_t *loader,VT13_t* vt13)
{
	
		DR16_Control_Shoot(rc_ctrl, loader);
		ChoosingFireRate(loader,&referee2024,1000);
	
		/* ң�������� ��*/
			if((loader->FireFlag == 1 && loader->FireFlagLast == 3) || vt13->rc.photo == 1)  
		{
			loader->shoot_flag.dr16 = continue_load;
			loader->FireFlagLast = 3;
		}
		else if (loader->FireFlag == 2 && loader->FireFlagLast == 3)
		{
			loader->shoot_flag.dr16 = single_load;
		}
		else
		{
			loader->FireFlagLast =  loader->FireFlag ;
			loader->shoot_flag.dr16 = 0;
		}
   /*   ������ ��*/
		if(rc_ctrl->mouse.press_l_flag == 1 || vt13->isOnline* vt13->mouse.press_l_flag) 
		{
			loader->MouseOneShootCut++;	
			
			loader->shoot_flag.mouse = single_load;
		
			if(loader->MouseOneShootCut >= 100) {
				loader->shoot_flag.mouse = continue_load;
				loader->MouseOneShootCut = 100;
			}
			
			loader->BulletDisk.Angle_Plate = 0;
		}
		else {
			loader->MouseOneShootCut = 0;
			loader->shoot_flag.mouse = 0; 
		}
			
		if( loader->shoot_flag.mouse == continue_load || loader->shoot_flag.dr16 == continue_load)	loader->Shoot_Mode = continue_load;
		else if  (loader->shoot_flag.mouse == single_load || loader->shoot_flag.dr16 == single_load) loader->Shoot_Mode = single_load;
		else loader->Shoot_Mode = 0;


}
/**
 * @brief ��������������
 */
uint32_t shoot_delay = 50 ;
int flag_vision = 1;
void Shoot_Fire_Mode_Control(RC_Ctrl_t *rc_ctrl, Loader_t *loader,Trace_t* info_vision,VT13_t* vt13)
{

		Shoot_Mode_Change(rc_ctrl,loader,vt13);
		
			/*  ��������  */
		if(IfOverHeat() == 1 && (rc_ctrl->isOnline == 1 || vt13->isOnline == 1))
		{	
			if( (info_vision->Hit_Mode == 3) && loader->Crowd_back_Flag == 0 ) //��һ��Ƶ���µ����� ��Ƶ�ʽ���ʵʱ����
			{
				if (loader->Shoot_Mode == single_load )
				{
					loader->One_Shoot_Sum = 1;
				}
				else if(loader->Shoot_Mode == continue_load)
				{
					loader->One_Shoot_Sum = 0;
					loader->ContinuousShoot_Target_Sum ++;

					if(loader->Shoot_Mode_Last == 0)
					{
						loader->BulletDisk.motor2006.Param.EcdOffset = loader->BulletDisk.motor2006.Data.RawEcd ;
						loader->BulletDisk.motor2006.Data.TotalAngle = 0 ;
					}	
				}
				else
				{
					loader->One_Shoot_Sum = 0;
					loader->ContinuousShoot_Target_Sum = 0;
					loader->BulletDisk.ALL_Angle = 0;
					loader->BulletDisk.Angle_Plate = 0;
					loader->BulletDisk.TaretSpeed = 0;
				}
				
			}
			else if (info_vision->Hit_Mode == 0  && loader->Crowd_back_Flag == 0)
			{ 
				if(loader->Shoot_Mode == single_load )
				{
					if(Vision_Info.Flag_Vision_Del_Small == 1)
					loader->One_Shoot_Sum = 1;
				}
				else if(loader->Shoot_Mode == continue_load )
				{
					loader->One_Shoot_Sum = 0;
					if(loader->ContinuousShoot_Target_Sum == 0 && Vision_Info.Flag_Vision_Del_Small == 1 )  loader->ContinuousShoot_Target_Sum = 1;
					
					if(flag_vision == 1)
					{
						shoot_delay++;
						if(shoot_delay % 90 ==0 )
						{
							if(Vision_Info.Flag_Vision_Del_Small == 1) 
							{
								loader->ContinuousShoot_Target_Sum +=1;	
								shoot_delay = 0;
							}
							else 
							{
								flag_vision = 0 ;
								loader->ContinuousShoot_Target_Sum = 0;
								loader->BulletDisk.Angle_Plate = 0;
							}

						}
					}
					
					if((loader->ContinuousShoot_Target_Sum_Last !=  loader->ContinuousShoot_Target_Sum) && loader->ContinuousShoot_Target_Sum != 0)
					{
						flag_vision = 1;
					}
					loader->ContinuousShoot_Target_Sum_Last =  loader->ContinuousShoot_Target_Sum ;
					
				}
				else{
					loader->ContinuousShoot_Target_Sum = 0;
					loader->BulletDisk.ALL_Angle = 0;
					loader->BulletDisk.Angle_Plate = 0;
					loader->BulletDisk.TaretSpeed = 0;	
					loader->One_Shoot_Sum = 0;					
				}

			}
			else if(info_vision->Hit_Mode != 0 && info_vision->Hit_Mode != 3 &&  loader->Crowd_back_Flag == 0) 
			{
				
				if(Vision_Info.Single_Flag_Vision_Del_Small == 1 && info_vision->Last_Single_Flag_Vision_Del_Small == 0 )	
				{
					
					loader->One_Shoot_Sum  += 1 ;
				
				}
			}
			
		}
		else{
			loader->One_Shoot_Sum = 0;
			loader->ContinuousShoot_Target_Sum = 0;
			loader->BulletDisk.ALL_Angle = 0;
			loader->BulletDisk.Angle_Plate = 0;
			loader->BulletDisk.TaretSpeed = 0;
		}
	
		info_vision->Last_Single_Flag_Vision_Del_Small = info_vision->Single_Flag_Vision_Del_Small;
		loader->Shoot_Mode_Last = loader->Shoot_Mode;
	
}

/**
 * @brief �����̿���
 */
int delay;
int finsh_shoot;
void BulletDisk_Shoot_control(Loader_t *loader, RC_Ctrl_t *rc_ctrl,Trace_t* info_vision)
{
	/*	�����������Ƕȼ���	*/
	loader->BulletDisk.ALL_Angle = (loader->One_Shoot_Sum + loader->ContinuousShoot_Target_Sum ) * EMISSION_ANGLE /*+ loader->BulletDisk.Continue_ForwardAngle*/;
	
	if(info_vision->Hit_Mode == 1  || info_vision->Hit_Mode == 2)
	{
		delay = 300;
		finsh_shoot = 1 ;
	}
	else{
		delay = 500;
		finsh_shoot = 5;
	}
	 /*		���������ܽǶ���ʵ�ʲ����̻��ֽǶ��Ƿ�һ��		*/
	if(loader->Crowd_back_Flag == 0 )
    {
				//< ���С��5����Ŀ��Ƕȴ�����ת���ĽǶ���Ϊ��ɴ�
        if ((loader->BulletDisk.ALL_Angle - loader->BulletDisk.Angle_Plate) <= finsh_shoot && (loader->BulletDisk.ALL_Angle - loader->BulletDisk.Angle_Plate)>=0) 
        {
           loader->BulletDisk.TaretSpeed = 0;
        }
        else if((loader->BulletDisk.ALL_Angle - loader->BulletDisk.Angle_Plate)<-10)			//<ת��ͷ��
        {
						loader->BulletDisk.TaretSpeed = -3500;
        }    
        else if(loader->BulletDisk.ALL_Angle - loader->BulletDisk.Angle_Plate>10)  //ת���� ���ж�ת����
        {
					loader->BulletDisk.TaretSpeed = loader->Target_Firing_Rate;

					/*	��ת�ж�		*/
					if(loader->BulletDisk.motor2006.Data.SpeedRPM <50 && (loader->Shoot_Mode == continue_load || info_vision->Hit_Mode == 1 ) )
					{
						loader->Stuck_Delay++;
						if(loader->Stuck_Delay>delay)//150
						{
							loader->Crowd_back_Flag = 1;//1��ת
							loader->Stuck_Delay = 0;
						}
					}
					else
					{
							loader->Stuck_Delay = 0;
					}
        }
		}
    else if(loader->Crowd_back_Flag == 1) //<�������˵�
    {
			loader->BulletDisk.TaretSpeed = -5000;
			
			loader->Inversion_Delay++;
			if(loader->Inversion_Delay > 300)
			{
				loader->ContinuousShoot_Target_Sum = 0;
				loader->Crowd_back_Flag = 0;
				loader->Inversion_Delay = 0;
				loader->One_Shoot_Sum  = 0;
			}
    }

		
		loader->BulletDisk.ALL_Angle_Last = loader->BulletDisk.ALL_Angle;
}

int   Friction_Start_Cnt;
float Friction_Stop_Cnt = 0;
float delta_angle = 0; //<ÿ֡���ݵ�����
//int   Friction_Speed_Left =3000;// 7000;
//int   Friction_Speed_Right =3000;// 6990;

int Friction_Speed_Left =5700;// 7000;
int Friction_Speed_Right =5700;// 6990;
/**
 * @brief  Ħ���ֿ���+�����ıջ� �úõ� ���ٷ�����ʱ
 * @note   ����˫������ �޶�ת�ж� Ŀǰû���������� ʵ�ֹ�Ħ����������SENCE���ٽǶȻ������
 */
void Friction_BulletDisk_Fire_Control(Loader_t *loader, RC_Ctrl_t *rc_ctrl)
{

	if (loader->Friction_Status_Flag == 1 && tim14.ClockTime > 400 && (rc_ctrl->isOnline == 1 || VT13.isOnline == 1))
	{
     Friction_Stop_Cnt = 0;
     Friction_Start_Cnt++;
		/*���ٴ���*/				   // ʼ���Ե�ǰ������� ����������
		if (Friction_Start_Cnt > 1000) // 1s������
		{
			Friction_Start_Cnt = 1000;
		}
		
		loader->LeftFriction.TaretSpeed  =  Friction_Speed_Left * Friction_Start_Cnt / 1000;
		loader->RightFriction.TaretSpeed = -Friction_Speed_Right * Friction_Start_Cnt / 1000;
		
//		loader->Target_Firing_Rate = BULLET_DISK_SPEED;
		
		// ���������ת��Ϊ�Ƕ�Load_Info.speed_raw * 2�У����Ǹõ��Ϊ1/36���ٵ����������Ҫ/36��
		// ��������������ֵ�����磺ʵ��תһȦ��348����˴˴���Ҫ*(360/348)���˴�������������Ե���
        delta_angle = loader->BulletDisk.motor2006.Data.SpeedRPM * 0.001f * BULLET_DISK_SENCE;
		if (fabs(delta_angle) < 0.01f)
			delta_angle = 0;							// ��ֹ�ܷ�
		loader->BulletDisk.Angle_Plate += delta_angle; // ������һ��ת���ĽǶ�
		BulletDisk_Shoot_control(loader, rc_ctrl,&Vision_Info);
		
		/*Ħ���ֻ�����*/
		

		loader->LeftFriction.motor3508.Data.Output = Friction_Bullet_Pid_Control(&loader->LeftFriction.PID, &loader->LeftFriction.PID, &loader->LeftFriction.PID,loader->LeftFriction.motor3508.Data.SpeedRPM, loader->LeftFriction.TaretSpeed, FRICTION);
		loader->RightFriction.motor3508.Data.Output = Friction_Bullet_Pid_Control(&loader->RightFriction.PID, &loader->RightFriction.PID,&loader->RightFriction.PID, loader->RightFriction.motor3508.Data.SpeedRPM, loader->RightFriction.TaretSpeed, FRICTION);
	
		loader->BulletDisk.motor2006.Data.Output = Friction_Bullet_Pid_Control(&loader->BulletDisk.PID[continue_speed],&loader->BulletDisk.PID[single_speed],&loader->BulletDisk.PID[single_angle], loader->BulletDisk.motor2006.Data.SpeedRPM, loader->BulletDisk.TaretSpeed, BULLET_CON);

//		if(loader->Shoot_Mode == continue_load)	loader->BulletDisk.motor2006.Data.Output = Friction_Bullet_Pid_Control(&loader->BulletDisk.PID[continue_speed],&loader->BulletDisk.PID[single_speed],&loader->BulletDisk.PID[single_angle], loader->BulletDisk.motor2006.Data.SpeedRPM, loader->BulletDisk.TaretSpeed, BULLET_CON);
//		if(loader->Shoot_Mode == single_load)		loader->BulletDisk.motor2006,Data.Output = Friction_Bullet_Pid_Control(

	}
	/*Ħ���ֻ�ͣ+������*/
	else if (rc_ctrl->isOnline == 0 || loader->Friction_Status_Flag == 0 || VT13.isOnline == 0)
	{
		Friction_Start_Cnt = 0;
		if ((myABS(loader->LeftFriction.motor3508.Data.SpeedRPM) >= 30) && (myABS(loader->RightFriction.motor3508.Data.SpeedRPM) >= 30))
		{
			Friction_Stop_Cnt += Friction_Speed_Left * 0.0008;

if (Friction_Stop_Cnt >= Friction_Speed_Left)
				Friction_Stop_Cnt = Friction_Speed_Left;
			
			loader->LeftFriction.TaretSpeed  = Friction_Speed_Left - Friction_Stop_Cnt;
			loader->RightFriction.TaretSpeed =-Friction_Speed_Right + Friction_Stop_Cnt;
			loader->LeftFriction.motor3508.Data.Output = Friction_Bullet_Pid_Control(&loader->LeftFriction.PID, &loader->LeftFriction.PID, &loader->LeftFriction.PID, loader->LeftFriction.motor3508.Data.SpeedRPM, loader->LeftFriction.TaretSpeed, FRICTION);
			loader->RightFriction.motor3508.Data.Output = Friction_Bullet_Pid_Control(&loader->RightFriction.PID, &loader->RightFriction.PID, &loader->RightFriction.PID, loader->RightFriction.motor3508.Data.SpeedRPM, loader->RightFriction.TaretSpeed, FRICTION);
		}
		else
		{
			loader->LeftFriction.motor3508.Data.Output = 0;
			loader->RightFriction.motor3508.Data.Output = 0;
		}
        loader->BulletDisk.motor2006.Data.Output = 0;
	}
	loader->RightFriction.motor3508.Data.Output = AmplitudeLimit(loader->RightFriction.motor3508.Data.Output,15000);
	loader->LeftFriction.motor3508.Data.Output  = AmplitudeLimit(loader->LeftFriction.motor3508.Data.Output,15000);
	
	MotorFillData(&loader->LeftFriction.motor3508, loader->LeftFriction.motor3508.Data.Output);
	MotorFillData(&loader->RightFriction.motor3508, loader->RightFriction.motor3508.Data.Output);
	MotorFillData(&loader->BulletDisk.motor2006, loader->BulletDisk.motor2006.Data.Output);
}
