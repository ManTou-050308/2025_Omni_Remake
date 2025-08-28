#include "hardware_config.h"

#include "omnichassis.h"

#include "referee.h"
#include "check.h"
#include "Supercap.h"
#include "ins.h"
#include "brain.h"


#include "math.h"

#define AtR 0.0174532f	              //<  3.1415 /180 角度制 转化为弧度制	
#define All	0.7071268f

#define Slope_1    30
#define SlopedFeedforward 0		//重力上坡补偿前馈

#define torque_term_k1_3508 1.9e-07//1.9
#define speed_term_k2_3508  4.0e-07//4.0                          (20/16384)*(0.3）*（减速比）/9.55    
#define TORQUE_COEFFICIENT_3508   2.43243960986e-6f // (20/16384)*(0.3)*(17/268)/9.55，其中9.55约等于60/2pi
#define CONSTANT_COEFFICIENT_3508 0.71f

float chassistoholder = 0.0025;//0.006;	//云台转动前馈
float sup_power = 0;  //底盘超电功率
float all_output = 0;
float kP_CHASSIS = 70;
/**
  * @brief  全向轮底盘初始化
  */
void	OmniChassis_Control(Omni_Chassis_t* chassis, RC_Ctrl_t* rc_ctrl,Holder_t* holder,Super_Cap_t* super_cap,VT13_t* vt13)
{
	if(Vision_Info.Hit_Mode == 0) {
		OmniChassis.Motors3508.FollowPID.Kd = 0;
		OmniChassis.Motors3508.FollowPID.Kp = 20;}
	else if(OmniChassis.Movement.Vx !=0 && OmniChassis.Movement.Vy !=0)
	{
		OmniChassis.Motors3508.FollowPID.Kd = -5;
		OmniChassis.Motors3508.FollowPID.Kp = 30;	
	}
	else {
		OmniChassis.Motors3508.FollowPID.Kd = 0;
		OmniChassis.Motors3508.FollowPID.Kp = 60;
	}
	OmniChassis_R_State(chassis);
	OmniChassis_RemoteControl(chassis, holder->Motors6020.motor[Yaw_6020].Data.Angle,rc_ctrl,holder,vt13);
	OmniChassisMotionControl(chassis,rc_ctrl,super_cap,holder);


}

/**
  * @brief  全向轮底盘初始化
  */
void OmniChassisInit(Omni_Chassis_t* chassis, BasePID_Object pid_run, BasePID_Object pid_follow, CanNumber canx)
{
/*  左上  */
	MotorInit(&chassis->Motors3508.motor[0], 0 , Motor3508, canx, 0x201); //从左上开始逆时针1234
	MotorInit(&chassis->Motors3508.motor[1], 0 , Motor3508, canx, 0x202);
	MotorInit(&chassis->Motors3508.motor[2], 0 , Motor3508, canx, 0x203);
	MotorInit(&chassis->Motors3508.motor[3], 0 , Motor3508, canx, 0x204);
	
	for(int i=0;i<4;i++){
	BasePID_Init(&chassis->Motors3508.RunPID[i], pid_run.Kp, pid_run.Ki, pid_run.Kd, pid_run.KiPartDetachment);
		
	chassis->COM.Target[i] = 0;
	chassis->COM.step[i] = 2000;
	}
	
	
	BasePID_Init(&chassis->Motors3508.FollowPID, pid_follow.Kp, pid_follow.Ki, pid_follow.Kd, pid_follow.KiPartDetachment);
	
	chassis->Flag.ChassisFollow_Flag = 1;   
	chassis->Flag.ChassisFollow_Flag_Last = 3;//初始底盘模式
	chassis->Flag.RollingFlag = 1;          //小陀螺
	chassis->Power.SpeedPowerLimit =18000;  //总输出电流值限制
}


float test_powr =40;

/**
	*	@brief	底盘功率限制并放缩分配
	*/
