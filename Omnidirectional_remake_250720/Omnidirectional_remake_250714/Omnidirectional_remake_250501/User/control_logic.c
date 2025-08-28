#include "control_logic.h"
#include "hardware_config.h"

#include "holder.h"
#include "omnichassis.h"
#include "load.h"
#include "ladrc.h"
#include "interaction.h"


#include "check.h"
#include "motor.h"
#include "dr16.h"
#include "vt13.h"
#include "Supercap.h"
#include "bmi088.h"
#include "mpu6050.h"
#include "brain.h"
#include "usart.h"
#include "ui.h"
#include "referee.h"
#include "heat_control.h"

#include "driver_timer.h"
int INS_FPS,ui,cntppp,flag_ppp;
int cnt = 0;
float de_pitch,de_yaw;
void TIM13_Task()
{
	tim13.ClockTime++;
	INS_attitude = INS_GetAttitude(IMU_data);
	

	
}

void TIM14_Task()
{
	tim14.UI_Time++;
	tim14.ClockTime++;
	tim14.HolderTime++;
	if(tim14.HolderTime >15000)  tim14.HolderTime = 15000; 
		
	if(Rc_Ctrl.RestFlag==1 || (VT13.isOnline * VT13.RestFlag) == 1) //QV长按重置
	{
		__set_FAULTMASK(1); 				//关闭所有中断 
		NVIC_SystemReset(); 				//复位
	}


	if(tim14.ClockTime %1000==0)
	{
		Motor_CheckFPS();
		FPS_Check(&tim14_FPS);
	}

	RobotOnlineState(&check_robot_state, &referee2024,&Rc_Ctrl,&Super_Cap,&VT13);

	RobotToBrain();
	
	UI_open();
	
	HolderReset(&Holder);
	
	Load_Task();
	
	if(Rc_Ctrl.isOnline == 1 )
	{
			HolderGetRemoteData(&Holder, &Rc_Ctrl,&VT13);
			OmniChassis_Control(&OmniChassis ,&Rc_Ctrl,&Holder,&Super_Cap,&VT13);
	}
	
	if(Rc_Ctrl.isOnline == 0 ) 	
	{
		DR16Init(&Rc_Ctrl);
		if(VT13.isOnline ==1)
		{
			HolderGetRemoteData(&Holder, &Rc_Ctrl,&VT13);
			OmniChassis_Control(&OmniChassis ,&Rc_Ctrl,&Holder,&Super_Cap,&VT13);
		}
		else{
			tim14.UI_Time = 0;
			VT13Init(&VT13);
			
			MotorFillData(&Holder.Motors6020.motor[Yaw_6020],0);
			MotorFillData(&Holder.Motors6020.motor[Pitch_6020],0);
			for(int i = 0;i < 4;i++)
			{
				MotorFillData(&OmniChassis.Motors3508.motor[i],0);
			}
		}
	}
	
	if ((OmniChassis.Motors3508.motor[0].Data.Online_check.FPS<100||OmniChassis.Motors3508.motor[1].Data.Online_check.FPS<100||OmniChassis.Motors3508.motor[2].Data.Online_check.FPS<100||OmniChassis.Motors3508.motor[3].Data.Online_check.FPS<100) &&flag_ppp==0 )
	{
		for (int i=0;i<4;i++){
			MotorFillData(&OmniChassis.Motors3508.motor[i],0);}
		if (referee2024.robot_status_t.power_management_chassis_output==1)	cntppp++;
			else cntppp=0;
	}else cntppp=0;
	
	if (cntppp>10000){flag_ppp=1;cntppp=0;}
	
	if(tim14.HolderTime < 200)
	{
		MotorCanOutput(can2, 0x2ff);  //yaw
		MotorCanOutput(can1, 0x1ff);	// pitch
	}
	else{
		Holder.Reset_OK_Flag  =1;
		
		if(tim14.ClockTime % 4 == 3 ){
			MotorCanOutput(can1, 0x200);	//摩擦轮
		}
		if(tim14.ClockTime % 4 ==1 ){
			MotorCanOutput(can2, 0x1ff);  //拨弹盘
		}
		MotorCanOutput(can1, 0x1ff);  //pitch
		MotorCanOutput(can2, 0x1ff);  //yaw

		MotorCanOutput(can2, 0x200);	//底盘

	}
	

	SupercapControl(can2,&Rc_Ctrl,&Super_Cap,&OmniChassis);
	de_pitch = Holder.Pitch.Target_Angle - INS_attitude->pitch;
	de_yaw   = Holder.Yaw.Target_Angle   - INS_attitude->yaw_total_angle;
	
//	UsartDmaPrintf("%f,%d,%f,%d,%d\n",Super_Cap.cap_state.Voltage,
//																		referee2024.power_heat_data_t.buffer_energy,
//																		OmniChassis.Power.max_power,
//																		Super_Cap.cap_state.Supercap_Mode,
//																		Super_Cap.cap_state.Supercap_Flag);

	UsartDmaPrintf("%f,%d,%d,%f\n",Super_Cap.cap_state.Voltage,
																 referee2024.power_heat_data_t.buffer_energy,
																 Super_Cap.cap_state.Supercap_Flag,
																 OmniChassis.Power.max_power);
////////	
//	UsartDmaPrintf("%d,%d,%d,%d,%d,%d,%d,%d,%f\n",OmniChassis.Motors3508.motor[0].Data.Output,
//																							OmniChassis.Motors3508.motor[1].Data.Output,
//																							OmniChassis.Motors3508.motor[2].Data.Output,
//																							OmniChassis.Motors3508.motor[3].Data.Output,
//																							OmniChassis.Motors3508.motor[0].Data.SpeedRPM,
//																							OmniChassis.Motors3508.motor[1].Data.SpeedRPM,
//																							OmniChassis.Motors3508.motor[2].Data.SpeedRPM,
//																							OmniChassis.Motors3508.motor[3].Data.SpeedRPM,
//																							Holder.Motors6020.motor[Yaw_6020].Data.Angle);

//	UsartDmaPrintf("%d,%d,%d,%d\n",OmniChassis.Motors3508.motor[0].Data.Online_check.FPS,OmniChassis.Motors3508.motor[1].Data.Online_check.FPS,OmniChassis.Motors3508.motor[2].Data.Online_check.FPS,OmniChassis.Motors3508.motor[3].Data.Online_check.FPS );


//	UsartDmaPrintf("%f,%f,%f,%f,%f,%f,%f,%d,%f,%f,%d,%d\n",Holder.Yaw.Target_Angle,
//																												INS_attitude->yaw_total_angle,
//																												Holder.Yaw.v1,
//																												Holder.Motors6020.motor[Yaw_6020].Data.Angle,
//																												Holder.Pitch.Target_Angle,
//																												INS_attitude->pitch,
//																												Holder.Pitch.v1,
//																												Vision_Info.Flag_Vision_Del_Small,
//																												de_pitch,
//																												de_yaw,
//																												Holder.Motors6020.motor[Yaw_6020].Data.Output,
//																												Holder.Motors6020.motor[Pitch_6020].Data.Output);
//																												
//UsartDmaPrintf("%f,%f,%f,%d,%d,%f\n",Holder.Yaw.Target_Angle,
//																	INS_attitude->yaw_total_angle,
//																	Holder.Yaw.v1,
//																	Vision_Info.Flag_Vision_Del_Small,
//																	Holder.Motors6020.motor[Yaw_6020].Data.Output,
//																	offangle);

//UsartDmaPrintf("%d,%d,%d,\n",Vision_Info.Flag_Vision_Del_Small,
//																	Loader.ContinuousShoot_Target_Sum,
//																	Loader.BulletDisk.TaretSpeed,
//																	);
//	UsartDmaPrintf("%f,%f,%f,%d,%d\n",Holder.Pitch.Target_、Angle,
//																												INS_attitude->pitch,
//																												Holder.Pitch.v1,
//																												Vision_Info.Flag_Vision_Del_Small,
//																												Holder.Motors6020.motor[Pitch_6020].Data.Output);

//	UsartDmaPrintf("%f,%f,%f,%d,%d,%d\n",Holder.Yaw.Target_Angle,
//																INS_attitude->yaw_total_angle,
//																Holder.Yaw.v1,
//																Holder.Motors6020.motor[Yaw_6020].Data.Output,
//																Vision_Info.Flag_Vision_Del_Small,
//																Vision_Info.Hit_Mode);

//UsartDmaPrintf("%f,%f,%f,%f,%f,%f,%d,%f,%d\n",Holder.Yaw.Target_Angle,
//																							INS_attitude->yaw_total_angle,
//																							Holder.Yaw.v1,	
//																							Holder.Pitch.Target_Angle,
//																							INS_attitude->pitch,
//																							Holder.Pitch.v1,
//																							Vision_Info.Flag_Vision_Del_Small,
//																							Brain_pitch_raw_add,
//																							Holder.Motors6020.motor[Yaw_6020].Data.Output);

//	UsartDmaPrintf("%d,%f,%f\n",Loader.One_Shoot_Sum,
//												Loader.BulletDisk.Angle_Plate,
//												Loader.BulletDisk.motor2006.Data.Total_Target);

	Energy_Cal();

}


uint8_t CAN2_rxCallBack(CAN_RxBuffer* rxBuffer)
{
		MotorRxCallback(can2, (*rxBuffer)); 
		Supercap_rxCallBack(can2,(*rxBuffer),&Super_Cap);
		return 0;
}
	uint8_t CAN1_rxCallBack(CAN_RxBuffer* rxBuffer)
{
		MotorRxCallback(can1, (*rxBuffer)); 

		return 0;
}
