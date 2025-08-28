//#include "hardware_config.h"
#include "control_logic.h"

#include "holder.h"
#include "ins.h"
#include "ladrc.h"
#include "omnichassis.h"

#include "pid.h"
#include "SMC.h"
#include "filter.h"
#include <math.h>
#include "brain.h"
#include "dr16.h"
#include "vt13.h"

#include "driver_timer.h"


#define myABS(x) ( (x)>0?(x):-(x) )
#define Behind(x)(0.5825 * pow(x, 3) + 3.5403 * pow(x, 2) + 217.4 * x + 13728) 
uint16_t Reset_Count = 0;
float up_limit;
float down_limit;


/**
	*	@brief 
	*/
void HodlerInit(Holder_t* holder,CanNumber canx,BasePID_Object pitch_reset_pid,BasePID_Object pitch_angle_pid,BasePID_Object pitch_speed_pid,BasePID_Object pitch_angle_vision_pid,BasePID_Object pitch_speed_vision_pid ,BasePID_Object yaw_reset_pid ,BasePID_Object yaw_angle_pid,BasePID_Object yaw_speed_pid,BasePID_Object yaw_angle_vision_pid,BasePID_Object yaw_speed_vision_pid,BasePID_Object yaw_speed_vision_single_pid,BasePID_Object yaw_angle_vision_single_pid,BasePID_Object pitch_speed_vision_single_pid,BasePID_Object pitch_angle_vision_single_pid)
{
	MotorInit(&holder->Motors6020.motor[Pitch_6020],1104,Motor6020,canx,0x206);//1125//6645,Motor6020,canx,0x205
	MotorInit(&holder->Motors6020.motor[Yaw_6020],2081,Motor6020,CAN2,0x205);//头朝电管4239//头朝电池6318
	
	BasePID_Init(&holder->Motors6020.turnPID[0],pitch_reset_pid.Kp,pitch_reset_pid.Ki,pitch_reset_pid.Kd,pitch_reset_pid.KiPartDetachment);
	BasePID_Init(&holder->Motors6020.turnPID[1],yaw_reset_pid.Kp,yaw_reset_pid.Ki,yaw_reset_pid.Kd,yaw_reset_pid.KiPartDetachment);

	BasePID_Init(&holder->Motors6020.turnPID[2],pitch_angle_pid.Kp,pitch_angle_pid.Ki,pitch_angle_pid.Kd,pitch_angle_pid.KiPartDetachment);
	BasePID_Init(&holder->Motors6020.turnPID[3],pitch_speed_pid.Kp,pitch_speed_pid.Ki,pitch_speed_pid.Kd,pitch_speed_pid.KiPartDetachment);
	BasePID_Init(&holder->Motors6020.turnPID[4],yaw_angle_pid.Kp,yaw_angle_pid.Ki,yaw_angle_pid.Kd,yaw_angle_pid.KiPartDetachment);
	BasePID_Init(&holder->Motors6020.turnPID[5],yaw_speed_pid.Kp,yaw_speed_pid.Ki,yaw_speed_pid.Kd,yaw_speed_pid.KiPartDetachment);

	BasePID_Init(&holder->Motors6020.turnPID[6],pitch_angle_vision_pid.Kp,pitch_angle_vision_pid.Ki,pitch_angle_vision_pid.Kd,pitch_angle_vision_pid.KiPartDetachment);
	BasePID_Init(&holder->Motors6020.turnPID[7],pitch_speed_vision_pid.Kp,pitch_speed_vision_pid.Ki,pitch_speed_vision_pid.Kd,pitch_speed_vision_pid.KiPartDetachment);
	BasePID_Init(&holder->Motors6020.turnPID[8],yaw_angle_vision_pid.Kp,yaw_angle_vision_pid.Ki,yaw_angle_vision_pid.Kd,yaw_angle_vision_pid.KiPartDetachment);
	BasePID_Init(&holder->Motors6020.turnPID[9],yaw_speed_vision_pid.Kp,yaw_speed_vision_pid.Ki,yaw_speed_vision_pid.Kd,yaw_speed_vision_pid.KiPartDetachment);

	BasePID_Init(&holder->Motors6020.visionPID[0],yaw_speed_vision_single_pid.Kp,yaw_speed_vision_single_pid.Ki,yaw_speed_vision_single_pid.Kd,yaw_speed_vision_single_pid.KiPartDetachment);
	BasePID_Init(&holder->Motors6020.visionPID[1],yaw_angle_vision_single_pid.Kp,yaw_angle_vision_single_pid.Ki,yaw_angle_vision_single_pid.Kd,yaw_angle_vision_single_pid.KiPartDetachment);
	BasePID_Init(&holder->Motors6020.visionPID[2],pitch_speed_vision_single_pid.Kp,pitch_speed_vision_single_pid.Ki,pitch_speed_vision_single_pid.Kd,pitch_speed_vision_single_pid.KiPartDetachment);
	BasePID_Init(&holder->Motors6020.visionPID[3],pitch_angle_vision_single_pid.Kp,pitch_angle_vision_single_pid.Ki,pitch_angle_vision_single_pid.Kd,pitch_angle_vision_single_pid.KiPartDetachment);

	holder->Pitch.Sensitivity = 0.00028;
	holder->Yaw.Sensitivity = 0.0005;//0.0005;
	Holder.Reset_OK_Flag  =0;
	holder->Pitch.MouseSensitivity = 0.0015;
	holder->Yaw.MouseSensitivity = 0.01;//0.005;
}

