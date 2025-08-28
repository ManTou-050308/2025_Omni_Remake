#include "hardware_config.h"
#include "control_logic.h"


#include "motor.h"
#include "referee.h"
#include "bmi088.h"

#include "Supercap.h"
#include "dr16.h"
#include "vt13.h"
#include "brain.h"
#include "Supercap.h"

#include "holder.h"
#include "omnichassis.h"
#include "load.h"
#include "check.h"
#include "SMC.h"
#include "driver_timer.h"
void MPU_Init(void)
{
	//MUP structure variable define
	MPU_Region_InitTypeDef MPU_Config;
	
	/*-----------Open FPU--------*///High speed FLOAT calculate
	SCB->CPACR |= ((3UL << (10*2))|(3UL << (11*2)));  /* set CP10 and CP11 Full Access */
	/*-----------Open Cache------------*/
	SCB_EnableICache();//ʹ��I-Cache
  SCB_EnableDCache();//ʹ��D-Cache   
	SCB->CACR|=1<<2;   //ǿ��D-Cache͸д,�粻����,ʵ��ʹ���п���������������	
	/*-----------Open MPU------------*/
	HAL_MPU_Disable();
	
	MPU_Config.Enable=MPU_REGION_ENABLE;
	MPU_Config.Number=MPU_REGION_NUMBER1;//��������� 1
	MPU_Config.BaseAddress= 0x24000000;//����������ַ
	MPU_Config.Size=MPU_REGION_SIZE_512KB;//���ñ�����512k
	MPU_Config.SubRegionDisable=0x00;//��ֹ������
	MPU_Config.TypeExtField=MPU_TEX_LEVEL0;//����������չ��Ϊlevel0
	MPU_Config.AccessPermission=MPU_REGION_FULL_ACCESS;//ȫ���ʣ���Ȩ&�û����ɷ��ʣ�
	MPU_Config.DisableExec=MPU_INSTRUCTION_ACCESS_ENABLE;//����ָ�����
	MPU_Config.IsShareable=MPU_ACCESS_SHAREABLE;//������
	MPU_Config.IsCacheable=MPU_ACCESS_CACHEABLE;//����cache
	MPU_Config.IsBufferable=MPU_ACCESS_NOT_BUFFERABLE;//�������� DMAģʽ��Ҫ��Ϊ������
	HAL_MPU_ConfigRegion(&MPU_Config);
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}	
/**
  * @brief ȫ�ֽṹ�崴����ʹ��
	* @note  
		XXX_T �����ṹ��ԭ��
		XXX_t �ṹ�崴��������
		XXX		ʵ���ṹ��
		xxx   ���νṹ��ʱ����

		��ģ���ﴴ���ṹ��
		typedef struct  XXX_T
		{

		}XXX_t��

		��hardwarexxx.h ȫ����ǰ���� 		//<Ŀ����������ģ���е�������ģ��ṹ��ʱ����ṹ��ָ�벻����Ϊͷ�ļ���������
		typedef struct XXX_T XXX_t;
		����hardwarexxx.c ʵ�����ṹ
		XXX_t XXX;
		����hardwarexxx.h ʵ����ȫ������
		extern XXX_t XXX;
		
		������ģ����ýṹ��ʱ
		��ģ�����ýṹ����Ӧ.h�ļ���hardwarexxx.h�ļ�
		
		������Ҫ��ԭ.c�ļ��н���ʵ�����ṹ�岢���и�ֵ�� Ҳ����Ҫ����hardwarexxx.h ȫ����ǰ������ʵ����ȫ������
		
		
  */

	uint8_t vofa_callback(uint8_t * recBuffer, uint16_t len)
{
	return 0;
}
uint8_t Usart8_RxBuffer[128];
UART_RxBuffer uart8_buffer={
	.Data = Usart8_RxBuffer,
	.Size = 128
};
//uint8_t Usart6_RxBuffer[128];
//UART_RxBuffer uart6_buffer={
//	.Data = Usart6_RxBuffer,
//	.Size = 128
//};
	Trace_t					Vision_Info;
	Super_Cap_t			Super_Cap;
	Holder_t				Holder;
	Omni_Chassis_t 	OmniChassis;
	Loader_t				Loader;

	BasePID_Object pid_yaw_reset;
	BasePID_Object pid_pitch_reset;

	BasePID_Object pid_yaw_angle;
	BasePID_Object pid_yaw_speed;
	BasePID_Object pid_yaw_vision_angle;
	BasePID_Object pid_yaw_vision_speed;
	BasePID_Object pid_yaw_vision_single_angle;
	BasePID_Object pid_yaw_vision_single_speed;
	
	BasePID_Object pid_follow;
	BasePID_Object pid_run;
	
	BasePID_Object pid_pitch_angle;
	BasePID_Object pid_pitch_speed;
	BasePID_Object pid_pitch_vision_angle;
	BasePID_Object pid_pitch_vision_speed;
	BasePID_Object pid_pitch_vision_single_angle;
	BasePID_Object pid_pitch_vision_single_speed;
	
	BasePID_Object pid_load_continue;
	BasePID_Object pid_load_single_speed;
	BasePID_Object pid_load_single_angle;
	
	BasePID_Object pid_friction;
	BasePID_Object pid_ecd;
	

	void HardwareConfig(void)
	{
		Zero_Init(&Holder);
		MPU_Init();
		DR16Init(&Rc_Ctrl);
		DWT_Init(480);
		INS_Init(&bmi088.bmi088_Data);
//		INS_Init(&mpu6050.mpu6050_Data);
		
		check_robot_state.usart_state.Check_receiver=40; 
		check_robot_state.usart_state.Check_receiver_vt13=40; 
		
		BasePID_Init(&pid_yaw_angle ,1.4,0.01,70,0.5); //1,0.006,-30,1//1,0,0,0//5,0.009,-300,3
		BasePID_Init(&pid_yaw_speed,2500,0,0,0); //2500,0,0,0//-350,0,0,0// 600,0,-1,0

		BasePID_Init(&pid_pitch_angle ,2.8,0.05,-2,1.5); //2,0.02,-35,2// 3.5,0,0,0//2,0.0025,-4,3 && 0.13,0,1.5
		BasePID_Init(&pid_pitch_speed ,2000,0,0,0);   //-1500,0,0,0//-400,0,0,0//1000,0.01,0,0)&&325,0,50,0
    //<����
//		BasePID_Init(&pid_yaw_vision_angle ,4,0.5,-25,0.5); //4,0,-25,1
//		BasePID_Init(&pid_yaw_vision_speed,4800,0,0,0); //2000,0,0,50
//		BasePID_Init(&pid_pitch_vision_angle ,3.5,0,0,0.5); //4,0,-25,1
//		BasePID_Init(&pid_pitch_vision_speed,4000,0,0,0); //2000,0,0,50
		
		//�������
		BasePID_Init(&pid_yaw_vision_angle ,8,0.01,-90,1); //4,0,-25,1
		BasePID_Init(&pid_yaw_vision_speed,3000,0,0,0); //2000,0,0,50
		BasePID_Init(&pid_pitch_vision_angle ,5,0.1,8,1); //4,0,-25,1
		BasePID_Init(&pid_pitch_vision_speed,4000,0,0,0); //2000,0,0,50
		
		//<���
		BasePID_Init(&pid_yaw_vision_single_angle ,1.5,0.01,-15,0.7); //4,0,-25,1
		BasePID_Init(&pid_yaw_vision_single_speed,7000,0,0,0); //2000,0,0,50
		BasePID_Init(&pid_pitch_vision_single_angle ,2,0.05,-5,0.7); //4,0,-25,1
		BasePID_Init(&pid_pitch_vision_single_speed,3000,0,0,0); //2000,0,0,50


		BasePID_Init(&pid_follow,40,0,0,0);
		BasePID_Init(&pid_run,10,0,0,0);	
			
		BasePID_Init(&pid_load_continue,5,0 ,0, 0);
		BasePID_Init(&pid_load_single_speed,10,0 ,0, 0);
		BasePID_Init(&pid_load_single_angle,2.5,0.1 ,-0.6,60);
		
	  BasePID_Init(&pid_friction,6.5 ,0, 0, 0);
		
		SMC_Init(&smc_yaw,150,20,0.001,0.6,0.5);
		
		OmniChassisInit(&OmniChassis,pid_run,pid_follow,CAN2);
		HodlerInit(&Holder,CAN1,pid_pitch_reset,pid_pitch_angle,pid_pitch_speed,pid_pitch_vision_angle,pid_pitch_vision_speed,pid_yaw_reset ,pid_yaw_angle,pid_yaw_speed,pid_yaw_vision_angle,pid_yaw_vision_speed,pid_yaw_vision_single_speed,pid_yaw_vision_single_angle,pid_pitch_vision_single_speed,pid_pitch_vision_single_angle);
	  LoaderInit(&Loader,pid_friction,pid_load_continue,pid_load_single_speed,pid_load_single_angle);

		UARTx_Init(&huart1,DR16_callback);
		USART1_DoubleBuffer_init(DR16_RxBuffer0,DR16_RxBuffer1,36u);

		UARTx_Init(&huart8, vofa_callback);
	  UART_ENABLE_IT(&uart8, &uart8_buffer);

	  UARTx_Init(&huart6, Brain_callback);
	  UART_ENABLE_IT(&uart6, &uart6_buffer);

		UARTx_Init(&huart3, VT13_callback);
	  UART_ENABLE_IT(&uart3, &uart3_buffer);
		
		UARTx_Init(&huart4, Referee_callback);
	  UART_ENABLE_IT(&uart4, &uart4_buffer);

		CANx_Init(&hfdcan1, CAN1_rxCallBack);
		CAN_Open(&can1);
		CANx_Init(&hfdcan2, CAN2_rxCallBack);
		CAN_Open(&can2);
		
		TIMx_Init(&htim14, TIM14_Task);
		TIM_Open(&tim14);
		TIMx_Init(&htim13, TIM13_Task);
		TIM_Open(&tim13);
		
//		UART_Receive_DMA(&uart1, &uart1_buffer);
		UART_Receive_DMA(&uart4, &uart4_buffer);
		UART_Receive_DMA(&uart3, &uart3_buffer);
		UART_Receive_DMA(&uart6, &uart6_buffer);
		UART_Receive_DMA(&uart8, &uart8_buffer);



	}
	