#ifndef _BRAIN_H__
#define _BRAIN_H__

#include "hardware_config.h"

#include "stm32h7xx.h"
#include "usart.h"

#define Brain_rxBufferLengh 	50
#define	BRAIN_TO_ROBOT_CMD  	1	 
#define	BRAIN_TO_ROBOT_HINT		2

/**
  * @brief    内核的回调函数，用于在结算数据后实时执行指令
  * @param[in]  
  */
typedef uint8_t (*Brain_CoreCallback)(Holder_t* holder, RC_Ctrl_t* rc_ctrl); 
   
/**
  * @brief    视觉结构体
  * @param[in]  
  */
typedef  struct Trace_T
{	
	uint8_t Hit_Mode;
	uint8_t	Fire_Flag;
	uint8_t Flag_Vision_Del_Small;//视觉误差小于一定程度
	uint8_t Last_Flag_Vision_Del_Small;//视觉误差小于一定程度	
	uint8_t Single_Flag_Vision_Del_Small;
	uint8_t Last_Single_Flag_Vision_Del_Small;
	struct
	{
		uint8_t FrameCoreID;
		
		float YawDeflectionAngle;
		float PitchDeflectionAngle;
		float Distance;
		uint8_t IsFire;      //开火建议
	}Core_Info;
	

	
}Trace_t;

/**
  * @brief      上位机数据拆分解算函数
	* @param[in]  brain      上位机数据结构体
	* @param[in]  recBuffer  缓存区数组数据
  */
void Brain_DataUnpack(Trace_t* Info_Vision, uint8_t* recBuffer,Holder_t* holder);

/**
  * @brief      下位发送给上位机降频函数
	*	@note      	调用Brain_RobotToBrainTime
  */
void RobotToBrain(void);

/**
  * @brief      下位发送给上位机数据函数
	*	@note       被RobotToBrain调用
  */
void Brain_RobotToBrainTime(void);

/**
  * @brief      串口回调函数
	* @param[in]  len      	 数据长（无意义）
	* @param[in]  recBuffer  缓存区数组数据
  */
uint8_t Brain_callback(uint8_t * recBuffer, uint16_t len);

extern float  PitchVisionSence;   //视觉pitch灵敏度
extern float YawVisionSence;      //视觉yaw灵敏度
extern float Brain_yaw_raw_add;   //视觉raw yaw
extern float Brain_pitch_raw_add; //视觉raw pitch

extern uint8_t ModeToBrain;       //视觉模式切换  
extern uint8_t ShootModeToBrain;  //电控控制预测 目前没用 视觉控制预测

extern UART_RxBuffer uart6_buffer; //视觉串口接受缓冲区
extern uint8_t RobotToBrainTimeBuffer[23]; //视觉串口发送缓冲区
extern uint8_t Brain_recData[Brain_rxBufferLengh]; //视觉串口接受数据结构体
extern int Bule_Red;
extern int Symbol_delay;           //打符计时
extern double offangle;
#endif