float k_holder = -0.008;//-0.00323244999
/**
	*	@brief 复位判断函数
	*/
void HolderReset(Holder_t* holder)
{
	if(tim14.HolderTime < 200 ){
		if (myABS(holder->Motors6020.motor[Pitch_6020].Data.Angle) >= 5.f && myABS(holder->Motors6020.motor[Yaw_6020].Data.Angle) >= 2.f)
		{
			tim14.HolderTime = 11;
			Holder.Reset_OK_Flag = 0;//0
		}
	}
	else{
			Holder.Reset_OK_Flag =1;
	}
}

float Td_Pitch =90;
float Td_Yaw=160;
float speed; //yaw实际转速
 float forward = 5000;
/**
	*	@brief 云台控制
	*/
void HolderGetRemoteData(Holder_t* holder, RC_Ctrl_t* rc_ctrl,VT13_t* vt13)
{

	speed = (OmniChassis.Motors3508.motor[0].Data.SpeedRPM + OmniChassis.Motors3508.motor[1].Data.SpeedRPM + OmniChassis.Motors3508.motor[2].Data.SpeedRPM + OmniChassis.Motors3508.motor[3].Data.SpeedRPM)/160.0f;
	if(speed < 10) speed  = holder->Motors6020.motor[Yaw_6020].Data.SpeedRPM;
	/*		遥控器目标角度		*/
	if(Holder.Reset_OK_Flag == 0)
	{
		holder->Pitch.Target_Angle = 0;
		holder->Yaw.Target_Angle   = holder->Gryo_Yaw_Zero_Angle;
	}
	else{
		holder->Pitch.Target_Angle += ((rc_ctrl->rc.ch3 - 1024)*holder->Pitch.Sensitivity +  rc_ctrl->mouse.y*holder->Pitch.MouseSensitivity + vt13->isOnline*(vt13->rc.ch2 - 1024)*holder->Pitch.Sensitivity + vt13->isOnline*vt13->mouse.y*holder->Pitch.MouseSensitivity );
		holder->Yaw.Target_Angle   -= ((rc_ctrl->rc.ch2 - 1024)*holder->Yaw.Sensitivity   +  rc_ctrl->mouse.x*holder->Yaw.MouseSensitivity + vt13->isOnline*(vt13->rc.ch3 - 1024)*holder->Pitch.Sensitivity + vt13->isOnline*vt13->mouse.x*holder->Pitch.MouseSensitivity)	;
	}
	
	/*		软件限位pitch 加安装误差修正		*/
	Change_limit(Angle_Dev_Pitch,holder,rc_ctrl);
	
	if(Vision_Info.Hit_Mode == 3)		//手瞄
	{
		holder->Motors6020.motor[Pitch_6020].Data.Output = BasePID_PitchSpeedControl((BasePID_Object*)(holder->Motors6020.turnPID + 3),BasePID_PitchAngleControl((BasePID_Object*)(holder->Motors6020.turnPID + 2),(holder->Pitch.Target_Angle),INS_attitude->pitch),3*INS_attitude->gyro[0]);  
		holder->Motors6020.motor[Yaw_6020].Data.Output   = BasePID_YawSpeedControl((BasePID_Object*)(holder->Motors6020.turnPID + 5),BasePID_YawAngleControl((BasePID_Object*)(holder->Motors6020.turnPID + 4),(holder ->Yaw.Target_Angle),(INS_attitude->yaw_total_angle )), 2 * INS_attitude->gyro[2]);//+ BasePID_YawSpeedControl((BasePID_Object*)(&holder->Motors6020.Feedback_Speed),speed, holder->Motors6020.motor[Yaw_6020].Data.SpeedRPM);
	//	holder->Motors6020.motor[Yaw_6020].Data.Output = SMC(holder ->Yaw.Target_Angle,INS_attitude->yaw_total_angle, 2 * INS_attitude->gyro[2]/*holder->Motors6020.motor[Yaw_6020].Data.SpeedRPM * 6*/ ,&smc_yaw);
	}
	else														
	{
		Holder_TD(&holder->Pitch,holder->Pitch.Target_Angle,Td_Pitch,0.001);
		Holder_TD(&holder->Yaw,holder->Yaw.Target_Angle ,Td_Yaw,0.001);
		if(Vision_Info.Hit_Mode==0){
//			if(Vision_Info.Core_Info.IsFire == 0) holder->Motors6020.turnPID[8].Ki  = 0;
//			else holder->Motors6020.turnPID[8].Ki  = 0.8;
			//自瞄
			holder->Motors6020.motor[Pitch_6020].Data.Output = BasePID_PitchSpeedControl((BasePID_Object*)(holder->Motors6020.turnPID + 7),BasePID_PitchAngleControl((BasePID_Object*)(holder->Motors6020.turnPID + 6),(holder->Pitch.v1),INS_attitude->pitch),2*INS_attitude->gyro[0]);		
			holder->Motors6020.motor[Yaw_6020].Data.Output   = BasePID_YawSpeedControl((BasePID_Object*)(holder->Motors6020.turnPID + 9),BasePID_Yaw_VISION_AngleControl((BasePID_Object*)(holder->Motors6020.turnPID + 8),(holder->Yaw.v1),(INS_attitude->yaw_total_angle )),2*INS_attitude->gyro[2]);			
			}
		else
		{
			if(Vision_Info.Core_Info.Distance > 6000 ){
				holder ->Motors6020.visionPID[0].Kp = 6000;
				holder ->Motors6020.visionPID[1].Kp = 1.5;
				holder ->Motors6020.visionPID[1].Ki = 0.07;
				holder ->Motors6020.visionPID[1].Kd = -20;
				holder ->Motors6020.visionPID[3].Ki = 0.007;
			}
			else{
				holder ->Motors6020.visionPID[0].Kp = 7000;
				holder ->Motors6020.visionPID[1].Kp = 1.5;
				holder ->Motors6020.visionPID[1].Ki = 0.005;
				holder ->Motors6020.visionPID[1].Kd = -20;
				holder ->Motors6020.visionPID[3].Ki = 0.05;
				
			}
			holder->Motors6020.motor[Pitch_6020].Data.Output = BasePID_PitchSpeedControl((BasePID_Object*)(holder->Motors6020.visionPID + 2),BasePID_PitchAngleControl((BasePID_Object*)(holder->Motors6020.visionPID + 3),(holder->Pitch.v1),INS_attitude->pitch),2*INS_attitude->gyro[0]);		
			holder->Motors6020.motor[Yaw_6020].Data.Output   = BasePID_YawSpeedControl((BasePID_Object*)(holder->Motors6020.visionPID + 0),BasePID_Yaw_VISION_AngleControl((BasePID_Object*)(holder->Motors6020.visionPID + 1),(holder->Yaw.v1),(INS_attitude->yaw_total_angle )),2*INS_attitude->gyro[2]);	
		}
	}
	
	/* 前馈补偿转动力矩 */
	if(OmniChassis.Flag.RollingFlag == 1) 	holder->Motors6020.motor[Yaw_6020].Data.Output -= 120*speed;

	if(holder->Motors6020.motor[Yaw_6020].Data.Output>=25000)  holder->Motors6020.motor[Yaw_6020].Data.Output = 25000;
	if(holder->Motors6020.motor[Yaw_6020].Data.Output<=-25000)  holder->Motors6020.motor[Yaw_6020].Data.Output = -25000;
	if(holder->Motors6020.motor[Pitch_6020].Data.Output>=25000)  holder->Motors6020.motor[Pitch_6020].Data.Output = 25000;
	if(holder->Motors6020.motor[Pitch_6020].Data.Output<=-25000)  holder->Motors6020.motor[Pitch_6020].Data.Output = -25000;

	MotorFillData(&holder->Motors6020.motor[Pitch_6020], - holder->Motors6020.motor[Pitch_6020].Data.Output);
	MotorFillData(&holder->Motors6020.motor[Yaw_6020], - holder->Motors6020.motor[Yaw_6020].Data.Output);

		
}