int16_t cnt1,Flag_up,cnt2,Flag_left;
float a;
float test111;
static void OmniChassisPowerCtrl(Omni_Chassis_t* chassis,Super_Cap_t* super_cap)
{
	
	float CURENT_LIMT;

  chassis->Power.now_power = referee2024.power_heat_data_t.chassis_power; // 实时功率

	if(super_cap->cap_state.Supercap_Flag == 1 && super_cap->cap_state.SupercapIsOnline == 1)
	{
		if(sup_power < 150)
			sup_power += 0.015f;
		else sup_power = 150;
	}
	
	if(super_cap->cap_state.Supercap_Flag == 0)
	{
		sup_power = 0;
	}
	
	chassis->Power.max_power = referee2024.robot_status_t.chassis_power_limit  + sup_power + (referee2024.power_heat_data_t.buffer_energy - 20) * 5 ;       // 功率上限			

	if(chassis->Power.max_power < 0)
	chassis->Power.max_power = 0;
			
	if(referee2024.power_heat_data_t.buffer_energy > 40 && chassis->Flag.RollingFlag == 0 )//加速起步且在不开超电时保证功率利用
	{		
			if(super_cap->cap_state.Supercap_Flag == 1) chassis->Power.max_power += 70;
			else chassis->Power.max_power += 100;
	}
	if(test111 == 1 || Rc_Ctrl.keyflag[11] == 1 || VT13.isOnline*VT13.key_C_flag_vT13 ==1) chassis->Power.max_power = 65;

	chassis->Power.target_require_power_sum = 0;

	  for (int8_t i = 0; i < 4; i++) 
	{
        chassis->Motors3508.initial_give_power[i] = TORQUE_COEFFICIENT_3508 * chassis->Motors3508.motor[i].Data.Output * chassis->Motors3508.motor[i].Data.SpeedRPM +
                                                    speed_term_k2_3508 * chassis->Motors3508.motor[i].Data.SpeedRPM * chassis->Motors3508.motor[i].Data.SpeedRPM +
                                                    torque_term_k1_3508 * chassis->Motors3508.motor[i].Data.Output * chassis->Motors3508.motor[i].Data.Output +
																										CONSTANT_COEFFICIENT_3508;
        
			if (chassis->Motors3508.initial_give_power[i] < 0) // negative power not included (transitory)
            continue;
        chassis->Power.target_require_power_sum += chassis->Motors3508.initial_give_power[i];
  }

    chassis->Power.scaling_ratio = chassis->Power.max_power/ chassis->Power.target_require_power_sum;
    if(chassis->Power.scaling_ratio > 1)
			chassis->Power.scaling_ratio = 1;

    for (uint8_t i = 0; i < 4; i++) 
		{
			if (chassis->Power.scaling_ratio == 1) 
			{
				chassis->Motors3508.scaled_give_power[i] = chassis->Motors3508.initial_give_power[i];
				continue;		
			}
			else
				{
					chassis->Motors3508.scaled_give_power[i] = chassis->Motors3508.initial_give_power[i] * chassis->Power.scaling_ratio; // get scaled power

					if (chassis->Motors3508.scaled_give_power[i] < 0)
					{
						chassis->Motors3508.motor[i].Data.Output = 0;
							continue;
					}
					float b = TORQUE_COEFFICIENT_3508 * chassis->Motors3508.motor[i].Data.SpeedRPM;
					float c = speed_term_k2_3508 * chassis->Motors3508.motor[i].Data.SpeedRPM * chassis->Motors3508.motor[i].Data.SpeedRPM - chassis->Motors3508.scaled_give_power[i] + CONSTANT_COEFFICIENT_3508;

					if (chassis->Motors3508.motor[i].Data.Output > 0) // Selection of the calculation formula according to the direction of the original moment
					{
							double temp = (-b + sqrt(b * b - 4 * torque_term_k1_3508 * c)) / (2 * torque_term_k1_3508);
							if (temp > 16000) 
								{
									chassis->Motors3508.motor[i].Data.Output  = 16000;
								} 
								else
									chassis->Motors3508.motor[i].Data.Output  = temp;
					}
					else 
						{
							double temp = (-b - sqrt(b * b - 4 * torque_term_k1_3508 * c)) / (2 * torque_term_k1_3508);
							if (temp < -16000) 
								{
									chassis->Motors3508.motor[i].Data.Output  = -16000;
								} 
								else
									chassis->Motors3508.motor[i].Data.Output  = temp;
						}
				}
		}

            if (OmniChassis.Movement.now_Vx>0) cnt1++;else if (OmniChassis.Movement.now_Vx<0)cnt1--;else cnt1=0;
						if (cnt1>10) {Flag_up=1;cnt1=10;}
						if (cnt1<-10) {Flag_up=-1;cnt1=-10;}
						if (cnt1==0)Flag_up=0;
						
						if (OmniChassis.Movement.now_Vy>0) cnt2++;else if (OmniChassis.Movement.now_Vy<0)cnt2--;else cnt2=0;
						if (cnt2>10) {Flag_left=1;cnt2=10;}
						if (cnt2<-10) {Flag_left=-1;cnt2=-10;}
						if (cnt2==0)Flag_left=0;
						
						
				a=myABS(OmniChassis.Movement.now_Vx)/(myABS(OmniChassis.Movement.now_Vx)+myABS(OmniChassis.Movement.now_Vy));
				if(chassis->Flag.RollingFlag == 1 || (myABS(Holder.Motors6020.motor[Yaw_6020].Data.Angle)>1.5 && chassis->Flag.RollingFlagLast == 1)){
				chassis->Motors3508.motor[0].Data.Output = AmplitudeLimit(chassis->Motors3508.motor[0].Data.Output,8000 /*+ Flag_up*4000*a + Flag_left * 4000*(1-a)*/);	
				chassis->Motors3508.motor[1].Data.Output = AmplitudeLimit(chassis->Motors3508.motor[1].Data.Output,8000 /*+Flag_up*4000*a - Flag_left * 4000*(1-a)*/);	
				chassis->Motors3508.motor[2].Data.Output = AmplitudeLimit(chassis->Motors3508.motor[2].Data.Output,8000/*- Flag_up*4000*a - Flag_left * 4000*(1-a)*/);	
				chassis->Motors3508.motor[3].Data.Output = AmplitudeLimit(chassis->Motors3508.motor[3].Data.Output,8000/*- Flag_up*4000*a  + Flag_left * 4000*(1-a)*/);	
				}
				else{ 
					chassis->Motors3508.motor[0].Data.Output = AmplitudeLimit(chassis->Motors3508.motor[0].Data.Output,12000 + Flag_up*2000*a - Flag_left * 2000*(1-a));	
					chassis->Motors3508.motor[1].Data.Output = AmplitudeLimit(chassis->Motors3508.motor[1].Data.Output,12000 - Flag_up*2000*a - Flag_left * 2000*(1-a));	
					chassis->Motors3508.motor[2].Data.Output = AmplitudeLimit(chassis->Motors3508.motor[2].Data.Output,12000 - Flag_up*2000*a + Flag_left * 2000*(1-a));	
					chassis->Motors3508.motor[3].Data.Output = AmplitudeLimit(chassis->Motors3508.motor[3].Data.Output,12000 + Flag_up*2000*a + Flag_left * 2000*(1-a));	
			}
			  chassis->Movement.Last_V =chassis->Movement.now_V;
}

