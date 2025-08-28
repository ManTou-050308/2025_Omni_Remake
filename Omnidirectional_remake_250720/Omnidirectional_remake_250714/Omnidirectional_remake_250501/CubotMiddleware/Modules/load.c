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
 * @brief 发射机构初始化
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
	
	loader->Crowd_back_Flag = 0;			 //初始化堵转
	loader->One_Shoot_Sum = 0;
	loader->ContinuousShoot_Target_Sum = 0;
	loader->ContinuousShoot_Target_Sum_Cnt = 0;
	Muzzle.heat_status = 1;
	loader->Friction_Status_Flag = 1;  //摩擦轮启动标志位
	loader->Target_Firing_Rate = 8500;//1000; //拨弹盘转速
}

// referee2024.robot_status_t.shooter_barrel_cooling_value  
// referee2024.robot_status_t.shooter_barrel_heat_limit

// 根据射频更改拨弹盘速度 
// 等级来改变射频 
// 从枪管获取弹丸数量计算射频 补偿拨弹盘速度 防止在热量计算里面有没打完的 导致超热量
// 对拨弹盘的控制逻辑是控制目标的增加没有控制实际的打弹量
/**
 * @brief 射频控制
 */

void ChoosingFireRate(Loader_t *loader,Referee2024 *referee,float Hz_Cnt)
{
	float BulletPerSecond_Limit;//不使热量上升的情况下，每ms能最多能打出的弹数（取决于冷却速度）
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
 * @brief PID控制选择摩擦轮还是拨弹盘
 */
float Friction_Bullet_Pid_Control(BasePID_Object *base_pid,BasePID_Object *base_pid_speed, BasePID_Object *base_pid_angle, float Feedback, int16_t target, uint8_t Motorflag)
{
	if (Motorflag == FRICTION)
	{
		base_pid->Error = target - Feedback;

		base_pid->KpPart = base_pid->Error * base_pid->Kp;

		if (base_pid->Error < base_pid->KiPartDetachment && base_pid->Error > -base_pid->KiPartDetachment) // 积分作用
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
	if (Motorflag == BULLET_SINGLE) //角度
	{
			base_pid->Out = BasePID_AngleControl_BULEET(base_pid_angle,target,Feedback);

		return base_pid->Out;
	}
	
	return 0;
}


/**
 * @brief 遥控器控制打弹
 */
void DR16_Control_Shoot(RC_Ctrl_t *rc_ctrl, Loader_t *loader)
{
	if(rc_ctrl->rc.s2 == 1)	loader->FireFlag = rc_ctrl->rc.s1;
	else {
		loader->FireFlag = 0;
	}
//	if(loader->FireFlagLast == 0 && loader->FireFlag !=3)  
	//上：1		中：3	下：2
}

void Shoot_Mode_Change(RC_Ctrl_t *rc_ctrl, Loader_t *loader,VT13_t* vt13)
{
	
		DR16_Control_Shoot(rc_ctrl, loader);
		ChoosingFireRate(loader,&referee2024,1000);
	
		/* 遥控器控制 打弹*/
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
   /*   鼠标控制 打弹*/
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
 * @brief 期望发弹量控制
 */
uint32_t shoot_delay = 50 ;
int flag_vision = 1;
void Shoot_Fire_Mode_Control(RC_Ctrl_t *rc_ctrl, Loader_t *loader,Trace_t* info_vision,VT13_t* vt13)
{

		Shoot_Mode_Change(rc_ctrl,loader,vt13);
		
			/*  发弹控制  */
		if(IfOverHeat() == 1 && (rc_ctrl->isOnline == 1 || vt13->isOnline == 1))
		{	
			if( (info_vision->Hit_Mode == 3) && loader->Crowd_back_Flag == 0 ) //在一定频率下的连射 对频率进行实时更醒
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
 * @brief 拨弹盘控制
 */
int delay;
int finsh_shoot;
void BulletDisk_Shoot_control(Loader_t *loader, RC_Ctrl_t *rc_ctrl,Trace_t* info_vision)
{
	/*	拨弹盘期望角度计算	*/
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
	 /*		连发计算总角度与实际拨弹盘积分角度是否一致		*/
	if(loader->Crowd_back_Flag == 0 )
    {
				//< 误差小于5度且目标角度大于已转过的角度视为完成打弹
        if ((loader->BulletDisk.ALL_Angle - loader->BulletDisk.Angle_Plate) <= finsh_shoot && (loader->BulletDisk.ALL_Angle - loader->BulletDisk.Angle_Plate)>=0) 
        {
           loader->BulletDisk.TaretSpeed = 0;
        }
        else if((loader->BulletDisk.ALL_Angle - loader->BulletDisk.Angle_Plate)<-10)			//<转过头了
        {
						loader->BulletDisk.TaretSpeed = -3500;
        }    
        else if(loader->BulletDisk.ALL_Angle - loader->BulletDisk.Angle_Plate>10)  //转不够 会有堵转嫌疑
        {
					loader->BulletDisk.TaretSpeed = loader->Target_Firing_Rate;

					/*	堵转判断		*/
					if(loader->BulletDisk.motor2006.Data.SpeedRPM <50 && (loader->Shoot_Mode == continue_load || info_vision->Hit_Mode == 1 ) )
					{
						loader->Stuck_Delay++;
						if(loader->Stuck_Delay>delay)//150
						{
							loader->Crowd_back_Flag = 1;//1堵转
							loader->Stuck_Delay = 0;
						}
					}
					else
					{
							loader->Stuck_Delay = 0;
					}
        }
		}
    else if(loader->Crowd_back_Flag == 1) //<拨弹盘退弹
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
float delta_angle = 0; //<每帧数据的误差角
//int   Friction_Speed_Left =3000;// 7000;
//int   Friction_Speed_Right =3000;// 6990;

int Friction_Speed_Left =5700;// 7000;
int Friction_Speed_Right =5700;// 6990;
/**
 * @brief  摩擦轮控制+拨弹的闭环 好好调 减少发弹延时
 * @note   单发双环控制 无堵转判断 目前没问题打出即可 实现关摩擦看误差算进SENCE减少角度换算误差
 */
void Friction_BulletDisk_Fire_Control(Loader_t *loader, RC_Ctrl_t *rc_ctrl)
{

	if (loader->Friction_Status_Flag == 1 && tim14.ClockTime > 400 && (rc_ctrl->isOnline == 1 || VT13.isOnline == 1))
	{
     Friction_Stop_Cnt = 0;
     Friction_Start_Cnt++;
		/*射速处理*/				   // 始终以当前最大射速 除缓启动外
		if (Friction_Start_Cnt > 1000) // 1s缓启动
		{
			Friction_Start_Cnt = 1000;
		}
		
		loader->LeftFriction.TaretSpeed  =  Friction_Speed_Left * Friction_Start_Cnt / 1000;
		loader->RightFriction.TaretSpeed = -Friction_Speed_Right * Friction_Start_Cnt / 1000;
		
//		loader->Target_Firing_Rate = BULLET_DISK_SPEED;
		
		// 正常情况下转换为角度Load_Info.speed_raw * 2π，但是该电机为1/36减速电机，所以需要/36，
		// 并根据误差调整该值，比如：实测转一圈是348，因此此处需要*(360/348)，此处具体电机具体测试调整
        delta_angle = loader->BulletDisk.motor2006.Data.SpeedRPM * 0.001f * BULLET_DISK_SENCE;
		if (fabs(delta_angle) < 0.01f)
			delta_angle = 0;							// 防止跑飞
		loader->BulletDisk.Angle_Plate += delta_angle; // 拨弹盘一共转过的角度
		BulletDisk_Shoot_control(loader, rc_ctrl,&Vision_Info);
		
		/*摩擦轮缓启动*/
		

		loader->LeftFriction.motor3508.Data.Output = Friction_Bullet_Pid_Control(&loader->LeftFriction.PID, &loader->LeftFriction.PID, &loader->LeftFriction.PID,loader->LeftFriction.motor3508.Data.SpeedRPM, loader->LeftFriction.TaretSpeed, FRICTION);
		loader->RightFriction.motor3508.Data.Output = Friction_Bullet_Pid_Control(&loader->RightFriction.PID, &loader->RightFriction.PID,&loader->RightFriction.PID, loader->RightFriction.motor3508.Data.SpeedRPM, loader->RightFriction.TaretSpeed, FRICTION);
	
		loader->BulletDisk.motor2006.Data.Output = Friction_Bullet_Pid_Control(&loader->BulletDisk.PID[continue_speed],&loader->BulletDisk.PID[single_speed],&loader->BulletDisk.PID[single_angle], loader->BulletDisk.motor2006.Data.SpeedRPM, loader->BulletDisk.TaretSpeed, BULLET_CON);

//		if(loader->Shoot_Mode == continue_load)	loader->BulletDisk.motor2006.Data.Output = Friction_Bullet_Pid_Control(&loader->BulletDisk.PID[continue_speed],&loader->BulletDisk.PID[single_speed],&loader->BulletDisk.PID[single_angle], loader->BulletDisk.motor2006.Data.SpeedRPM, loader->BulletDisk.TaretSpeed, BULLET_CON);
//		if(loader->Shoot_Mode == single_load)		loader->BulletDisk.motor2006,Data.Output = Friction_Bullet_Pid_Control(

	}
	/*摩擦轮缓停+拨弹盘*/
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
