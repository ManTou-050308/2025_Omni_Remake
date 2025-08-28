#include "hardware_config.h"
#include "control_logic.h"

#include "ins.h"

#include "brain.h"
#include "check.h"
#include "filter.h"
#include "mpu6050.h"
#include "bmi088.h"

#include "holder.h"
#include "load.h"
#include "omnichassis.h"

#include "string.h"
#include "driver_timer.h"


#define myABS(x) ( (x)>0?(x):-(x) )

uint8_t RobotToBrainTimeBuffer[23];
float RobotToBrainPitchAngle[256];
float RobotToBrainYawAngle[256];

float  PitchVisionSence = 1;  
float  YawVisionSence = 1;  

float Brain_yaw_raw_add = 0;			
float Brain_pitch_raw_add = 0;    

int Symbol_delay;

uint8_t ModeToBrain = 0;				
uint8_t ShootModeToBrain = 0;			

uint8_t Brain_recData[Brain_rxBufferLengh];
UART_RxBuffer uart6_buffer={
	.Data = Brain_recData,
	.Size = Brain_rxBufferLengh
};
 



/**
  * @brief   串口2视觉回调函数 
  * @param[in]  
  */
uint8_t Brain_callback(uint8_t * recBuffer, uint16_t len)
{
	Brain_DataUnpack(&Vision_Info,recBuffer,&Holder);

	return 0;
}


/**
  * @brief  上位机数据解算
  */
int taest_single;
double offangle;
float Single_limt;
void Brain_DataUnpack(Trace_t* Info_Vision, uint8_t* recBuffer,Holder_t* holder)
{
	
	if(recBuffer[0] == 0xAA)
	{
		if( recBuffer[1] == 0xDD) {Vision_Info.Flag_Vision_Del_Small = 0;}
		else if( recBuffer[7] == 0xDD)  //< 解算偏转角
		{
			//< yaw偏转角 在（-63.99 ~ +63.99）范围内
			if((recBuffer[1] >> 6) == 0) 
				Info_Vision->Core_Info.YawDeflectionAngle = ((float)((recBuffer[1]&0x3f)*100 + recBuffer[2])/100);
			else if((recBuffer[1] >> 6) == 1) 
				Info_Vision->Core_Info.YawDeflectionAngle = (-1) * ((float)((recBuffer[1]&0x3f)*100 + recBuffer[2])/100);
			//< pitch偏转角 在（-63.99 ~ +63.99）范围内
			if((recBuffer[3] >> 6) == 0) 
				Info_Vision->Core_Info.PitchDeflectionAngle = ((float)((recBuffer[3]&0x3f)*100 + recBuffer[4])/100);
			else if((recBuffer[3] >> 6) == 1) 
				Info_Vision->Core_Info.PitchDeflectionAngle =  -1* ((float)((recBuffer[3]&0x3f)*100 + recBuffer[4])/100);
			//< 距离信息 在（0 ~ 12.7）范围内

			Info_Vision->Core_Info.Distance = ((float)(recBuffer[5]))*100;
			Info_Vision->Core_Info.IsFire = ((float)(recBuffer[6]));

			offangle =fabs(atan(0.034/(Info_Vision->Core_Info.Distance/1000))*57.3);
			if (offangle<0.5)offangle=0.5;
			
			check_robot_state.usart_state.Check_vision = 0;
			Brain_yaw_raw_add = -Info_Vision->Core_Info.YawDeflectionAngle ;
			Brain_pitch_raw_add = Info_Vision->Core_Info.PitchDeflectionAngle ;
				
			if(Info_Vision->Core_Info.Distance < 6000 ) Single_limt = 0.8f;
			else Single_limt = 0.4f;

			tim14_FPS.Vision_cnt++;
		 
			//<自瞄
			if(Vision_Info.Hit_Mode == 0)
			{
				
				if((myABS(holder->Pitch.Target_Angle - INS_attitude->pitch) < 0.4f ) && myABS(holder->Yaw.v1 - INS_attitude->yaw_total_angle)	< offangle)
				{
					Vision_Info.Flag_Vision_Del_Small = 1;
				}
				else
				{
					Vision_Info.Flag_Vision_Del_Small = 0;	
				}
				
				holder->Yaw.Target_Angle = YawVisionSence * Brain_yaw_raw_add + INS_attitude->yaw_total_angle;
				holder->Pitch.Target_Angle = PitchVisionSence * Brain_pitch_raw_add + INS_attitude->pitch;	
				
				Symbol_delay = 0;
				Vision_Info.Single_Flag_Vision_Del_Small = 0;
			}
			//<打符
			else if(Vision_Info.Hit_Mode == 1 || Vision_Info.Hit_Mode == 2)
			{
				taest_single++;
				if( Vision_Info.Single_Flag_Vision_Del_Small == 1) Symbol_delay ++ ;
				else if((myABS(holder->Pitch.Target_Angle - INS_attitude->pitch) < Single_limt) && myABS(holder->Yaw.v1 - INS_attitude->yaw_total_angle)	< Single_limt)
				{
					Symbol_delay ++;
					if(Symbol_delay > 5) Vision_Info.Single_Flag_Vision_Del_Small = 1;
				}
				
				if (Symbol_delay > 70){
					Symbol_delay = 0;
					Vision_Info.Single_Flag_Vision_Del_Small = 0;
				}

				holder->Yaw.Target_Angle = YawVisionSence * Brain_yaw_raw_add +  INS_attitude->yaw_total_angle ;
				holder->Pitch.Target_Angle = PitchVisionSence * Brain_pitch_raw_add + INS_attitude->pitch;	
			
			}
			else 
			{
				taest_single = 0;
				Symbol_delay = 0;
				Vision_Info.Single_Flag_Vision_Del_Small = 0;
			}

		}


	}

}