/**
 * @brief 遥控器控制底盘模式
 */
void DR16_Control_Chassis(RC_Ctrl_t *rc_ctrl,Omni_Chassis_t* chassis)
{
	if(rc_ctrl->rc.s2 == 3)	chassis->Flag.ChassisFollow_Flag = rc_ctrl->rc.s1;
	//上：1		中：3	下：2
}

/*模拟*/
int change = 3;					//调试模式切换 2正常 3 反转 1 变速 
int Roll_Stop_Cnt = 0;  		//匀速计时
int Roll_start_Cnt = 0;			//减速计时
int Back_cnt = 0;         	//反转计时
int Stop_bride = 4000;   		// 第一次匀速转动时间 ms
int Stop_bride11 = 2000;   	// 第一次匀速转动时间 ms 调试使用
int Start_bride = 500;			// 减速时间时间 ms
int Back_birde = 100;  			//反转时间
float Stop_k = 0.001;				//减速比例系数
float Start_k = 0.001;			//恢复匀速比例系数
float Down_limit1 = 2000;	 //最低小陀螺电流值
float Down_limit2 = 2000; 	//最低小陀螺平动电流值
int First_Flag = 0;
int Stop_bride_Second = 1200;//后续匀速转动时间 ms
int out_put = 4000;        //反转加速度

