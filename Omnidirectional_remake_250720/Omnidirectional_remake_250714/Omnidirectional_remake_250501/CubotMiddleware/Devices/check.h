#ifndef _CHECK_H
#define _CHECK_H

#include "stdint.h"
#include "referee.h"
#include "dr16.h"
#include "vt13.h"
#include "hardware_config.h"
#include "linux_list.h"

typedef struct
{
	struct
	{
	  uint16_t Check_receiver;
	  uint16_t Check_vision;
	  uint16_t Check_referee;
		uint16_t Check_receiver_vt13;
	}usart_state;
	
	struct
	{
		uint16_t Check_can2_0x201;       //底盘驱动电机
		uint16_t Check_can2_0x202;
		uint16_t Check_can2_0x203;
		uint16_t Check_can2_0x204;
		
		uint16_t Check_can1_0x201;       //摩擦轮
		uint16_t Check_can1_0x202;
	}motor3508_state;
	
	struct
	{
		uint16_t Check_can2_0x205;       //底盘转向电机
		uint16_t Check_can2_0x206;
		uint16_t Check_can2_0x207;
		uint16_t Check_can2_0x208;
		 
		uint16_t Check_can1_0x205;       //yaw轴电机
		uint16_t Check_can1_0x206;       //pitch轴电机
	}motor6020_state;
	
	struct
	{
		uint8_t Check_can1_0x203;       //播弹盘
	}motor2006_state;
	
	struct
	{
		uint16_t IsOnline;
		uint16_t power;
		uint16_t power_limit;
		uint16_t heat;
		uint16_t heat_limit;
		uint16_t power_buffer;
		uint16_t power_buffer_limit;
	}referee_state;
	
	uint16_t SuperCap_state;
	
}Check_Robot_State;

typedef struct
{
	float Vision_FPS;
	float Receiver_FPS;
	float Referee_FPS;
	float Holder_FPS;
	float Receiver_VT13_FPS;
	
	
	int   Receiver_cnt;
	int   Receiver_vt13_cnt;
	int   Referee_cnt;
	int   Vision_cnt;
	int   Holder_cnt;
	int   sin_cnt;
}FPS;

void RobotOnlineState(Check_Robot_State *CheckRobotState, Referee2024 *referee2024,RC_Ctrl_t *rc_ctrl,Super_Cap_t* Cap,VT13_t* vt13);
void FPS_Check(FPS * fps);
void Motor_CheckFPS(void);
extern Check_Robot_State check_robot_state;
extern FPS tim14_FPS;
#endif  
