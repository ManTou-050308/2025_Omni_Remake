 	/**@file  dr16.c
	* @brief    设备层
	* @details  主要包括构建串口管理器，提供串口初始化和用户回调重定义
	* @author      RyanJiao  any question please send mail to 1095981200@qq.com
	* @date        2021-10-9
	* @version     V1.0
	* @copyright    Copyright (c) 2021-2121  中国矿业大学CUBOT战队
	**********************************************************************************
	* @attention
	* 硬件平台: STM32H750VBT \n
	* SDK版本：-++++
	* @par 修改日志:
	* <table>
	* <tr><th>Date        <th>Version  <th>Author    <th>Description
	* <tr><td>2021-8-12  <td>1.0      <td>RyanJiao  <td>创建初始版本
	* </table>
	*
	**********************************************************************************
	 ==============================================================================
														How to use this driver  
	 ==============================================================================
	 

		********************************************************************************
		* @attention
		* 硬件平台: STM32H750VBT \n
		* SDK版本：-++++
		* if you had modified this file, please make sure your code does not have many 
		* bugs, update the version NO., write dowm your name and the date, the most
		* important is make sure the users will have clear and definite understanding 
		* through your new brief.
		********************************************************************************
	*/

//	#include "hardware_config.h"
	#include "check.h"
	#include "dr16.h"

	#include "brain.h"
	#include "Supercap.h"
	
	#include "omnichassis.h"
	#include "load.h"

	#include "hardware_config.h"


	
	#define myABS(x) ( (x)>0?(x):-(x) )
	RC_Ctrl_t Rc_Ctrl={
		.isUnpackaging = 0,
		.isOnline = 0
	};
	
int Q_dex;

	/**
		* @brief  创建dr16的接收机缓存数组
		*/
	uint8_t DR16_recData[DR16_rxBufferLengh];