int Reload_cnt_1 =0; //换弹计数1
int Reload_cnt_2 = 0; //换弹计数2
int Reload_1 = 397;    //换弹1
int Reload_2 = 400;    //换弹2
int Reload_output = 5000; //换弹输出

int Reload_Flag;






int test_speed = 100;
int com_flag = 0;
/**
	*	@brief 		全向轮底盘遥控器解算目标速度
	*/
void	OmniChassis_RemoteControl(Omni_Chassis_t* chassis, float canAngle, RC_Ctrl_t* rc_ctrl, Holder_t* holder,VT13_t* vt13)
{
	/*		云台角度			*/
	float angle = canAngle * AtR;
	DR16_Control_Chassis(rc_ctrl,chassis);
	
	if(chassis->Flag.RollingFlag == 1) //小陀螺模式  
	{
		if(change == 2) /*测试*/
		{
			
				if((rc_ctrl->Chassis_X_Integ == 0 && (vt13->isOnline*vt13->Chassis_X_Integ) == 0) && (rc_ctrl->Chassis_Y_Integ == 0 && (vt13->isOnline*vt13->Chassis_Y_Integ) == 0))
					{
						chassis->Movement.RollingOmega = test_speed * 62.428;//test_speed（rpm） * 62.428 
						chassis->Movement.ModuleOfSpeed = 0;
					}
					else
					{	
						chassis->Movement.RollingOmega  = 20000*0.4 ; // 小陀螺期望速度电流占比init修改
						chassis->Movement.ModuleOfSpeed = 20000*0.6 ;
					}
		
		}		
		else if(change == 3) /*测试*/
		{
			if(rc_ctrl->key_R_flag == 1 || (vt13->isOnline*vt13->key_R_vT13) == 1 || Reload_Flag == 1 )
			{	
				Reload_Flag = 1;
				if( Reload_cnt_1 < Reload_1)
				{
					Reload_cnt_1++;
					Reload_cnt_2 = 0;
					chassis->Movement.RollingOmega  = Reload_output;
				}
				else if(Reload_cnt_2 < Reload_2)
				{
					Reload_cnt_1 =Reload_cnt_1;
					Reload_cnt_2++;
					chassis->Movement.RollingOmega  = -Reload_output;
				}
				else{
					Reload_cnt_1 = 0;
					Reload_cnt_2 = 0;
					Reload_Flag = 0;
				}
			
			
			}
			else if(rc_ctrl->key_ctrl ==1 || (vt13->isOnline*vt13->key_ctrl_flag_vT13) == 1 || vt13->isOnline*vt13->rc.Fn == 1 || dr16 == 1)//|| vt13->isOnline*vt13->rc.Fn == 1)
			{	
				
				if((rc_ctrl->Chassis_X_Integ != 0 || (vt13->isOnline*vt13->Chassis_X_Integ) != 0 ) || (rc_ctrl->Chassis_Y_Integ!= 0 || (vt13->isOnline*vt13->Chassis_Y_Integ) != 0))
				{
						chassis->Movement.RollingOmega  = 50 * 62.428; // 小陀螺期望速度电流占比init修改
						chassis->Movement.ModuleOfSpeed = 5000;
					  com_flag=0;
				}
				else{
					if(Roll_Stop_Cnt < Stop_bride )
					{
								com_flag = 1;
								chassis->Movement.RollingOmega = 75 * 62.428;
								chassis->Movement.ModuleOfSpeed = 0;
										
								Roll_Stop_Cnt++;
								Roll_start_Cnt = 0;
								Back_cnt = 0 ;
						}
					else
					{
						com_flag = 0;
						if( Back_cnt < Back_birde) 
						{
							Back_cnt++;
							chassis->Movement.RollingOmega  = 2000;
						}
						else	
						{
							chassis->Movement.RollingOmega  = -out_put ;
							Roll_start_Cnt ++;
							if(Roll_start_Cnt >Start_bride)	
							{
								Roll_Stop_Cnt = 0;
								Stop_bride = Stop_bride_Second;
							}
						}
					}
				}
			}
				
		}
		
	}
	else if((chassis->Flag.ChassisFollow_Flag == 1 && chassis->Flag.ChassisFollow_Flag_Last == 3) || vt13->isOnline == 1)	//底盘跟随模式
	{
		Roll_start_Cnt = 0;
		Roll_Stop_Cnt = 0;
		Stop_bride = Stop_bride11;
		
		chassis->Movement.RollingOmega  = BasePID_AngleControlFollow(&chassis->Motors3508.FollowPID, 0 ,holder->Motors6020.motor[Yaw_6020].Data.Angle,holder->Motors6020.motor[Yaw_6020].Data.SpeedRPM);
		chassis->Movement.ModuleOfSpeed = 10000;
		chassis->Flag.ChassisFollow_Flag_Last = 3;
	}
	else
	{
		Roll_start_Cnt = 0;
		Roll_Stop_Cnt = 0;
		Stop_bride = Stop_bride11;
		
		chassis->Movement.RollingOmega  = 0;
		chassis->Movement.ModuleOfSpeed = 20000 ; //chassis->Power.SpeedPowerLimit;
		chassis->Flag.ChassisFollow_Flag_Last = chassis->Flag.ChassisFollow_Flag;
	}


	
	/*		遥控器坐标系		*/
	VectorSolve(Rc_Ctrl.Chassis_X_Integ/3 + vt13->isOnline*vt13->Chassis_X_Integ/3,Rc_Ctrl.Chassis_Y_Integ/3 + vt13->isOnline*vt13->Chassis_Y_Integ/3,(&chassis->RcVectors.Rc_Ctrl_AngleOfSpeed),&chassis->RcVectors.Rc_Ctrl_ModuleOfSpeed);
	
	if(chassis->RcVectors.Rc_Ctrl_ModuleOfSpeed>=1)
	chassis->RcVectors.Rc_Ctrl_ModuleOfSpeed=1; 
	
	if(chassis->RcVectors.Rc_Ctrl_ModuleOfSpeed<=-1)
	chassis->RcVectors.Rc_Ctrl_ModuleOfSpeed=-1;
	
	/*		云台坐标系		*/
	holder->HoalderCirl.Vx = chassis->Movement.ModuleOfSpeed * cos(AtR * chassis->RcVectors.Rc_Ctrl_AngleOfSpeed) * chassis->RcVectors.Rc_Ctrl_ModuleOfSpeed;
	holder->HoalderCirl.Vy = chassis->Movement.ModuleOfSpeed * sin(AtR * chassis->RcVectors.Rc_Ctrl_AngleOfSpeed) * chassis->RcVectors.Rc_Ctrl_ModuleOfSpeed;
	
	/*		底盘坐标系		*/
	chassis->Movement.Vx = + holder->HoalderCirl.Vx * cos(angle + (holder->Motors6020.motor[Yaw_6020].Data.SpeedRPM * chassistoholder)) + holder->HoalderCirl.Vy * sin(angle + (holder->Motors6020.motor[Yaw_6020].Data.SpeedRPM * chassistoholder));
	chassis->Movement.Vy = - holder->HoalderCirl.Vx * sin(angle + (holder->Motors6020.motor[Yaw_6020].Data.SpeedRPM * chassistoholder)) + holder->HoalderCirl.Vy * cos(angle + (holder->Motors6020.motor[Yaw_6020].Data.SpeedRPM * chassistoholder));


	/*		3508电机		*/
	chassis->Motors3508.motor[0].Data.Target = - All * chassis->Movement.Vx + All * chassis->Movement.Vy + chassis->Movement.RollingOmega; //左下开始顺时针
	chassis->Motors3508.motor[1].Data.Target = - All * chassis->Movement.Vx - All * chassis->Movement.Vy + chassis->Movement.RollingOmega; 
	chassis->Motors3508.motor[2].Data.Target = + All * chassis->Movement.Vx - All * chassis->Movement.Vy + chassis->Movement.RollingOmega;
	chassis->Motors3508.motor[3].Data.Target = + All * chassis->Movement.Vx + All * chassis->Movement.Vy + chassis->Movement.RollingOmega;
	
	
	
	if(chassis ->Flag.RollingFlag == 1 && com_flag == 1)// && (Rc_Ctrl.Chassis_X_Integ != 0 || Rc_Ctrl.Chassis_Y_Integ != 0 ))
	{
		chassis->COM.Current[UP] = Ramp(chassis->COM.Current[UP],chassis->COM.Target[UP],chassis->COM.step[UP]);
		Offset_Com(UP,chassis,chassis->COM.Current[UP]);
		
		chassis->COM.Current[DOWN] = Ramp(chassis->COM.Current[DOWN],chassis->COM.Target[DOWN],chassis->COM.step[DOWN]);
		Offset_Com(DOWN,chassis,chassis->COM.Current[DOWN]);
		
		chassis->COM.Current[LEFT] = Ramp(chassis->COM.Current[LEFT],chassis->COM.Target[LEFT],chassis->COM.step[LEFT]);
		Offset_Com(LEFT,chassis,chassis->COM.Current[LEFT]);
		
		chassis->COM.Current[RIGHT] = Ramp(chassis->COM.Current[RIGHT],chassis->COM.Target[RIGHT],chassis->COM.step[RIGHT]);
		Offset_Com(RIGHT ,chassis,chassis->COM.Current[RIGHT]);
	}
	else{

		chassis->COM.Current[UP] = 0;
		chassis->COM.Current[DOWN] = 0;
		chassis->COM.Current[LEFT] = 0;
		chassis->COM.Current[RIGHT] = 0;
		Offset_Com(UP,chassis,0);
		Offset_Com(DOWN,chassis,0);		
		Offset_Com(LEFT,chassis,0);		
		Offset_Com(RIGHT ,chassis,0);
	
	}

}

