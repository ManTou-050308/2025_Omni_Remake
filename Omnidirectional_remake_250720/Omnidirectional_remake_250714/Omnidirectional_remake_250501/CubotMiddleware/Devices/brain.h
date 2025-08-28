#ifndef _BRAIN_H__
#define _BRAIN_H__

#include "hardware_config.h"

#include "stm32h7xx.h"
#include "usart.h"

#define Brain_rxBufferLengh 	50
#define	BRAIN_TO_ROBOT_CMD  	1	 
#define	BRAIN_TO_ROBOT_HINT		2

/**
  * @brief    �ں˵Ļص������������ڽ������ݺ�ʵʱִ��ָ��
  * @param[in]  
  */
typedef uint8_t (*Brain_CoreCallback)(Holder_t* holder, RC_Ctrl_t* rc_ctrl); 
   
/**
  * @brief    �Ӿ��ṹ��
  * @param[in]  
  */
typedef  struct Trace_T
{	
	uint8_t Hit_Mode;
	uint8_t	Fire_Flag;
	uint8_t Flag_Vision_Del_Small;//�Ӿ����С��һ���̶�
	uint8_t Last_Flag_Vision_Del_Small;//�Ӿ����С��һ���̶�	
	uint8_t Single_Flag_Vision_Del_Small;
	uint8_t Last_Single_Flag_Vision_Del_Small;
	struct
	{
		uint8_t FrameCoreID;
		
		float YawDeflectionAngle;
		float PitchDeflectionAngle;
		float Distance;
		uint8_t IsFire;      //������
	}Core_Info;
	

	
}Trace_t;

/**
  * @brief      ��λ�����ݲ�ֽ��㺯��
	* @param[in]  brain      ��λ�����ݽṹ��
	* @param[in]  recBuffer  ��������������
  */
void Brain_DataUnpack(Trace_t* Info_Vision, uint8_t* recBuffer,Holder_t* holder);

/**
  * @brief      ��λ���͸���λ����Ƶ����
	*	@note      	����Brain_RobotToBrainTime
  */
void RobotToBrain(void);

/**
  * @brief      ��λ���͸���λ�����ݺ���
	*	@note       ��RobotToBrain����
  */
void Brain_RobotToBrainTime(void);

/**
  * @brief      ���ڻص�����
	* @param[in]  len      	 ���ݳ��������壩
	* @param[in]  recBuffer  ��������������
  */
uint8_t Brain_callback(uint8_t * recBuffer, uint16_t len);

extern float  PitchVisionSence;   //�Ӿ�pitch������
extern float YawVisionSence;      //�Ӿ�yaw������
extern float Brain_yaw_raw_add;   //�Ӿ�raw yaw
extern float Brain_pitch_raw_add; //�Ӿ�raw pitch

extern uint8_t ModeToBrain;       //�Ӿ�ģʽ�л�  
extern uint8_t ShootModeToBrain;  //��ؿ���Ԥ�� Ŀǰû�� �Ӿ�����Ԥ��

extern UART_RxBuffer uart6_buffer; //�Ӿ����ڽ��ܻ�����
extern uint8_t RobotToBrainTimeBuffer[23]; //�Ӿ����ڷ��ͻ�����
extern uint8_t Brain_recData[Brain_rxBufferLengh]; //�Ӿ����ڽ������ݽṹ��
extern int Bule_Red;
extern int Symbol_delay;           //�����ʱ
extern double offangle;
#endif

