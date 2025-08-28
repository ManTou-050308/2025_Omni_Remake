#include "check.h"
#include "referee.h"
#include "control_logic.h"
#include "Supercap.h"
#include "motor.h"
#include "vt13.h"

Check_Robot_State check_robot_state;

FPS tim14_FPS={
.Receiver_cnt= 0,
.Referee_cnt = 0,
.Vision_cnt = 0,
.Holder_cnt = 0
};


/**
  * @brief   ��ȡ������״̬���������Ƿ����� 
  * @param[in]  
  */	
void RobotOnlineState(Check_Robot_State *CheckRobotState, Referee2024 *referee2024,RC_Ctrl_t *rc_ctrl,Super_Cap_t* Cap,VT13_t* vt13)
{
	CheckRobotState->usart_state.Check_receiver++;
	CheckRobotState->usart_state.Check_receiver_vt13++;
	CheckRobotState->usart_state.Check_vision++;
	CheckRobotState->usart_state.Check_referee++;
	CheckRobotState->SuperCap_state++;

	CheckRobotState->referee_state.heat          = referee2024->power_heat_data_t.shooter_17mm_1_barrel_heat;           //ǹ�ڵ�ǰ����
	CheckRobotState->referee_state.heat_limit    = referee2024->robot_status_t.shooter_barrel_heat_limit;        //ǹ����������
	CheckRobotState->referee_state.power         = referee2024->power_heat_data_t.chassis_power;                           //���̵�ǰ����
	CheckRobotState->referee_state.power_limit   = referee2024->robot_status_t.chassis_power_limit;                   //���̹�������
	CheckRobotState->referee_state.power_buffer  = referee2024->power_heat_data_t.buffer_energy;                    //���̵�ǰ��������

	if(CheckRobotState->usart_state.Check_receiver > 100)
		CheckRobotState->usart_state.Check_receiver = 100;                                                                //�޷���������ֵ�ӵ�2^16��ͻ���0��ʼ���𣬳������ʧ��
	if(check_robot_state.usart_state.Check_receiver>30)
		rc_ctrl->isOnline = 0;
	else
		rc_ctrl->isOnline = 1;
	
	if(CheckRobotState->usart_state.Check_receiver_vt13 > 100)
		CheckRobotState->usart_state.Check_receiver_vt13 = 100;                                                                //�޷���������ֵ�ӵ�2^16��ͻ���0��ʼ���𣬳������ʧ��
	if(check_robot_state.usart_state.Check_receiver_vt13>30)
		vt13->isOnline = 0;
	else
		vt13->isOnline = 1;
	
	if(vt13->rc.mode_sw == 2 && rc_ctrl->isOnline == 0)
		vt13->isOnline = 1;
	else
		vt13->isOnline = 0;
		
	
	if(CheckRobotState->SuperCap_state > 500)
		CheckRobotState->SuperCap_state = 500;
	if(CheckRobotState->SuperCap_state > 400)
		Cap->cap_state.SupercapIsOnline = 0;
	else
		Cap->cap_state.SupercapIsOnline = 1;
	
	if(CheckRobotState->usart_state.Check_referee > 200)
		CheckRobotState->usart_state.Check_referee = 200;                                                                //�޷���������ֵ�ӵ�2^16��ͻ���0��ʼ���𣬳������ʧ��
	if(check_robot_state.usart_state.Check_referee>150)
		referee2024->IsOnline = 0;
	else
		referee2024->IsOnline = 1;
}

/**
  * @brief   ��ģ��֡�ʼ�� 
  * @param[in]  
  */	
void FPS_Check(FPS * fps)
{	
	fps->Receiver_FPS = fps->Receiver_cnt;
	fps->Referee_FPS = fps->Referee_cnt;
	fps->Vision_FPS = fps->Vision_cnt;
	fps->Holder_FPS = fps->Holder_cnt;
	fps->Receiver_VT13_FPS = fps->Receiver_vt13_cnt;
	fps->Receiver_cnt = 0;
	fps->Referee_cnt  = 0;
	fps->Vision_cnt   = 0;
	fps->Holder_cnt   = 0;
	fps->Receiver_vt13_cnt = 0;
}

/**
  * @brief   ���֡�ʼ�� 
  * @param[in]  
  */	
int Error_Can; // ����Can��;
void Motor_CheckFPS(void)
{
	Motor* motor = NULL;
	list_t *node = NULL;		
	for (node = can1.DevicesList.next;    		//< ��ѭ���������һȦ
			 node != (can1.DevicesList.prev->next);
			 node = node->next)
	{	
		motor = list_entry(node, Motor, list);  //< ��������ͷ�����ڽ�㡢��Ƕ������Ľṹ�����͡���Ƕ������Ľṹ������������������ƣ����ɷ���Ƕ��ͷ�����ڽ��Ľṹ��
    motor->Data.Online_check.FPS=motor->Data.Online_check.Cnt;
		motor->Data.Online_check.Cnt=0;
		if(motor->Data.Online_check.FPS == 0) Error_Can = 1;
	}
	for (node = can2.DevicesList.next;    		//< ��ѭ���������һȦ
			 node != (can2.DevicesList.prev->next);
			 node = node->next)
	{	
		motor = list_entry(node, Motor, list);  //< ��������ͷ�����ڽ�㡢��Ƕ������Ľṹ�����͡���Ƕ������Ľṹ������������������ƣ����ɷ���Ƕ��ͷ�����ڽ��Ľṹ��
    motor->Data.Online_check.FPS=motor->Data.Online_check.Cnt;
		motor->Data.Online_check.Cnt=0;
		
		if(motor->Data.Online_check.FPS == 0) Error_Can = 2;
	}
}