/**
	*	@brief 电机Cirl并填入缓冲区
	*/
void OmniChassisMotionControl(Omni_Chassis_t* chassis,RC_Ctrl_t* rc_rtrl,Super_Cap_t* super_cap, Holder_t* holder)
{

	
					chassis->Motors3508.motor[0].Data.Output = Slope_Com(chassis,holder,INS_attitude) + 
																								 OmniDampedFeedforward(chassis->Motors3508.motor[0].Data.SpeedRPM ) +  
																								 BasePID_Speed_Omni_Control(&chassis->Motors3508.RunPID[0],chassis->Motors3508.motor[0].Data.Target ,
																								 chassis->Motors3508.motor[0].Data.SpeedRPM) /*1.0256410256410*/;
				
					chassis->Motors3508.motor[1].Data.Output = -Slope_Com(chassis,holder,INS_attitude) - 
																								 OmniDampedFeedforward(chassis->Motors3508.motor[1].Data.SpeedRPM ) +  
																								 BasePID_Speed_Omni_Control(&chassis->Motors3508.RunPID[1],chassis->Motors3508.motor[1].Data.Target ,
																								 chassis->Motors3508.motor[1].Data.SpeedRPM);
				
					chassis->Motors3508.motor[2].Data.Output = -Slope_Com(chassis,holder,INS_attitude) - 
																								 OmniDampedFeedforward(chassis->Motors3508.motor[2].Data.SpeedRPM ) +  
																								 BasePID_Speed_Omni_Control(&chassis->Motors3508.RunPID[2],chassis->Motors3508.motor[2].Data.Target ,
																								 chassis->Motors3508.motor[2].Data.SpeedRPM);
				
					chassis->Motors3508.motor[3].Data.Output = Slope_Com(chassis,holder,INS_attitude) + 
																								 OmniDampedFeedforward(chassis->Motors3508.motor[3].Data.SpeedRPM ) +  
																								 BasePID_Speed_Omni_Control(&chassis->Motors3508.RunPID[3],chassis->Motors3508.motor[3].Data.Target ,
																								 chassis->Motors3508.motor[3].Data.SpeedRPM);
				
	OmniChassisPowerCtrl(chassis,super_cap);

	for(int i = 0 ; i<4 ; i++){
			if(rc_rtrl->isOnline == 1|| VT13.isOnline == 1){
			MotorFillData(&chassis->Motors3508.motor[i],chassis->Motors3508.motor[i].Data.Output);
		}
		else{
			MotorFillData(&chassis->Motors3508.motor[i],0);
		}
	}
}