/**
  * @brief  下位机向上位机发送时间戳以及四元数
  */

int Bule_Red;
uint8_t Single; 
void Brain_RobotToBrainTime(void)
{
	int16_t tmp0,tmp1,tmp2,tmp3,tmp4,cnt;
	tim14.Brain_Time++;
	tmp0 = (int16_t)(INS_attitude->q[0] *  30000);
	tmp1 = (int16_t)(INS_attitude->q[1] *  30000);
	tmp2 = (int16_t)(INS_attitude->q[2] *  30000);
	tmp3 = (int16_t)(INS_attitude->q[3] *  30000);

	if(referee2024.robot_status_t.robot_id > 10)  Bule_Red = 0;
	else Bule_Red = 1;

	//自瞄
	
	RobotToBrainTimeBuffer[0]  = 0xAA;
	RobotToBrainTimeBuffer[1]  = ( tim14.ClockTime >>24);    
	RobotToBrainTimeBuffer[2]  = ((tim14.ClockTime >>16)&0xff);
	RobotToBrainTimeBuffer[3]  = ((tim14.ClockTime >>8)&0xff);
	RobotToBrainTimeBuffer[4]  = ((tim14.ClockTime &0xff));
	RobotToBrainTimeBuffer[5]  = Bule_Red;              
	RobotToBrainTimeBuffer[6]  = tmp0 & 0xFF;                   
	RobotToBrainTimeBuffer[7]  = tmp0 >> 8;
	RobotToBrainTimeBuffer[8]  = tmp1 & 0xFF;
	RobotToBrainTimeBuffer[9]  = tmp1 >> 8;  
	RobotToBrainTimeBuffer[10] = tmp2 & 0xFF;             
	RobotToBrainTimeBuffer[11] = tmp2 >> 8;
	RobotToBrainTimeBuffer[12] = tmp3 & 0xFF;
	RobotToBrainTimeBuffer[13] = tmp3 >> 8; 
	RobotToBrainTimeBuffer[14] = Vision_Info.Hit_Mode;
	RobotToBrainTimeBuffer[15] = 0xDD;
	RobotToBrainTimeBuffer[16] = 0xDD;
	HAL_UART_Transmit_DMA(&huart6, RobotToBrainTimeBuffer, 17);
}


/*下位机对上位机数据发送主任务*/
void RobotToBrain(void)
{

	   Brain_RobotToBrainTime();   		
}