/**
	*	@brief 零点初始化
	*/
void 	Zero_Init(Holder_t* holder)
{
	HAL_Delay(10);
	holder->Gryo_Yaw_Zero_Angle		=		holder->Motors6020.motor[Yaw_6020].Data.Angle;
	holder->Gryo_Pitch_Zero_Angle =  holder->Motors6020.motor[Pitch_6020].Data.Angle;
}

/**
	*	@brief TD微分器
	*/
float Holder_TD(struct Holder_Motor_Info* holder_info,float Expect,float r,float h)
{
    double fh= -r * r * (holder_info->v1 - Expect) - 2 * r * holder_info->v2;
    holder_info->v1 += holder_info->v2 * h;
    holder_info->v2 += fh * h;
		return holder_info->v1;
}

/**
	*	@brief 上下限
	*/
float Dog_angle=-35;
void Change_limit(float dev,Holder_t* holder,RC_Ctrl_t* rc_Ctrl)
{
	uint32_t Down_cnt;
	uint32_t Up_cnt;
	//下坡前置
	if(rc_Ctrl->keyflag[11] == 1 || VT13.isOnline*VT13.key_C_flag_vT13 ==1 )holder->Pitch.Target_Angle = -12;
	if(Vision_Info.Hit_Mode == 1 )
	{
		up_limit = 45;
		down_limit =  -14.5f;
		Down_cnt=0;
		Up_cnt=0;
		
	}
	else{
		//坡度检测 + 限制改变
		if(	dev < -8){
			up_limit = 15;
			down_limit =-50;
			Up_cnt=0;
			Down_cnt++;
			if(rc_Ctrl->keyflag[11] == 1 || VT13.isOnline*VT13.key_C_flag_vT13 ==1) holder->Pitch.Target_Angle = Dog_angle;		
		}
		else if(dev >8){
			up_limit +=10 ;
			down_limit =-14.5;
			Down_cnt=0;
			Up_cnt++;
		}
		else{
		up_limit = 25;
		down_limit =  -14.5f;
		Down_cnt=0;
		Up_cnt=0;
		}
	}
	
	//目标限幅
	if(holder->Pitch.Target_Angle >= up_limit)
		holder->Pitch.Target_Angle = up_limit;
	else if(holder->Pitch.Target_Angle <= down_limit)
		holder->Pitch.Target_Angle = down_limit;

}