/**
  * @brief  输入向量在正交坐标系下的分量，求解向量的模和角度
  */

void VectorSolve(float vx, float vy, float* vectorAngle, float* vectorModule)
{
	float angle;
	float module;
	
	if (vx == 0) angle = 90;
	else
	{
		angle =  57.29f * atan(vy / vx);
	}
	module  =  sqrt(vx * vx + vy * vy);	
	
	if(vx < 0 && vy >= 0)//将三角函数系下的角度值转换为0-180°和0-(-180°)
		{
			angle = 180 + angle;
		}			
		else if(vx <= 0 && vy < 0 )
		{		
			angle = -180 + angle;
		}

	*vectorAngle  = angle; 
	*vectorModule = module;
}


/**
	*	@brief 坡度补偿前馈
	*   主动供力补偿
	*/
float Angle_Dev_Pitch;
int slope_cnt_start;
int slope_cnt_end;
float Slope_Com(Omni_Chassis_t* chassis,Holder_t* holder,Attitude_t* attitude)
{
	
	Angle_Dev_Pitch = (attitude->pitch - (-holder->Motors6020.motor[0].Data.Angle));

	if(Angle_Dev_Pitch > 6)  
	{
		slope_cnt_start ++; 
	}
	else{
		slope_cnt_start --;
	}
	
	if(slope_cnt_start>100)
	{
		slope_cnt_start = 100;
		chassis->Flag.SlopeFlag = 1;
	}
	else	if(slope_cnt_start <0) 
	{
		slope_cnt_start = 0;
		chassis->Flag.SlopeFlag = 0;
	}
	
	
	if(chassis->Flag.SlopeFlag == 1 && Vision_Info.Hit_Mode !=1 )
	{	
		
	  return 2000;
	}
	else{
		return 0;
	}
	
}