uint8_t  DR16_RxBuffer0[36];
uint8_t  DR16_RxBuffer1[36];
	/**
		* @brief  创建dr16串口缓存区数据结构
		*/
	UART_RxBuffer uart1_buffer={
		.Data = DR16_recData,
		.Size = DR16_rxBufferLengh
	};

	/**
		* @brief  初始化接收机数据类型的数据，将杆量和按键信息归零
		*/
	void DR16Init(RC_Ctrl_t* rc_ctrl)
	{
		rc_ctrl->rc.ch0=1024;
		rc_ctrl->rc.ch1=1024;
		rc_ctrl->rc.ch2=1024;
		rc_ctrl->rc.ch3=1024;
		rc_ctrl->rc.s1=3;
		rc_ctrl->rc.s2=3;
		rc_ctrl->rc.sw=1024;
		rc_ctrl->mouse.x=0;
		rc_ctrl->mouse.y=0;
		rc_ctrl->mouse.z=0;		
		rc_ctrl->OneShoot = 0;
		rc_ctrl->key_Q_flag=0;
		rc_ctrl->key_E_flag=0;    //< 上电关弹舱
		rc_ctrl->key_R_flag=0;
		rc_ctrl->key_F_flag=0;
		rc_ctrl->key_G_flag=0;
		rc_ctrl->key_Z_flag=0;
		rc_ctrl->key_X_flag=0;
		rc_ctrl->key_C_flag=0;
		rc_ctrl->key_V_flag=0;
		rc_ctrl->key_B_flag=0;
		rc_ctrl->Chassis_Y_Integ=0;//斜坡积分变量
		rc_ctrl->Chassis_X_Integ=0;
		rc_ctrl->ShootNumber=9;
		rc_ctrl->Cruise_Mode = 0;
	}


	/**
		* @brief  创建dr16的接收机缓存数组
		*/
	uint8_t DR16_callback(uint8_t * recBuffer, uint16_t len)
	{
		DR16_DataUnpack(&Rc_Ctrl, recBuffer, len); 		//< callback函数由格式限制
		KeyBoard_DataUnpack(&Rc_Ctrl, recBuffer,len,&Vision_Info, &OmniChassis,&Super_Cap ,&Loader);
		if (tim14_FPS.Receiver_FPS>10) check_robot_state.usart_state.Check_receiver = 0;  //在线第二道保险
		
		return 0;
	}


	/**
		* @brief  创建dr16的接收机缓存数组, 并对全局变量rc_Ctrl赋值，以供其他函数调用
		*/
	void DR16_DataUnpack(RC_Ctrl_t* rc_ctrl, uint8_t * recBuffer, uint16_t len)
{ 
		tim14_FPS.Receiver_cnt++;
	
		rc_ctrl->isUnpackaging = 1;					//< 解算期间不允许读取数据
		uint8_t correct_num=0;
		correct_num=0;
		if(((recBuffer[0] | (recBuffer[1] << 8)) & 0x07ff)<=1684 && ((recBuffer[0] | (recBuffer[1] << 8)) & 0x07ff)>=364)
			correct_num++;
		if((((recBuffer[1] >> 3) | (recBuffer[2] << 5)) & 0x07ff)<=1684 && (((recBuffer[1] >> 3) | (recBuffer[2] << 5)) & 0x07ff)>=364)
			correct_num++;
		if((((recBuffer[2] >> 6) | (recBuffer[3] << 2) |(recBuffer[4] << 10)) & 0x07ff)<=1684 && (((recBuffer[2] >> 6) | (recBuffer[3] << 2) |(recBuffer[4] << 10)) & 0x07ff)>=364)
			correct_num++;
		if((((recBuffer[4] >> 1) | (recBuffer[5] << 7)) & 0x07ff)<=1684 && (((recBuffer[4] >> 1) | (recBuffer[5] << 7)) & 0x07ff)>=364)
			correct_num++;
		if((((recBuffer[5] >> 4)& 0x000C) >> 2)==1 || (((recBuffer[5] >> 4)& 0x000C) >> 2)==2 || (((recBuffer[5] >> 4)& 0x000C) >> 2)==3)
			correct_num++;
		if(((recBuffer[5] >> 4)& 0x0003)==1 || ((recBuffer[5] >> 4)& 0x0003)==2 || ((recBuffer[5] >> 4)& 0x0003)==3)
			correct_num++;
		if(correct_num==6)																																												//< 数据完整性验证 
		{
			rc_ctrl->rc.ch0 = (recBuffer[0]| (recBuffer[1] << 8)) & 0x07ff; 																					//< Channel 0   高8位与低3位
			rc_ctrl->rc.ch1 = ((recBuffer[1] >> 3) | (recBuffer[2] << 5)) & 0x07ff; 																	//< Channel 1   高5位与低6位
			rc_ctrl->rc.ch2 = ((recBuffer[2] >> 6) | (recBuffer[3] << 2) |(recBuffer[4] << 10)) & 0x07ff; 						//< Channel 2
			rc_ctrl->rc.ch3 = ((recBuffer[4] >> 1) | (recBuffer[5] << 7)) & 0x07ff; 																	//< Channel 3
			rc_ctrl->rc.s1 = ((recBuffer[5] >> 4)& 0x000C) >> 2; 																											//!< Switch left
			rc_ctrl->rc.s2 = ((recBuffer[5] >> 4)& 0x0003); 				//提取数组5的3，4位																									//!< Switch right
			rc_ctrl->rc.sw=(uint16_t)(recBuffer[16]|(recBuffer[17]<<8))&0x7ff; 
				
			if((rc_ctrl->rc.ch0>1020)&&(rc_ctrl->rc.ch0<1028))          //遥控器零飘
				rc_ctrl->rc.ch0=1024;
			if((rc_ctrl->rc.ch1>1020)&&(rc_ctrl->rc.ch1<1028))
				rc_ctrl->rc.ch1=1024;
			if((rc_ctrl->rc.ch2>1020)&&(rc_ctrl->rc.ch2<1028))
				rc_ctrl->rc.ch2=1024;
			if((rc_ctrl->rc.ch3>1020)&&(rc_ctrl->rc.ch3<1028))
				rc_ctrl->rc.ch3=1024;
			
				/***********按键映射*************/
			rc_ctrl->mouse.x = recBuffer[6]  | (recBuffer[7] << 8);                       //< Mouse X axis   
			rc_ctrl->mouse.y = recBuffer[8]  | (recBuffer[9] << 8);                       //< Mouse Y axis     
			rc_ctrl->mouse.z = recBuffer[10] | (recBuffer[11] << 8);                      //< Mouse Z axis   
			rc_ctrl->mouse.press_l = recBuffer[12];                                       //< Mouse Left Is Press ?   
			rc_ctrl->mouse.press_r = recBuffer[13];                                       //< Mouse Right Is Press ? 
							
			if(rc_ctrl->mouse.x>25000)   rc_ctrl->mouse.x=25000;     																												//< 限幅
			if(rc_ctrl->mouse.x<-25000)  rc_ctrl->mouse.x=-25000;	
			if(rc_ctrl->mouse.y>25000)   rc_ctrl->mouse.y=25000;
			if(rc_ctrl->mouse.y<-25000)  rc_ctrl->mouse.y=-25000;
				
			rc_ctrl->keyboard.v = recBuffer[14]| (recBuffer[15] << 8);  									//< 共16个按键值   

			rc_ctrl->key_W=recBuffer[14]&0x01;	
			rc_ctrl->key_S=(recBuffer[14]>>1)&0x01;					
			rc_ctrl->key_A=(recBuffer[14]>>2)&0x01;
			rc_ctrl->key_D=(recBuffer[14]>>3)&0x01;					
			rc_ctrl->key_B=(recBuffer[15]>>7)&0x01;
			rc_ctrl->key_V=(recBuffer[15]>>6)&0x01;				
			rc_ctrl->key_C=(recBuffer[15]>>5)&0x01;
			rc_ctrl->key_X=(recBuffer[15]>>4)&0x01;					
			rc_ctrl->key_Z=(recBuffer[15]>>3)&0x01;					
			rc_ctrl->key_G=(recBuffer[15]>>2)&0x01;			
			rc_ctrl->key_F=(recBuffer[15]>>1)&0x01;
			rc_ctrl->key_R=(recBuffer[15])&0x01;													
			rc_ctrl->key_E=(recBuffer[14]>>7)&0x01;
			rc_ctrl->key_Q=(recBuffer[14]>>6)&0x01;
			rc_ctrl->key_ctrl=(recBuffer[14]>>5)&0x01;
			rc_ctrl->key_shift=(recBuffer[14]>>4)&0x01;
			PC_keybroad_filter(rc_ctrl);				//< 防抖
			
		}
		rc_ctrl->isUnpackaging = 0;					//< 解算完成标志位，允许读取
		

}
uint8_t VisionFlag = 0 ;
uint8_t VisionFlag_Last = 0 ;
int dr16; //遥控标志
int aga_small;
int aga_big;
int big;
	void KeyBoard_DataUnpack(RC_Ctrl_t* rc_ctrl, uint8_t * recBuffer, uint16_t len, Trace_t* Info_Vision , Omni_Chassis_t* chassis , Super_Cap_t* Cap ,Loader_t* loader)
{

/*********************电容及自旋控制*****************************/		
/*正常遥控器*/	
		if(rc_ctrl->rc.sw <800 || rc_ctrl->key_R_flag % 2 == 1 || rc_ctrl->key_ctrl == 1)
		{ 	
			Cap->cap_state.Supercap_Mode = 1;
			chassis->Flag.RollingFlag = 1;
		}
		else if(rc_ctrl->key_shift == 1 || rc_ctrl->rc.sw >1400)	
		{
			Cap->cap_state.Supercap_Mode = 1;
		}
		else
		{
			Cap->cap_state.Supercap_Mode = 0;
			chassis->Flag.RollingFlag = 0;
		}	
		if(rc_ctrl->rc.sw <800)
		{
			dr16 = 1;
		}
		else dr16 = 0;
		
		chassis->Flag.RollingFlagLast = chassis->Flag.RollingFlag;

// F小符 G反小符 V大符 B反大符

/**********************视觉模式选择***************************/		 
										
	//<自瞄选择打符或则机器人
//	if(rc_ctrl->rc.s2 == 2)	VisionFlag = rc_ctrl->rc.s1;
//		
//	if (rc_ctrl->key_G_flag == 1 || aga_small == 1){
//		Info_Vision->Hit_Mode = 3;  //反小符
//	}
//	else if(rc_ctrl->key_F_flag == 1 || VisionFlag == 2){
//		Info_Vision->Hit_Mode = 1; //小符
//		VisionFlag_Last       = 3;
//	}
//	else if(rc_ctrl->key_B_flag == 1 || aga_big == 1){
//		Info_Vision->Hit_Mode = 4; //反大符
//	}
//	else if(rc_ctrl->key_V_flag == 1 || big == 1){
//		Info_Vision->Hit_Mode = 2;  //大符
//	}
//	else if (rc_ctrl->mouse.press_r_flag == 1 || VisionFlag == 1){
//		Info_Vision->Hit_Mode = 0;  //预测
//		VisionFlag_Last       = 3;
//	}
//	else{
//		Info_Vision->Hit_Mode = 5;  //手瞄
//	}
	
	
		if(rc_ctrl->rc.s2 == 2)	VisionFlag = rc_ctrl->rc.s1;
		

	if(rc_ctrl->key_B_flag == 1 || VisionFlag == 2){
		Info_Vision->Hit_Mode = 1; //小符
		VisionFlag_Last       = 3;
	}
	else if(rc_ctrl->key_G_flag == 1 || big == 1){
		Info_Vision->Hit_Mode = 2;  //大符
	}
	else if (rc_ctrl->mouse.press_r_flag == 1 || VisionFlag == 1){
		Info_Vision->Hit_Mode = 0;  //预测
		VisionFlag_Last       = 3;
	}
	else{
		Info_Vision->Hit_Mode = 3;  //手瞄
	}
	VisionFlag_Last = VisionFlag;

//在加速时，车辆可能需要一段时间才能达到最大速度，避免直接跳跃到最高速度。
//转向时，倾斜角度的平滑过渡，避免车辆瞬间转向。
//低速时：车辆容易做出更大的转向反应。
//高速时：车辆转向响应会减弱，需要更大的输入才能发生相同的转向。

/*--------------------正弦代替阶跃的键鼠控制开始--------------------*/				
		if((rc_ctrl->rc.ch1-1024) > 300)   rc_ctrl->key_W = 1;
		else if((rc_ctrl->rc.ch1-1024) < -300) rc_ctrl->key_S=1;
		
		if((rc_ctrl->rc.ch0-1024)>300)   rc_ctrl->key_D=1;
		else if((rc_ctrl->rc.ch0-1024)<-300)  rc_ctrl->key_A=1;
			/*********前后加速减速处理*********/	
		 if(rc_ctrl->Chassis_Y_Integ>0 && (rc_ctrl->key_S-rc_ctrl->key_W)>=0 )    //有前进期望速度且期望能减速
		 {
				rc_ctrl->Chassis_Y_Integ-=ACE_SHACHE*160*pow(rc_ctrl->Chassis_Y_Integ,2);  //Ace_Sense为加速度灵敏度
				if(rc_ctrl->Chassis_Y_Integ<0.2)  rc_ctrl->Chassis_Y_Integ=0;	
		 }
		 else if(rc_ctrl->Chassis_Y_Integ<0 && (rc_ctrl->key_S-rc_ctrl->key_W)<=0 )    //有前进期望速度且期望能减速
		 {
				rc_ctrl->Chassis_Y_Integ+=ACE_SHACHE*160*pow(rc_ctrl->Chassis_Y_Integ,2);  //Ace_Sense为加速度灵敏度
				if(rc_ctrl->Chassis_Y_Integ>-0.2)  rc_ctrl->Chassis_Y_Integ=0;	
		 }
		 else              //加速
		 { 
					 
			 rc_ctrl->Chassis_Y_Integ +=ACE_SENSE*0.3f*(rc_ctrl->key_W - rc_ctrl->key_S);//正常行驶  //Shoot_Info.bullet_cap_status=1;//开弹仓 							 

			 if(rc_ctrl->Chassis_Y_Integ>3)          rc_ctrl->Chassis_Y_Integ=3;
			 else if (rc_ctrl->Chassis_Y_Integ<-3)   rc_ctrl->Chassis_Y_Integ=-3;									 
		 }
		 
		 /*********左右加速减速处理*********/	 
		 		 if(rc_ctrl->Chassis_X_Integ>0 && (rc_ctrl->key_A-rc_ctrl->key_D)>=0 )    //有右进期望速度且期望能减速
		 {
				rc_ctrl->Chassis_X_Integ -=ACE_SHACHE*160*pow(rc_ctrl->Chassis_X_Integ,2);  //Ace_Sense为加速度灵敏度
				if(rc_ctrl->Chassis_X_Integ<0.2)  rc_ctrl->Chassis_X_Integ=0;	
		 }
		 else if(rc_ctrl->Chassis_X_Integ<0 && (rc_ctrl->key_A-rc_ctrl->key_D)<=0 )    //有前进期望速度且期望能减速
		 {
				rc_ctrl->Chassis_X_Integ +=ACE_SHACHE*160*pow(rc_ctrl->Chassis_X_Integ,2);  //Ace_Sense为加速度灵敏度
				if(rc_ctrl->Chassis_X_Integ>-0.2)  rc_ctrl->Chassis_X_Integ=0;	
		 }
		 else              //加速
		 { 
					 
			 rc_ctrl->Chassis_X_Integ +=ACE_SENSE*0.3f*(rc_ctrl->key_D - rc_ctrl->key_A);//正常行驶  //Shoot_Info.bullet_cap_status=1;//开弹仓 							 

			 if(rc_ctrl->Chassis_X_Integ>3)          rc_ctrl->Chassis_X_Integ=3;
			 else if (rc_ctrl->Chassis_X_Integ<-3)   rc_ctrl->Chassis_X_Integ=-3;									 
		 }
		 
		 	
	/*	小陀螺与平动动态环	 */
		if((rc_ctrl->Chassis_X_Integ/3 >0.1f || rc_ctrl->Chassis_X_Integ/3 < -0.1f) || (rc_ctrl->Chassis_Y_Integ/3 >0.1f || rc_ctrl->Chassis_Y_Integ/3 < -0.1f))
		{
			rc_ctrl->Ring.ring[Q_dex] =  sqrt(rc_ctrl->Chassis_X_Integ/3 * rc_ctrl->Chassis_X_Integ/3 + rc_ctrl->Chassis_Y_Integ/3 * rc_ctrl->Chassis_Y_Integ/3)/sqrt(2);
		}
		else rc_ctrl->Ring.ring[Q_dex] = 0 ;
	
		Q_dex++;
		rc_ctrl->Ring.Q_dexSum=0;
		if(Q_dex >=10) Q_dex = 0;
		for(int i; i<10 ; i++)
		{
			rc_ctrl->Ring.Q_dexSum +=rc_ctrl->Ring.ring[i];
		}
		rc_ctrl->Ring.Q_dexAverge =rc_ctrl->Ring.Q_dexSum/10;

		 

/*--------------------------软件复位---------------------------------------*/	

		if(rc_ctrl->keyflag[6]==1 && rc_ctrl->keyflag[8]==1)	
		{
			rc_ctrl->RestFlag = 1; 
		}
		else{
			rc_ctrl->RestFlag = 0;
		}
	

}


