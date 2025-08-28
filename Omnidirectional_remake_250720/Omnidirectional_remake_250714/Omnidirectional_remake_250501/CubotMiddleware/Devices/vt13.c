	#include "vt13.h"
	#include "brain.h"
	#include "Supercap.h"
	
	#include "omnichassis.h"
	#include "load.h"
	
	#include "check.h"
	#include "hardware_config.h"
	
	VT13_t VT13={
		.isUnpackaging = 0,
		.isOnline = 0
	};
uint8_t VT13_recData[VT13_rxBufferLengh];
UART_RxBuffer uart3_buffer={
	.Data = VT13_recData,
	.Size = VT13_rxBufferLengh
};
	
uint8_t VT13_callback(uint8_t * recBuffer)
{
	vt13_recv_datas_modify(recBuffer,&VT13); 		//< callback函数由格式限制
	KeyBoard_DataUnpack_vt13(&VT13, recBuffer,&Vision_Info, &OmniChassis,&Super_Cap ,&Loader);
	if (tim14_FPS.Receiver_VT13_FPS>10) check_robot_state.usart_state.Check_receiver_vt13 = 0;  //在线第二道保险
	
	return 0;
	}
/*vt13遥控器*/
void vt13_recv_datas_modify(uint8_t *pdata,VT13_t* vt13 )
{  
	tim14_FPS.Receiver_vt13_cnt++;
	uint16_t Header;
	uint16_t crc16_temp;
	Header = pdata[0]<<8| pdata[1];
	//crc16_temp = Get_CRC16_Check((unsigned char*)pdata, 2+rc_data_lenth, 0xFFFF);
	if(Header == 0xA953)
	{
		vt13->onlineCheckCnt ++ ;
		/*注意新遥控的ch2和ch3和dt7是反的*/
		vt13->rc.ch0 = 	(pdata[2]| (pdata[3] << 8)) & 0x07ff;
		vt13->rc.ch1 = 	((pdata[3]>>3)| (pdata[4] << 5)) & 0x07ff;
		vt13->rc.ch2 =	((pdata[4] >> 6) | (pdata[5] << 2) |(pdata[6] << 10)) & 0x07ff; 						//< Channel 2
		vt13->rc.ch3 = 	((pdata[6] >> 1) | (pdata[7] << 7)) & 0x07ff; 	
		vt13->rc.mode_sw = ((pdata[7]>>4)&0x03);
		vt13->rc.go_home = ((pdata[7]>>6)&0x01);
		vt13->rc.Fn= ((pdata[7]>>7));
		vt13->rc.change = (pdata[8]&0x01);
		vt13->rc.sw = ((pdata[8]>>1)|(pdata[9]<<7)) & 0x07ff;
		vt13->rc.photo = (pdata[9]>>4)&0x03;
		vt13->mouse.x = (pdata[10])|(pdata[11]<<8);
		vt13->mouse.y = (pdata[12])|(pdata[13]<<8);
		vt13->mouse.z = (pdata[14])|(pdata[15]<<8);
		vt13->mouse.press_l = pdata[16]&0x03;
		vt13->mouse.press_r = (pdata[16]>>2)&0x03;
		vt13->mouse.press_m = (pdata[16]>>4)&0x03;//鼠标中键
			vt13->keyboard.keyboard14=pdata[17];
			vt13->keyboard.keyboard15=pdata[18];
			vt13->key_W_vT13=pdata[17]&0x01;	
			vt13->key_S_vT13=(pdata[17]>>1)&0x01;					
			vt13->key_A_vT13=(pdata[17]>>2)&0x01;
			vt13->key_D_vT13=(pdata[17]>>3)&0x01;
			vt13->key_shift_vT13=(pdata[17]>>4)&0x01;	
			vt13->key_ctrl_vT13=(pdata[17]>>5)&0x01;
			vt13->key_Q_vT13=(pdata[17]>>6)&0x01;		
			vt13->key_E_vT13=(pdata[17]>>7)&0x01;
			vt13->key_B_vT13=(pdata[18]>>7)&0x01;
			vt13->key_V_vT13=(pdata[18]>>6)&0x01;				
			vt13->key_C_vT13=(pdata[18]>>5)&0x01;
			vt13->key_X_vT13=(pdata[18]>>4)&0x01;					
			vt13->key_Z_vT13=(pdata[18]>>3)&0x01;					
			vt13->key_G_vT13=(pdata[18]>>2)&0x01;			
			vt13->key_F_vT13=(pdata[18]>>1)&0x01;
			vt13->key_R_vT13=(pdata[18])&0x01;													
		
			PC_keybroad_filter_vt13(vt13);
		
	}
}