/**
	*	@brief 阻尼前馈
	*/
float OmniDampedFeedforward(float speed)
{
	float DampedFeedforward;
	
	DampedFeedforward = 0;    //实地测不同机体速度下的电流反馈值 在拟合二次函数 模糊前馈
	
	return DampedFeedforward;

}

/**
	*	@brief	偏移补偿
	*/

void Offset_Com(Offest direction,Omni_Chassis_t* chassis,int Com){
	
	switch(direction){
		case UP:
	chassis->Motors3508.motor[0].Data.Target += Com;
	chassis->Motors3508.motor[1].Data.Target -= Com;
	chassis->Motors3508.motor[2].Data.Target -= Com;
	chassis->Motors3508.motor[3].Data.Target += Com;
			break;
		case DOWN:
	chassis->Motors3508.motor[0].Data.Target -= Com;
	chassis->Motors3508.motor[1].Data.Target += Com;
	chassis->Motors3508.motor[2].Data.Target += Com;
	chassis->Motors3508.motor[3].Data.Target -= Com;
			break;	
		case LEFT:
	chassis->Motors3508.motor[0].Data.Target += Com;
	chassis->Motors3508.motor[1].Data.Target += Com;
	chassis->Motors3508.motor[2].Data.Target -= Com;
	chassis->Motors3508.motor[3].Data.Target -= Com;	
			break;
		case RIGHT:
	chassis->Motors3508.motor[0].Data.Target -= Com;
	chassis->Motors3508.motor[1].Data.Target -= Com;
	chassis->Motors3508.motor[2].Data.Target += Com;
	chassis->Motors3508.motor[3].Data.Target += Com;		
			break;		
	}

}