void PC_keybroad_filter(RC_Ctrl_t* rc_ctrl)
{
	static uint16_t key_W_cnt,key_A_cnt,key_S_cnt,key_D_cnt,key_ctrl_cnt,
								 key_shift_cnt,mouse_press_l_cnt,mouse_press_r_cnt,
								 key_C_cnt,key_F_cnt,key_G_cnt,key_Q_cnt,key_E_cnt,
									key_Z_cnt,key_V_cnt,key_X_cnt,key_B_cnt,key_R_cnt;
		/*   支持连续按 W A S D   */
		//< key_W
	if(rc_ctrl->key_W==1) 
	{
		key_W_cnt++;
		if(key_W_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_W_flag=1;	
		}	 
	}   
	else
	{
		rc_ctrl->key_W_flag=0;	
		key_W_cnt=0;	
	}	
			//key_A
	if(rc_ctrl->key_A==1) 
	{
		key_A_cnt++;
		if(key_A_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_A_flag=1;
		}	
	}

	else
	{	
		key_A_cnt=0;	
		rc_ctrl->key_A_flag=0;
	}
	//key_S
	if(rc_ctrl->key_S==1) 
	{
		key_S_cnt++;
		if(key_S_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_S_flag=1;	
		}			
	}	
	else
	{
		key_S_cnt=0;
		rc_ctrl->key_S_flag=0;
	}		
	//key_D
	if(rc_ctrl->key_D==1) 
	{
		key_D_cnt++;
		if(key_D_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_D_flag=1;			
		}	
	}		
	else
	{
		key_D_cnt=0;
		rc_ctrl->key_D_flag=0;
	}
	
		//key_B
	if(rc_ctrl->key_B==1) 
	{
		key_B_cnt++;
		if(key_B_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_B_flag=1;
		}	
	}	
	else
	{
		key_B_cnt=0;
		rc_ctrl->key_B_flag=0;
	}
	
	//key_C
	if(rc_ctrl->key_C==1) 
	{
		key_C_cnt++;
		if(key_C_cnt==3)	
		{
			rc_ctrl->key_C_flag=1;
		}	 
	}  
	else
	{
		key_C_cnt=0;
		rc_ctrl->key_C_flag=0;
	}

		//key_R
	if(rc_ctrl->key_R==1) 
	{
		key_R_cnt++;
		if(key_R_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_R_flag = 1;
			key_R_cnt=0;

		}	 
	}  
	else
	{	
		rc_ctrl->key_R_flag=0;
//		 key_R_cnt=0;
	}
	//key_F
	if(rc_ctrl->key_F==1) 
	{
		key_F_cnt++;
		if(key_F_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_F_flag = 1;
			
		}	
	}	
	else
	{
		key_F_cnt=0;
		rc_ctrl->key_F_flag = 0;
	}
		//key_X
	if(rc_ctrl->key_X==1) 
	{
		key_X_cnt++;
		if(key_X_cnt==3)	
		{
		rc_ctrl->key_X_flag=1;			
//				rc_ctrl->key_X_flag++;
//				if(rc_ctrl->key_X_flag>1) 
//					rc_ctrl->key_X_flag=1;
		}	 
	}	
	else
	{
		key_X_cnt=0;
		rc_ctrl->key_X_flag=0;
	}
	
	//key_G
	if(rc_ctrl->key_G == 1) 
	{
		key_G_cnt++;
		if(key_G_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_G_flag = 1;
			
		}
	}
	else
	{
		key_G_cnt = 0;
		rc_ctrl->key_G_flag = 0;
	}	

	
	//key_Q
	if(rc_ctrl->key_Q==1) 
	{
		key_Q_cnt++;
		if(key_Q_cnt==Key_Filter_Num)	
		{
		rc_ctrl->key_Q_flag=1;			
		}	
		else
	 {
		rc_ctrl->key_Q_flag=0;	
	 }	
	}	
	else
	{
		key_Q_cnt=0;
		rc_ctrl->key_Q_flag=0;
	}
	//key_E
	if(rc_ctrl->key_E==1) 
	 {
		key_E_cnt++;
		if(key_E_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_E_flag++;	
		if(rc_ctrl->key_E_flag>1)  rc_ctrl->key_E_flag=0;			
		}	 
	 }
	 else
	 {
		key_E_cnt=0;
		//rc_ctrl.key_E_flag=0;
	 }	
	//key_Z
		if(rc_ctrl->key_Z==1) 
	{
		key_Z_cnt++;
		if(key_Z_cnt==Key_Filter_Num)	//rc_ctrl.key_Z_flag清0的位置可以思考一下
		{

			rc_ctrl->key_Z_flag=1;
		}			
	}	else
	 {
		key_Z_cnt=0;
		rc_ctrl->key_Z_flag=0;
	 } 
	//key_V
		if(rc_ctrl->key_V==1) 
	{
		key_V_cnt++;
		if(key_V_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_V_flag = 1;			
		}			
	}	else
	 {
			key_V_cnt=0;
		rc_ctrl->key_V_flag=0;
	 } 	 
	 
	//key_ctrl
	if(rc_ctrl->key_ctrl==1) 
	{
		key_ctrl_cnt++;
		if(key_ctrl_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_ctrl_flag=1;
			key_ctrl_cnt=0;	
		}	
	} 
	else
	{
		rc_ctrl->key_ctrl_flag=0;
	}	 
	//key_shift 
	 if(rc_ctrl->key_shift==1) 
	{
		key_shift_cnt++;
		if(key_shift_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_shift_flag=1;
			key_shift_cnt=0;	
		}	
	 }	
	else
	{
		rc_ctrl->key_shift_flag=0;
	}
	 //mouse_l
	 if(rc_ctrl->mouse.press_l==1)
	 {
		mouse_press_l_cnt++;
		if(mouse_press_l_cnt== 1 )
		{
			rc_ctrl->mouse.press_l_flag=1;
			mouse_press_l_cnt=0;
		}
	 }
	 else
	 {
		 rc_ctrl->mouse.press_l_flag=0;
	 }
		 //mouse_r
	 if(rc_ctrl->mouse.press_r==1)
	 {
		mouse_press_r_cnt++;
		if(mouse_press_r_cnt==Key_Filter_Num)
		{
			rc_ctrl->mouse.press_r_flag=1;
			mouse_press_r_cnt=0;
		}
	 }
	 else
	 {
		 rc_ctrl->mouse.press_r_flag=0;
	 }
}