void VT13Init( VT13_t * rc_ctrl)
{
		rc_ctrl->rc.ch0=1024;
		rc_ctrl->rc.ch1=1024;
		rc_ctrl->rc.ch2=1024;
		rc_ctrl->rc.ch3=1024;
		rc_ctrl->rc.sw=1024;
		rc_ctrl->mouse.x=0;
		rc_ctrl->mouse.y=0;
		rc_ctrl->mouse.z=0;		
		rc_ctrl->key_Q_flag=0;
		rc_ctrl->key_E_flag=0;   
		rc_ctrl->key_R_flag=0;
		rc_ctrl->key_F_flag=0;
		rc_ctrl->key_G_flag=0;
		rc_ctrl->key_Z_flag=0;
		rc_ctrl->key_X_flag=0;
		rc_ctrl->key_C_flag=0;
		rc_ctrl->key_V_flag=0;
		rc_ctrl->key_B_flag=0;

	
}
	
void PC_keybroad_filter_vt13(VT13_t* rc_ctrl)
{
	if(rc_ctrl->rc.mode_sw!=0){
	static uint16_t key_W_cnt,key_A_cnt,key_S_cnt,key_D_cnt,key_ctrl_cnt,
								 key_shift_cnt,mouse_press_l_cnt,mouse_press_r_cnt,
								key_C_cnt,key_F_cnt,key_G_cnt,key_Q_cnt,key_E_cnt,
								key_Z_cnt,key_V_cnt,key_X_cnt,key_B_cnt,key_R_cnt,key_shift_ctrl_z_cnt,key_shift_ctrl_x_cnt,key_shift_ctrl_v_cnt,key_shift_Q_cnt;
	static uint16_t key_shift_c_cnt,key_shift_ctrl_r_cnt,key_shift_V_cnt,key_shift_b_cnt,key_ctrl_shift_b_cnt;
			/*   支持连续按 W A S D   */
			//< key_W
	if(rc_ctrl->key_W==1) 
	{
		key_W_cnt++;
		if(key_W_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_W_flag_vT13=1;	
		}	 
	}   
	else
	{
		rc_ctrl->key_W_flag_vT13=0;	
		key_W_cnt=0;	
	}	
			//key_A
	if(rc_ctrl->key_A==1) 
	{
		key_A_cnt++;
		if(key_A_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_A_flag_vT13=1;
		}	
	}

	else
	{	
		key_A_cnt=0;	
		rc_ctrl->key_A_flag_vT13=0;
	}
	//key_S
	if(rc_ctrl->key_S==1) 
	{
		key_S_cnt++;
		if(key_S_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_S_flag_vT13=1;	
		}			
	}	
	else
	{
		key_S_cnt=0;
		rc_ctrl->key_S_flag_vT13=0;
	}		
	//key_D
	if(rc_ctrl->key_D==1) 
	{
		key_D_cnt++;
		if(key_D_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_D_flag_vT13=1;			
		}	
	}		
	else
	{
		key_D_cnt=0;
		rc_ctrl->key_D_flag_vT13=0;
	}
	
		//key_B
	if(rc_ctrl->key_B==1) 
	{
		key_B_cnt++;
		if(key_B_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_B_flag_vT13=1;
		}	
	}	
	else
	{
		key_B_cnt=0;
		rc_ctrl->key_B_flag_vT13=0;
	}
	//key_C
	if(rc_ctrl->key_C==1) 
	{
		key_C_cnt++;
		if(key_C_cnt==3)	
		{
			rc_ctrl->key_C_flag_vT13=1;
		}	 
	}  
	else
	{
		key_C_cnt=0;
		rc_ctrl->key_C_flag_vT13=0;
	}

		//key_R
	if(rc_ctrl->key_R==1) 
	{
		key_R_cnt++;
		if(key_R_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_R_flag_vT13 = 1;
			key_R_cnt=0;

		}	 
	}  
	else
	{	
		rc_ctrl->key_R_flag_vT13=0;
//		 key_R_cnt=0;
	}
	//key_F
	if(rc_ctrl->key_F==1) 
	{
		key_F_cnt++;
		if(key_F_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_F_flag_vT13=1;
		}	
	}	
	else
	{
		key_F_cnt=0;
		rc_ctrl->key_F_flag_vT13=0;
	}
		//key_X
	if(rc_ctrl->key_X==1) 
	{
		key_X_cnt++;
		if(key_X_cnt==3)	
		{
		rc_ctrl->key_X_flag_vT13=1;			
//				rc_ctrl->key_X_flag++;
//				if(rc_ctrl->key_X_flag>1) 
//					rc_ctrl->key_X_flag=1;
		}	 
	}	
	else
	{
		key_X_cnt=0;
		rc_ctrl->key_X_flag_vT13=0;
	}
	
	//key_G
	if(rc_ctrl->key_G == 1) 
	{
		key_G_cnt++;
		if(key_G_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_G_flag_vT13 = 1;
			
		}
	}
	else
	{
		key_G_cnt = 0;
		rc_ctrl->key_G_flag_vT13 = 0;
	}	

	
	//key_Q
	if(rc_ctrl->key_Q==1) 
	{
		key_Q_cnt++;
		if(key_Q_cnt==Key_Filter_Num)	
		{
		rc_ctrl->key_Q_flag_vT13=1;			
		}	
		else
	 {
		rc_ctrl->key_Q_flag_vT13=0;	
	 }	
	}	
	else
	{
		key_Q_cnt=0;
		rc_ctrl->key_Q_flag_vT13=0;
	}
	//key_E
	if(rc_ctrl->key_E==1) 
	 {
		key_E_cnt++;
		if(key_E_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_E_flag_vT13++;	
		if(rc_ctrl->key_E_flag_vT13>1)  rc_ctrl->key_E_flag_vT13=0;			
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

			rc_ctrl->key_Z_flag_vT13=1;
		}			
	}	else
	 {
		key_Z_cnt=0;
		rc_ctrl->key_Z_flag_vT13=0;
	 } 
	//key_V
		if(rc_ctrl->key_V==1) 
	{
		key_V_cnt++;
		if(key_V_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_V_flag_vT13 = 1;		
		}			
	}	else
	 {
		key_V_cnt=0;	
		rc_ctrl->key_V_flag_vT13=0;
	 } 	 
	 
	//key_ctrl
	if(rc_ctrl->key_ctrl==1) 
	{
		key_ctrl_cnt++;
		if(key_ctrl_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_ctrl_flag_vT13=1;
			key_ctrl_cnt=0;	
		}	
	} 
	else
	{
		rc_ctrl->key_ctrl_flag_vT13=0;
	}	 
	//key_shift 
	 if(rc_ctrl->key_shift==1) 
	{
		key_shift_cnt++;
		if(key_shift_cnt==Key_Filter_Num)	
		{
			rc_ctrl->key_shift_flag_vT13=1;
			key_shift_cnt=0;	
		}	
	 }	
	else
	{
		rc_ctrl->key_shift_flag_vT13=0;
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
//***************************************************************************	 
 } 
}
	
void KeyBoard_DataUnpack_vt13(VT13_t * vt13,uint8_t * recBuffer, Trace_t* Info_Vision , Omni_Chassis_t* chassis , Super_Cap_t* Cap ,Loader_t* loader)
{
	if( vt13->isOnline == 1){
	/*********************电容及自旋控制*****************************/		
/*正常遥控器*/	
		if( vt13->key_R_vT13 == 1 || vt13->key_ctrl_vT13 == 1 || vt13->rc.Fn == 1)
		{ 	
			Cap->cap_state.Supercap_Mode = 1;
			chassis->Flag.RollingFlag = 1;
		}
		else if(vt13->key_shift_vT13 == 1)	
		{
			Cap->cap_state.Supercap_Mode = 1;
		}
		else
		{
			Cap->cap_state.Supercap_Mode = 0;
			chassis->Flag.RollingFlag = 0;
		}	
/**********************视觉模式选择***************************/		 								
	//<自瞄选择打符或则机器人
//	if (vt13->key_G_flag == 1){
//		Info_Vision->Hit_Mode = 3;  //反小符
//	}
//	else if(vt13->key_F_flag == 1){
//		Info_Vision->Hit_Mode = 1; //小符
//	}
//	else if(vt13->key_B_flag == 1){
//		Info_Vision->Hit_Mode = 4; //反大符
//	}
//	else if(vt13->key_V_flag == 1){
//		Info_Vision->Hit_Mode = 2;  //大符
//	}
//	else if (vt13->mouse.press_r_flag == 1){
//		Info_Vision->Hit_Mode = 0;  //预测
//	}
//	else{
//		Info_Vision->Hit_Mode = 5;  //手瞄
//	}
	

	if(vt13->key_B_flag_vT13 == 1){
		Info_Vision->Hit_Mode = 1; //小符
	}
	else if(vt13->key_G_flag_vT13 == 1){
		Info_Vision->Hit_Mode = 2;  //大符
	}
	else if (vt13->mouse.press_r_flag == 1){
		Info_Vision->Hit_Mode = 0;  //预测
	}
	else{
		Info_Vision->Hit_Mode = 3;  //手瞄
	}
	
	
	if((vt13->rc.ch1-1024) > 300)   vt13->key_W = 1;
	else if((vt13->rc.ch1-1024) < -300) vt13->key_S=1;
	
	if((vt13->rc.ch0-1024)>300)   vt13->key_D=1;
	else if((vt13->rc.ch0-1024)<-300)  vt13->key_A=1;
	
		/*********前后加速减速处理*********/	
	 if(vt13->Chassis_Y_Integ>0 && (vt13->key_S-vt13->key_W)>=0 )    //有前进期望速度且期望能减速
	 {
			vt13->Chassis_Y_Integ-=ACE_SHACHE*160*pow(vt13->Chassis_Y_Integ,2);  //Ace_Sense为加速度灵敏度
			if(vt13->Chassis_Y_Integ<0.2)  vt13->Chassis_Y_Integ=0;	
	 }
	 else if(vt13->Chassis_Y_Integ<0 && (vt13->key_S-vt13->key_W)<=0 )    //有前进期望速度且期望能减速
	 {
			vt13->Chassis_Y_Integ+=ACE_SHACHE*160*pow(vt13->Chassis_Y_Integ,2);  //Ace_Sense为加速度灵敏度
			if(vt13->Chassis_Y_Integ>-0.2)  vt13->Chassis_Y_Integ=0;	
	 }
	 else              //加速
	 { 
				 
		 vt13->Chassis_Y_Integ +=ACE_SENSE*0.3f*(vt13->key_W - vt13->key_S);//正常行驶  //Shoot_Info.bullet_cap_status=1;//开弹仓 							 

		 if(vt13->Chassis_Y_Integ>3)          vt13->Chassis_Y_Integ=3;
		 else if (vt13->Chassis_Y_Integ<-3)   vt13->Chassis_Y_Integ=-3;									 
	 }
	 
	 /*********左右加速减速处理*********/	 
			 if(vt13->Chassis_X_Integ>0 && (vt13->key_A-vt13->key_D)>=0 )    //有右进期望速度且期望能减速
	 {
			vt13->Chassis_X_Integ -=ACE_SHACHE*160*pow(vt13->Chassis_X_Integ,2);  //Ace_Sense为加速度灵敏度
			if(vt13->Chassis_X_Integ<0.2)  vt13->Chassis_X_Integ=0;	
	 }
	 else if(vt13->Chassis_X_Integ<0 && (vt13->key_A-vt13->key_D)<=0 )    //有前进期望速度且期望能减速
	 {
			vt13->Chassis_X_Integ +=ACE_SHACHE*160*pow(vt13->Chassis_X_Integ,2);  //Ace_Sense为加速度灵敏度
			if(vt13->Chassis_X_Integ>-0.2)  vt13->Chassis_X_Integ=0;	
	 }
	 else              //加速
	 { 
				 
		 vt13->Chassis_X_Integ +=ACE_SENSE*0.3f*(vt13->key_D - vt13->key_A);//正常行驶  //Shoot_Info.bullet_cap_status=1;//开弹仓 							 

		 if(vt13->Chassis_X_Integ>3)          vt13->Chassis_X_Integ=3;
		 else if (vt13->Chassis_X_Integ<-3)   vt13->Chassis_X_Integ=-3;									 
	 }
		
	 
/*--------------------------软件复位---------------------------------------*/	

		if(vt13->key_Q_flag_vT13==1 && vt13->key_V_flag_vT13==1)	
		{
			vt13->RestFlag = 1; 
		}
		else{
			vt13->RestFlag = 0;
		}
	}
}