/**
	* @brief 斜坡目标值函数
	*/

int Ramp(int current, int target, int step) {
    if (current < target) {
        current += step;
        if (current > target) {
            current = target;
        }
    }
		return current;
}

/**
	*	@brief	底盘功率限制并更新总速度  模糊控制
	*/
void 	OmniChassisSpeed_Power_Control(Omni_Chassis_t* chassis)
{
	/*		裁判系统瞬时超功率边界		*/
	float Target_Power;
	Target_Power = referee2024.robot_status_t.chassis_power_limit;
	
	/*		更新功率限制变化的总速度变化		*/
	if(chassis->Power.PowerLimitLast	!= Target_Power )
	{
		chassis->Power.PowerLimitLast  = Target_Power;
		chassis->Power.SpeedPowerLimit = 300; //OmniPowertoSpeed(Target_Power);
	}
	
	/*		超电更新功率		*/
	if(chassis->Power.SpeedUp == 1 && chassis->Power.SpeedUpLast == 0 && Super_Cap.cap_state.SupercapIsOnline == 1)
	{
		//不同的当前功率应修改成不同的速度控制量
		if(Target_Power >= 80)
		{
			chassis->Power.SpeedPowerLimit = OmniPowertoSpeed(160);
		}
		else
		{
			//防止速度突变太大导致超功率扣血
			chassis->Power.SpeedPowerLimit = OmniPowertoSpeed(Target_Power + 80);
		}
		
		chassis->Power.SpeedUpLast = chassis->Power.SpeedUp;
	}
	else if(chassis->Power.SpeedUp != chassis->Power.SpeedUpLast)
	{
		//关超电后，速度返回原有的量
		chassis->Power.SpeedPowerLimit = OmniPowertoSpeed(Target_Power);
		chassis->Power.SpeedUpLast = chassis->Power.SpeedUp;
	}

}

/**
	*	@brief 功率速度匹配拟合函数
	*/
float OmniPowertoSpeed(float target_power)
{
	float TotalSpeed;
	
	if(target_power<35) target_power = 45;
	if(target_power>250) target_power = 250;
	
	TotalSpeed = target_power*100;
	
	return TotalSpeed;

}


void OmniChassis_R_State(Omni_Chassis_t* chassis)
{
		float V0,V1,V2,V3;
		
	 //-2 3
   //-1 0
	
	V0 = chassis->Motors3508.motor[0].Data.SpeedRPM / sqrt(2);
	V1 = chassis->Motors3508.motor[1].Data.SpeedRPM / sqrt(2);
	V2 = chassis->Motors3508.motor[2].Data.SpeedRPM / sqrt(2);
	V3 = chassis->Motors3508.motor[3].Data.SpeedRPM / sqrt(2);

	chassis->Movement.now_Vx = (-V0 - V1 + V2 + V3) / 4;
	chassis->Movement.now_Vy = ( V0 - V1 - V2 + V3) / 4;
	chassis->Movement.now_V = sqrt(pow(chassis->Movement.now_Vy,2) + pow(chassis->Movement.now_Vx,2));
}

///* 逆时针正*/
//void OmniChassis_R_State(Omni_Chassis_t* chassis)
//{
//		float LU,LD,RU,RD,rvx,rvy;
//		
//		LU = chassis->Motors3508.motor[0].Data.SpeedRPM;
//		LD = chassis->Motors3508.motor[1].Data.SpeedRPM;
//		RU = chassis->Motors3508.motor[2].Data.SpeedRPM;
//		RD = chassis->Motors3508.motor[3].Data.SpeedRPM;
//		
//		rvx = - LU - RU + LD + RD ; 
//		rvy = - LU + RU + RD - LD ;
//		
//}

