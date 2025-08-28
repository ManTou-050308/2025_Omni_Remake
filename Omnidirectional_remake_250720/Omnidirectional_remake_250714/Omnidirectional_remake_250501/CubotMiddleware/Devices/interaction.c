#include "interaction.h"
#include "string.h"
#include "usart.h"
#include "brain.h"
#include "referee.h"
#include "driver_timer.h"
#include "hardware_config.h"
#include "control_logic.h"

#include "dr16.h"
#include "Supercap.h"
#include "omnichassis.h"
#include "load.h"


/*


*/
 
uint8_t Data_Pack[128];
uint8_t UI_state = 1;

UI_t UI;
control_UI_ALL control_UI_all;

void UI_show(void)
{
	
	if(Rc_Ctrl.isOnline == 1)
		tim14.UI_Time++;
	else
		tim14.UI_Time = 0;

	control_UI_all.VOLT = Super_Cap.cap_state.Voltage;
	control_UI_all.control_aiming = Vision_Info.Hit_Mode;
	control_UI_all.control_roll = OmniChassis.Flag.RollingFlag;
	control_UI_all.pitch_data = Holder.Motors6020.motor[Pitch_6020].Data.Angle;
	control_UI_all.Cap_State = Super_Cap.cap_state.Supercap_Charge_mode_Real;

	if(Holder.Motors6020.motor[Yaw_6020].Data.Angle>=0)
		control_UI_all.state_angle = Holder.Motors6020.motor[Yaw_6020].Data.Angle;
	else
		control_UI_all.state_angle = Holder.Motors6020.motor[Yaw_6020].Data.Angle+360;
	
	control_UI_all.level = referee2024.robot_status_t.robot_level;
	
	
		control_UI_all.level = referee2024.robot_status_t.robot_level;
	
	
	control_UI_all.Bullet_Num = referee2024.projectile_allowance_t.projectile_allowance_17mm;
	if(control_UI_all.Bullet_Num == control_UI_all.Bullet_Num_Last && Loader.BulletDisk.motor2006.Data.SpeedRPM >= 2000)
		control_UI_all.bullet_state = 3;
	control_UI_all.Bullet_Num_Cnt++;
	if(control_UI_all.Bullet_Num_Cnt >=150)
	{
		control_UI_all.Bullet_Num_Cnt = 0;
		control_UI_all.Bullet_Num = control_UI_all.Bullet_Num_Last;
	}
	
	if(referee2024.projectile_allowance_t.projectile_allowance_17mm >= 1000)
		control_UI_all.bullet_state = 0;
	else if(referee2024.projectile_allowance_t.projectile_allowance_17mm <=50)
		control_UI_all.bullet_state = 1;
	else
		control_UI_all.bullet_state = 2;

	
	if(UI_state==1)
	{
		referee_draw_basic(referee2024.robot_status_t.robot_id,tim14.UI_Time);
		referee_draw_patterning_all_1(referee2024.robot_status_t.robot_id,tim14.UI_Time);
	}
	
	else
	{
		referee_draw_modify_basic(referee2024.robot_status_t.robot_id,tim14.UI_Time);
		referee_draw_modify(referee2024.robot_status_t.robot_id,tim14.UI_Time);
	}

}


void referee_draw_basic(uint8_t robot_id,uint32_t cnt)
{
	char* level="level";
	char* Roll="Roll";
	char* Home="Home";
	if(cnt%180==0&&cnt<4800&&cnt!=0)
//		referee_draw_basic_1(robot_id);
			referee_draw_string(robot_id,Home,9,ADD,WHITE,5,350,665);
	if(cnt%180==30&&cnt<4800&&cnt!=0)
		referee_draw_basic_2(robot_id);
	if(cnt%180==60&&cnt<4800&&cnt!=0)
		referee_draw_string(robot_id,level,10,ADD,WHITE,2,850,800);
	if(cnt%180==90&&cnt<4800&&cnt!=0)
		referee_draw_string(robot_id,Roll,11,ADD,WHITE,5,1490,665);
//  if(cnt%180==120&&cnt<4800&&cnt!=0)
//		referee_draw_string(robot_id,Home,11,ADD,WHITE,5,350,665);
}

void referee_draw_patterning_all_1(uint8_t robot_id,uint32_t cnt)
{

	if(cnt%180==120&&cnt<4800&&cnt!=0)
		referee_draw_patterning_1_basic(robot_id);
	if(cnt%180==150&&cnt<4800&&cnt!=0)
		referee_draw_patterning_2_basic(robot_id);
	if(cnt%60==0&&cnt>=4800)
		referee_draw_patterning_1(robot_id);
	if(cnt%60==30&&cnt>=4800)
		referee_draw_patterning_2(robot_id);
}

/*-------------------------------------------------------------------------------------------------------------------*/
void referee_draw_modify_basic(uint8_t robot_id,uint32_t cnt)
{
	char* level="level";

	if(cnt%150==0&&cnt<3000&&cnt!=0)
			referee_draw_basic_1(robot_id);
	if(cnt%150==30&&cnt<3000&&cnt!=0)
			referee_draw_basic_2(robot_id);
	if(cnt%150==60&&cnt<3000&&cnt!=0) 
		referee_draw_string(robot_id,level,10,ADD,WHITE,2,850,800);
	if(cnt%150==90&&cnt<3000&&cnt!=0)
		referee_draw_patterning_1_basic(robot_id);
	if(cnt%150==120&&cnt<3000&&cnt!=0)
		referee_draw_patterning_2_basic(robot_id);
	
}

void referee_draw_modify(uint8_t robot_id,uint32_t cnt)
{
	char* level="level";

	if(cnt%150==0&&cnt>=3000&&cnt!=0)
			referee_draw_basic_1_modify(robot_id);
	if(cnt%150==30&&cnt>=3000&&cnt!=0)
			referee_draw_basic_2_modify(robot_id);
	if(cnt%150==60&&cnt>=3000&&cnt!=0)
		referee_draw_string(robot_id,level,10,ADD,WHITE,2,850,800);
	if(cnt%150==90&&cnt>=3000&&cnt!=0)
		referee_draw_patterning_1_basic_modify(robot_id);
	if(cnt%150==120&&cnt>=3000&&cnt!=0)
		referee_draw_patterning_2_basic_modify(robot_id);

}

/*-------------------------------------------------------------------------------------------------------------------*/

void referee_draw_string(uint8_t robot_id,char *string,uint8_t string_dex,uint8_t control_way,uint8_t color,uint8_t on_layer, uint16_t x,uint16_t y)
{
	uint16_t crc16_temp;
	UI.ext_client_custom_character.frame_header.SOF = 0xA5;
	UI.ext_client_custom_character.frame_header.data_length = 51;
	UI.ext_client_custom_character.frame_header.seq = 0;
	UI.ext_client_custom_character.frame_header.CRC8 = Get_CRC8_Check((unsigned char*)&UI.ext_client_custom_character.frame_header,4,0xFF); 
	UI.ext_client_custom_character.cmd_id = 0x0301;
	UI.ext_client_custom_character.ext_student_interactive_header_data.data_cmd_id = 0x0110;
	UI.ext_client_custom_character.ext_student_interactive_header_data.sender_ID = robot_id;
	UI.ext_client_custom_character.ext_student_interactive_header_data.receiver_ID = referee_get_receiver_ID(robot_id);
	UI.ext_client_custom_character.grapic_data_struct.graphic_name[0]=string_dex;
	UI.ext_client_custom_character.grapic_data_struct.graphic_name[1]=0x0;
	UI.ext_client_custom_character.grapic_data_struct.graphic_name[2]=0x0;
	UI.ext_client_custom_character.grapic_data_struct.operate_tpye = control_way;
	UI.ext_client_custom_character.grapic_data_struct.graphic_tpye = 0x07;         //< 表明字符型
	UI.ext_client_custom_character.grapic_data_struct.layer = on_layer;
	UI.ext_client_custom_character.grapic_data_struct.color = color;
	UI.ext_client_custom_character.grapic_data_struct.start_angle = 18;
	UI.ext_client_custom_character.grapic_data_struct.end_angle = 20;
	UI.ext_client_custom_character.grapic_data_struct.width = 2;
	UI.ext_client_custom_character.grapic_data_struct.start_x = x;
	UI.ext_client_custom_character.grapic_data_struct.start_y = y;
	UI.ext_client_custom_character.grapic_data_struct.radius = 0;
	UI.ext_client_custom_character.grapic_data_struct.end_x = 0;
	UI.ext_client_custom_character.grapic_data_struct.end_y = 0;
	memset(UI.ext_client_custom_character.data, 0, 30);					//< 每次绘制前清除上次字符串缓存
	strcpy((char *)UI.ext_client_custom_character.data, string);
	
	crc16_temp = Get_CRC16_Check((unsigned char*)&UI.ext_client_custom_character, 58, 0xFFFF);
	UI.ext_client_custom_character.CRC16[0] = crc16_temp & 0xFF;
	UI.ext_client_custom_character.CRC16[1] = crc16_temp >> 8;
	HAL_UART_Transmit_DMA(&huart3,(unsigned char*)&UI.ext_client_custom_character,60);
}


void referee_draw_string_modify(uint8_t robot_id,char *string,graphic_string_t *patterning)
{

	uint16_t crc16_temp;
		
	(*patterning).frame_header.SOF = 0xA5;
	(*patterning).frame_header.data_length = 51; 
	(*patterning).frame_header.seq = 2;
	(*patterning).frame_header.CRC8 = Get_CRC8_Check((unsigned char*)&(*patterning).frame_header,4,0xFF); 
	(*patterning).cmd_id = 0x0301;
	(*patterning).ext_student_interactive_header_data.data_cmd_id = 0x0110;
	(*patterning).ext_student_interactive_header_data.sender_ID = robot_id;
	(*patterning).ext_student_interactive_header_data.receiver_ID = referee_get_receiver_ID(robot_id);

	(*patterning).grapic_data_struct.operate_tpye =MODIFY;

	memset((*patterning).data, 0, 30);					//< ??????????????
	strcpy((char *)(*patterning).data, string);
	
	crc16_temp = Get_CRC16_Check((unsigned char*)&(*patterning), 58, 0xFFFF);
	(*patterning).CRC16[0] = crc16_temp & 0xFF;
	(*patterning).CRC16[1] = crc16_temp >> 8;
	HAL_UART_Transmit_DMA(&huart3,(unsigned char*)&(*patterning),60);

}

/*??????(????,???)*/
void referee_graphic_delete(uint8_t del_layer, uint8_t operation, uint8_t robot_id) 
{
	uint16_t crc16_temp;
	UI.ext_client_graphic_delete.frame_header.SOF = 0xA5;
	UI.ext_client_graphic_delete.frame_header.data_length = 8;   //< sizeof(ext_student_interactive_header_data_t) + sizeof(graphic_data_struct_t)
	UI.ext_client_graphic_delete.frame_header.seq = 0;
	UI.ext_client_graphic_delete.frame_header.CRC8 = Get_CRC8_Check((unsigned char*)&UI.ext_client_graphic_delete.frame_header,4,0xFF); 
	UI.ext_client_graphic_delete.cmd_id = 0x0301;
	UI.ext_client_graphic_delete.ext_student_interactive_header_data.data_cmd_id = 0x0100;
	UI.ext_client_graphic_delete.ext_student_interactive_header_data.sender_ID = robot_id;
	UI.ext_client_graphic_delete.ext_student_interactive_header_data.receiver_ID = referee_get_receiver_ID(robot_id);
	UI.ext_client_graphic_delete.ext_client_custom_graphic_delete.operate_tpye=operation;
	UI.ext_client_graphic_delete.ext_client_custom_graphic_delete.layer=del_layer;
	
	crc16_temp = Get_CRC16_Check((unsigned char*)&UI.ext_client_graphic_delete, 15, 0xFFFF);
	UI.ext_client_graphic_delete.CRC16[0] = crc16_temp & 0xFF;
	UI.ext_client_graphic_delete.CRC16[1] = crc16_temp >> 8;
	HAL_UART_Transmit_DMA(&huart3,(unsigned char*)&UI.ext_client_graphic_delete,17);
}


/**
  * @brief  ???????????
	*/
void draw_graph(graphic_data_struct_t *patterning, uint16_t index, uint8_t control_way, uint8_t graph, uint8_t layer, uint8_t color, uint32_t Sa, uint32_t Ea, uint8_t With,  uint16_t x,uint16_t y, uint8_t R, uint16_t Ex, uint16_t Ey)
{
	(*patterning).graphic_name[0]=index;
	(*patterning).graphic_name[1]=0;
	(*patterning).graphic_name[2]=0;
	(*patterning).operate_tpye=control_way;
	(*patterning).graphic_tpye=graph;
	(*patterning).layer=layer;
	(*patterning).color=color;
	(*patterning).start_angle=Sa;
	(*patterning).end_angle=Ea;
	(*patterning).width=With;
	(*patterning).start_x=x;
	(*patterning).start_y=y;
	(*patterning).radius=R;
	(*patterning).end_x=Ex;
	(*patterning).end_y=Ey;
}

void draw_graph_modify(graphic_data_struct_t *patterning)
{
	
	(*patterning).operate_tpye=MODIFY;

}



void referee_draw_basic_1(uint8_t robot_id)
{
	uint16_t crc16_temp;
	UI.referee_draw_basic_1_struct.frame_header.SOF = 0xA5;
	UI.referee_draw_basic_1_struct.frame_header.data_length = 111;
	UI.referee_draw_basic_1_struct.frame_header.seq = 0;
	UI.referee_draw_basic_1_struct.frame_header.CRC8 = Get_CRC8_Check((unsigned char*)&UI.referee_draw_basic_1_struct.frame_header,4,0xFF); 
	UI.referee_draw_basic_1_struct.cmd_id = 0x0301;
	UI.referee_draw_basic_1_struct.ext_student_interactive_header_data.data_cmd_id = 0x0104;
	UI.referee_draw_basic_1_struct.ext_student_interactive_header_data.sender_ID = robot_id;
	UI.referee_draw_basic_1_struct.ext_student_interactive_header_data.receiver_ID = referee_get_receiver_ID(robot_id);
	
	draw_graph(&UI.referee_draw_basic_1_struct.grapic_data_struct[0],90,ADD,RECTANGLE,2,WHITE,0,0,2,10,860,0,600,890);
//	draw_graph(&UI.referee_draw_basic_1_struct.grapic_data_struct[1],91,ADD,INT,2,YELLOW,15,0,2,(10+(4-LowVolt)*590.0/(HighVolt-LowVolt)),850,4,0,0);
//	draw_graph(&UI.referee_draw_basic_1_struct.grapic_data_struct[2],92,ADD,INT,2,YELLOW,15,0,2,(10+(9-LowVolt)*590.0/(HighVolt-LowVolt)),850,9,0,0);
//	draw_graph(&UI.referee_draw_basic_1_struct.grapic_data_struct[3],93,ADD,INT,2,YELLOW,15,0,2,(10+(14-LowVolt)*590.0/(HighVolt-LowVolt)),850,14,0,0);
//	draw_graph(&UI.referee_draw_basic_1_struct.grapic_data_struct[4],94,ADD,INT,2,YELLOW,15,0,2,(10+(19-LowVolt)*590.0/(HighVolt-LowVolt)),850,19,0,0);
//	draw_graph(&UI.referee_draw_basic_1_struct.grapic_data_struct[5],95,ADD,CIRCLE,2,FUCHSIA,0,0,8,600,220,60,0,0);	//表示底盘
//	draw_graph(&UI.referee_draw_basic_1_struct.grapic_data_struct[6],96,ADD,LINE,5,ORANGE,0,0,8,600,220,0,600,320);  //表示云台
 	
	
	
	crc16_temp = Get_CRC16_Check((unsigned char*)&UI.referee_draw_basic_1_struct, 118, 0xFFFF);
	UI.referee_draw_basic_1_struct.CRC16[0] = crc16_temp & 0xFF;
	UI.referee_draw_basic_1_struct.CRC16[1] = crc16_temp >> 8;
	
	memcpy(Data_Pack,(unsigned char*)&UI.referee_draw_basic_1_struct,sizeof(UI.referee_draw_basic_1_struct));
	
	HAL_UART_Transmit_DMA(&huart3,Data_Pack,120);
}

void referee_draw_basic_1_modify(uint8_t robot_id)
{
	uint16_t crc16_temp;
	UI.referee_draw_basic_1_struct.frame_header.SOF = 0xA5;
	UI.referee_draw_basic_1_struct.frame_header.data_length = 111;
	UI.referee_draw_basic_1_struct.frame_header.seq = 0;
	UI.referee_draw_basic_1_struct.frame_header.CRC8 = Get_CRC8_Check((unsigned char*)&UI.referee_draw_basic_1_struct.frame_header,4,0xFF); 
	UI.referee_draw_basic_1_struct.cmd_id = 0x0301;
	UI.referee_draw_basic_1_struct.ext_student_interactive_header_data.data_cmd_id = 0x0104;
	UI.referee_draw_basic_1_struct.ext_student_interactive_header_data.sender_ID = robot_id;
	UI.referee_draw_basic_1_struct.ext_student_interactive_header_data.receiver_ID = referee_get_receiver_ID(robot_id);

	draw_graph_modify(&UI.referee_draw_basic_1_struct.grapic_data_struct[0]);
	draw_graph_modify(&UI.referee_draw_basic_1_struct.grapic_data_struct[1]);
	draw_graph_modify(&UI.referee_draw_basic_1_struct.grapic_data_struct[2]);
	draw_graph_modify(&UI.referee_draw_basic_1_struct.grapic_data_struct[3]);
	draw_graph_modify(&UI.referee_draw_basic_1_struct.grapic_data_struct[4]);
	draw_graph_modify(&UI.referee_draw_basic_1_struct.grapic_data_struct[5]);//表示底盘
	draw_graph_modify(&UI.referee_draw_basic_1_struct.grapic_data_struct[6]);//表示云台
// 	
//	
	
	crc16_temp = Get_CRC16_Check((unsigned char*)&UI.referee_draw_basic_1_struct, 118, 0xFFFF);
	UI.referee_draw_basic_1_struct.CRC16[0] = crc16_temp & 0xFF;
	UI.referee_draw_basic_1_struct.CRC16[1] = crc16_temp >> 8;
	
	memcpy(Data_Pack,(unsigned char*)&UI.referee_draw_basic_1_struct,sizeof(UI.referee_draw_basic_1_struct));
	
	HAL_UART_Transmit_DMA(&huart3,Data_Pack,120);
}


void referee_draw_basic_2(uint8_t robot_id)
{
	uint16_t crc16_temp;
	UI.referee_draw_basic_2_struct.frame_header.SOF = 0xA5;
	UI.referee_draw_basic_2_struct.frame_header.data_length =  111;
	UI.referee_draw_basic_2_struct.frame_header.seq = 1;
	UI.referee_draw_basic_2_struct.frame_header.CRC8 = Get_CRC8_Check((unsigned char*)&UI.referee_draw_basic_2_struct.frame_header,4,0xFF); 
	UI.referee_draw_basic_2_struct.cmd_id = 0x0301;
	UI.referee_draw_basic_2_struct.ext_student_interactive_header_data.data_cmd_id = 0x0104;
	UI.referee_draw_basic_2_struct.ext_student_interactive_header_data.sender_ID = robot_id;
	UI.referee_draw_basic_2_struct.ext_student_interactive_header_data.receiver_ID = referee_get_receiver_ID(robot_id);

	draw_graph(&UI.referee_draw_basic_2_struct.grapic_data_struct[0],80,ADD,INT,2,WHITE,15,0,2,(10+(24-LowVolt)*590.0/(HighVolt-LowVolt)),550,24,0,0);
//	draw_graph(&UI.referee_draw_basic_2_struct.grapic_data_struct[1],81,ADD,CIRCLE,2,GREEN,0,0,2,943,467,3,0,0);//准星


	crc16_temp = Get_CRC16_Check((unsigned char*)&UI.referee_draw_basic_2_struct, 118, 0xFFFF);
	UI.referee_draw_basic_2_struct.CRC16[0] = crc16_temp & 0xFF;
	UI.referee_draw_basic_2_struct.CRC16[1] = crc16_temp >> 8;
	
	memcpy(Data_Pack,(unsigned char*)&UI.referee_draw_basic_2_struct,sizeof(UI.referee_draw_basic_2_struct));
	
	HAL_UART_Transmit_DMA(&huart3,Data_Pack,120);
}

void referee_draw_basic_2_modify(uint8_t robot_id)
{
	uint16_t crc16_temp;
	UI.referee_draw_basic_2_struct.frame_header.SOF = 0xA5;
	UI.referee_draw_basic_2_struct.frame_header.data_length = 111;
	UI.referee_draw_basic_2_struct.frame_header.seq = 0;
	UI.referee_draw_basic_2_struct.frame_header.CRC8 = Get_CRC8_Check((unsigned char*)&UI.referee_draw_basic_2_struct.frame_header,4,0xFF); 
	UI.referee_draw_basic_2_struct.cmd_id = 0x0301;
	UI.referee_draw_basic_2_struct.ext_student_interactive_header_data.data_cmd_id = 0x0104;
	UI.referee_draw_basic_2_struct.ext_student_interactive_header_data.sender_ID = robot_id;
	UI.referee_draw_basic_2_struct.ext_student_interactive_header_data.receiver_ID = referee_get_receiver_ID(robot_id);
	
	draw_graph_modify(&UI.referee_draw_basic_2_struct.grapic_data_struct[0]);	
	draw_graph_modify(&UI.referee_draw_basic_2_struct.grapic_data_struct[1]);
	draw_graph_modify(&UI.referee_draw_basic_2_struct.grapic_data_struct[2]);
	draw_graph_modify(&UI.referee_draw_basic_2_struct.grapic_data_struct[3]);
	draw_graph_modify(&UI.referee_draw_basic_2_struct.grapic_data_struct[4]);
	draw_graph_modify(&UI.referee_draw_basic_2_struct.grapic_data_struct[5]);
	draw_graph_modify(&UI.referee_draw_basic_2_struct.grapic_data_struct[6]);
 	
	
	
	crc16_temp = Get_CRC16_Check((unsigned char*)&UI.referee_draw_basic_2_struct, 118, 0xFFFF);
	UI.referee_draw_basic_2_struct.CRC16[0] = crc16_temp & 0xFF;
	UI.referee_draw_basic_2_struct.CRC16[1] = crc16_temp >> 8;
	
	memcpy(Data_Pack,(unsigned char*)&UI.referee_draw_basic_2_struct,sizeof(UI.referee_draw_basic_2_struct));
	
	HAL_UART_Transmit_DMA(&huart3,Data_Pack,120);
}

void referee_draw_patterning_1(uint8_t robot_id)
{
	float angle_start;
	float angle_end;
	
	uint16_t crc16_temp;
	UI.referee_draw_patterning_1_struct.frame_header.SOF = 0xA5;
	UI.referee_draw_patterning_1_struct.frame_header.data_length =  111;
	UI.referee_draw_patterning_1_struct.frame_header.seq = 3;
	UI.referee_draw_patterning_1_struct.frame_header.CRC8 = Get_CRC8_Check((unsigned char*)&UI.referee_draw_patterning_1_struct.frame_header,4,0xFF); 
	UI.referee_draw_patterning_1_struct.cmd_id = 0x0301;
	UI.referee_draw_patterning_1_struct.ext_student_interactive_header_data.data_cmd_id = 0x0104;
	UI.referee_draw_patterning_1_struct.ext_student_interactive_header_data.sender_ID = robot_id;
	UI.referee_draw_patterning_1_struct.ext_student_interactive_header_data.receiver_ID = referee_get_receiver_ID(robot_id);
	
	
//	if(control_UI_all.bullet_state == 2)
//	{
//		draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[0],70,MODIFY,RECTANGLE,4,WHITE,0,0,3,1300,300,0,620,890);
//		draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[2],72,MODIFY,CIRCLE,5,WHITE,0,0,3,955,480,8,0,0);
//	}
//	else if(control_UI_all.bullet_state == 1)
//	{
//		draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[0],70,MODIFY,RECTANGLE,4,WHITE,0,0,6,1300,300,0,620,890);
//		draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[2],72,MODIFY,CIRCLE,5,WHITE,0,0,3,955,480,8,0,0);
//	}
//	else if(control_UI_all.bullet_state == 0)
//	{
//		draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[0],70,MODIFY,RECTANGLE,4,WHITE,0,0,6,1300,300,0,620,890);
//		draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[2],72,MODIFY,CIRCLE,5,WHITE,0,0,3,955,480,8,0,0);
//	}
//	else if(control_UI_all.bullet_state == 3)
//	{
//		draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[0],70,MODIFY,RECTANGLE,4,WHITE,0,0,6,1300,300,0,620,890);
//		draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[2],72,MODIFY,CIRCLE,5,WHITE,0,0,3,955,480,8,0,0);
//}
	
	//电容当前能量
	if(control_UI_all.Cap_State == 1)
		draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[1],71,MODIFY,LINE,4,WHITE,0,0,30,10,875,0,(10+(control_UI_all.VOLT-LowVolt)*590.0f/(HighVolt-LowVolt)),875);
	else if(control_UI_all.Cap_State == 0)
		draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[1],71,MODIFY,LINE,4,CYAN_BLUE,0,0,30,10,875,0,(10+(control_UI_all.VOLT-LowVolt)*590.0f/(HighVolt-LowVolt)),875);
	
	//自瞄
	if(control_UI_all.control_aiming!=0)
	{
		draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[3],73,MODIFY,LINE,4,WHITE,0,0,1,660,320,0,660,760);
		draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[4],74,MODIFY,LINE,4,WHITE,0,0,1,1260,320,0,1260,760);
	}
	else
	{
		draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[3],73,MODIFY,LINE,4,WHITE,0,0,1,660,320,0,660,320);
		draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[4],74,MODIFY,LINE,4,WHITE,0,0,1,1260,320,0,1260,320);
	}
	
	
	if(control_UI_all.control_roll==1)//自旋	
		draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[5],75,MODIFY,RECTANGLE,5,CYAN_BLUE,0,0,4,1460,680,0,1600,635);
	else if(control_UI_all.control_roll==0)
		draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[5],75,MODIFY,RECTANGLE,5,CYAN_BLUE,0,0,4,1460,680,0,1460,680);
	
	if(control_UI_all.state_angle-30<0)
		angle_start=control_UI_all.state_angle+330;
	else
		angle_start=control_UI_all.state_angle-30;
	if(control_UI_all.state_angle+30>360)
		angle_end=control_UI_all.state_angle-330;
	else
		angle_end=control_UI_all.state_angle+30;

//	draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[6],76,MODIFY,ARC,3,YELLOW,angle_start,angle_end,8,600,220,0,60,60);						

	
	crc16_temp = Get_CRC16_Check((unsigned char*)&UI.referee_draw_patterning_1_struct, 118, 0xFFFF);
	UI.referee_draw_patterning_1_struct.CRC16[0] = crc16_temp & 0xFF;
	UI.referee_draw_patterning_1_struct.CRC16[1] = crc16_temp >> 8;
	
 	memcpy(Data_Pack,(unsigned char*)&UI.referee_draw_patterning_1_struct,sizeof(UI.referee_draw_patterning_1_struct));
	
	HAL_UART_Transmit_DMA(&huart3,Data_Pack,120);
}

void referee_draw_patterning_1_basic(uint8_t robot_id)
{
	
	uint16_t crc16_temp;
	UI.referee_draw_patterning_1_struct.frame_header.SOF = 0xA5;
	UI.referee_draw_patterning_1_struct.frame_header.data_length =  111;
	UI.referee_draw_patterning_1_struct.frame_header.seq = 3;
	UI.referee_draw_patterning_1_struct.frame_header.CRC8 = Get_CRC8_Check((unsigned char*)&UI.referee_draw_patterning_1_struct.frame_header,4,0xFF); 
	UI.referee_draw_patterning_1_struct.cmd_id = 0x0301;
	UI.referee_draw_patterning_1_struct.ext_student_interactive_header_data.data_cmd_id = 0x0104;
	UI.referee_draw_patterning_1_struct.ext_student_interactive_header_data.sender_ID = robot_id;
	UI.referee_draw_patterning_1_struct.ext_student_interactive_header_data.receiver_ID = referee_get_receiver_ID(robot_id);
	

//draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[0],70,ADD,RECTANGLE,4,WHITE,0,0,3,1300,300,0,620,890);
	
	draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[1],71,ADD,LINE,4,0,0,0,30,10,875,0,11,875);     //?????
	//当前电容容量
	draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[2],72,ADD,CIRCLE,5,WHITE,0,0,3,930,500,8,0,0);

	draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[3],73,ADD,LINE,4,WHITE,0,0,3,660,320,0,660,320);
	
	draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[4],74,ADD,LINE,4,WHITE,0,0,3,1260,320,0,1260,320);

	draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[5],75,ADD,RECTANGLE,5,CYAN_BLUE,0,0,4,1460,680,0,1600,635);
	//自旋
//	draw_graph(&UI.referee_draw_patterning_1_struct.grapic_data_struct[6],76,ADD,ARC,3,WHITE,330,30,8,600,220,0,60,60);						
//相对位姿
	
	
	crc16_temp = Get_CRC16_Check((unsigned char*)&UI.referee_draw_patterning_1_struct, 118, 0xFFFF);
	UI.referee_draw_patterning_1_struct.CRC16[0] = crc16_temp & 0xFF;
	UI.referee_draw_patterning_1_struct.CRC16[1] = crc16_temp >> 8;
	
	memcpy(Data_Pack,(unsigned char*)&UI.referee_draw_patterning_1_struct,sizeof(UI.referee_draw_patterning_1_struct));
	
	HAL_UART_Transmit_DMA(&huart3,Data_Pack,120);
}

void referee_draw_patterning_1_basic_modify(uint8_t robot_id)
{

	uint16_t crc16_temp;
	UI.referee_draw_patterning_1_struct.frame_header.SOF = 0xA5;
	UI.referee_draw_patterning_1_struct.frame_header.data_length =  111;
	UI.referee_draw_patterning_1_struct.frame_header.seq = 3;
	UI.referee_draw_patterning_1_struct.frame_header.CRC8 = Get_CRC8_Check((unsigned char*)&UI.referee_draw_patterning_1_struct.frame_header,4,0xFF); 
	UI.referee_draw_patterning_1_struct.cmd_id = 0x0301;
	UI.referee_draw_patterning_1_struct.ext_student_interactive_header_data.data_cmd_id = 0x0104;
	UI.referee_draw_patterning_1_struct.ext_student_interactive_header_data.sender_ID = robot_id;
	UI.referee_draw_patterning_1_struct.ext_student_interactive_header_data.receiver_ID = referee_get_receiver_ID(robot_id);


	draw_graph_modify(&UI.referee_draw_patterning_1_struct.grapic_data_struct[0]);
	draw_graph_modify(&UI.referee_draw_patterning_1_struct.grapic_data_struct[1]);
	draw_graph_modify(&UI.referee_draw_patterning_1_struct.grapic_data_struct[2]);
	draw_graph_modify(&UI.referee_draw_patterning_1_struct.grapic_data_struct[3]);
	draw_graph_modify(&UI.referee_draw_patterning_1_struct.grapic_data_struct[4]);
	draw_graph_modify(&UI.referee_draw_patterning_1_struct.grapic_data_struct[5]);
	draw_graph_modify(&UI.referee_draw_patterning_1_struct.grapic_data_struct[6]);

	
	
	crc16_temp = Get_CRC16_Check((unsigned char*)&UI.referee_draw_patterning_1_struct, 118, 0xFFFF);
	UI.referee_draw_patterning_1_struct.CRC16[0] = crc16_temp & 0xFF;
	UI.referee_draw_patterning_1_struct.CRC16[1] = crc16_temp >> 8;
	
	memcpy(Data_Pack,(unsigned char*)&UI.referee_draw_patterning_1_struct,sizeof(UI.referee_draw_patterning_1_struct));
	
	HAL_UART_Transmit_DMA(&huart3,Data_Pack,120);
}

void referee_draw_patterning_2_basic(uint8_t robot_id)
{

	uint16_t crc16_temp;

	UI.referee_draw_patterning_2_struct.frame_header.SOF = 0xA5;
	UI.referee_draw_patterning_2_struct.frame_header.data_length =  111;
	UI.referee_draw_patterning_2_struct.frame_header.seq = 0;
	UI.referee_draw_patterning_2_struct.frame_header.CRC8 = Get_CRC8_Check((unsigned char*)&UI.referee_draw_patterning_2_struct.frame_header,4,0xFF); 
	UI.referee_draw_patterning_2_struct.cmd_id = 0x0301;
	UI.referee_draw_patterning_2_struct.ext_student_interactive_header_data.data_cmd_id = 0x0104;
	UI.referee_draw_patterning_2_struct.ext_student_interactive_header_data.sender_ID = robot_id;
	UI.referee_draw_patterning_2_struct.ext_student_interactive_header_data.receiver_ID = referee_get_receiver_ID(robot_id);

//	draw_graph(&UI.referee_draw_patterning_2_struct.grapic_data_struct[0],20,ADD,CIRCLE,4,WHITE,0,0,3,1350,870,40,0,0);
	draw_graph(&UI.referee_draw_patterning_2_struct.grapic_data_struct[2],22,ADD,INT,2,WHITE,20,0,2,970,802,control_UI_all.level,0,0);  
	//level
	
//	draw_graph(&UI.referee_draw_patterning_2_struct.grapic_data_struct[3],23,ADD,LINE,4,FUCHSIA,0,0,3,955,545,0,965,535);
//	draw_graph(&UI.referee_draw_patterning_2_struct.grapic_data_struct[4],24,ADD,LINE,4,FUCHSIA,0,0,3,955,535,0,965,545);
	
//	draw_graph(&UI.referee_draw_patterning_2_struct.grapic_data_struct[5],25,ADD,INT,5,YELLOW,30,0,4,1720,745,(control_UI_all.Chassis_Speed)&0x3f,(control_UI_all.Chassis_Speed>>10)&0x7f,(control_UI_all.Chassis_Speed>>21)&0x7f);

	
	crc16_temp = Get_CRC16_Check((unsigned char*)&UI.referee_draw_patterning_2_struct, 118, 0xFFFF);
	UI.referee_draw_patterning_2_struct.CRC16[0] = crc16_temp & 0xFF;
	UI.referee_draw_patterning_2_struct.CRC16[1] = crc16_temp >> 8;
	
	memcpy(Data_Pack,(unsigned char*)&UI.referee_draw_patterning_2_struct,sizeof(UI.referee_draw_patterning_2_struct));
	
	HAL_UART_Transmit_DMA(&huart3,Data_Pack,120);
}

void referee_draw_patterning_2_basic_modify(uint8_t robot_id)
{

	uint16_t crc16_temp;

	UI.referee_draw_patterning_2_struct.frame_header.SOF = 0xA5;
	UI.referee_draw_patterning_2_struct.frame_header.data_length =  111;
	UI.referee_draw_patterning_2_struct.frame_header.seq = 0;
	UI.referee_draw_patterning_2_struct.frame_header.CRC8 = Get_CRC8_Check((unsigned char*)&UI.referee_draw_patterning_2_struct.frame_header,4,0xFF); 
	UI.referee_draw_patterning_2_struct.cmd_id = 0x0301;
	UI.referee_draw_patterning_2_struct.ext_student_interactive_header_data.data_cmd_id = 0x0104;
	UI.referee_draw_patterning_2_struct.ext_student_interactive_header_data.sender_ID = robot_id;
	UI.referee_draw_patterning_2_struct.ext_student_interactive_header_data.receiver_ID = referee_get_receiver_ID(robot_id);


	draw_graph_modify(&UI.referee_draw_patterning_2_struct.grapic_data_struct[0]);
	draw_graph_modify(&UI.referee_draw_patterning_2_struct.grapic_data_struct[1]);
	draw_graph_modify(&UI.referee_draw_patterning_2_struct.grapic_data_struct[2]);
	draw_graph_modify(&UI.referee_draw_patterning_2_struct.grapic_data_struct[3]);
	draw_graph_modify(&UI.referee_draw_patterning_2_struct.grapic_data_struct[4]);
	draw_graph_modify(&UI.referee_draw_patterning_2_struct.grapic_data_struct[5]);
	draw_graph_modify(&UI.referee_draw_patterning_2_struct.grapic_data_struct[6]);


	crc16_temp = Get_CRC16_Check((unsigned char*)&UI.referee_draw_patterning_2_struct, 118, 0xFFFF);
	UI.referee_draw_patterning_2_struct.CRC16[0] = crc16_temp & 0xFF;
	UI.referee_draw_patterning_2_struct.CRC16[1] = crc16_temp >> 8;
	
	memcpy(Data_Pack,(unsigned char*)&UI.referee_draw_patterning_2_struct,sizeof(UI.referee_draw_patterning_2_struct));
	
	HAL_UART_Transmit_DMA(&huart3,Data_Pack,120);
}

void referee_draw_patterning_2(uint8_t robot_id)
{

	uint16_t crc16_temp;

	UI.referee_draw_patterning_2_struct.frame_header.SOF = 0xA5;
	UI.referee_draw_patterning_2_struct.frame_header.data_length =  111;
	UI.referee_draw_patterning_2_struct.frame_header.seq = 0;
	UI.referee_draw_patterning_2_struct.frame_header.CRC8 = Get_CRC8_Check((unsigned char*)&UI.referee_draw_patterning_2_struct.frame_header,4,0xFF); 
	UI.referee_draw_patterning_2_struct.cmd_id = 0x0301;
	UI.referee_draw_patterning_2_struct.ext_student_interactive_header_data.data_cmd_id = 0x0104;
	UI.referee_draw_patterning_2_struct.ext_student_interactive_header_data.sender_ID = robot_id;
	UI.referee_draw_patterning_2_struct.ext_student_interactive_header_data.receiver_ID = referee_get_receiver_ID(robot_id);


//	if(ModeToBrain == 2)
//		draw_graph(&UI.referee_draw_patterning_2_struct.grapic_data_struct[0],20,MODIFY,RECTANGLE,4,CYAN_BLUE,0,0,3,1325,895,0,1375,845);
//	else if(ModeToBrain == 3)
//		draw_graph(&UI.referee_draw_patterning_2_struct.grapic_data_struct[0],20,MODIFY,RECTANGLE,4,CYAN_BLUE,0,0,6,1325,895,0,1375,845);
//	else if(ModeToBrain == 1)
//		draw_graph(&UI.referee_draw_patterning_2_struct.grapic_data_struct[0],20,MODIFY,CIRCLE,4,PINK,0,0,6,1350,870,40,0,0);
//	else
//		draw_graph(&UI.referee_draw_patterning_2_struct.grapic_data_struct[0],20,MODIFY,CIRCLE,4,WHITE,0,0,3,1350,870,40,0,0);

	
	draw_graph(&UI.referee_draw_patterning_2_struct.grapic_data_struct[2],22,MODIFY,INT,2,WHITE,20,0,2,970,802,control_UI_all.level,0,0);  
	//level
	
//	if(control_UI_all.bullet_state == 0 || control_UI_all.bullet_state == 1)
//	{
//		draw_graph(&UI.referee_draw_patterning_2_struct.grapic_data_struct[3],23,MODIFY,LINE,4,FUCHSIA,0,0,3,955,480,0,965,480);
//		draw_graph(&UI.referee_draw_patterning_2_struct.grapic_data_struct[4],24,MODIFY,LINE,4,FUCHSIA,0,0,3,955,490,0,965,490);
//	}
//	else
//	{
//		draw_graph(&UI.referee_draw_patterning_2_struct.grapic_data_struct[3],23,MODIFY,LINE,4,FUCHSIA,0,0,3,955,490,0,955,490);
//		draw_graph(&UI.referee_draw_patterning_2_struct.grapic_data_struct[4],24,MODIFY,LINE,4,FUCHSIA,0,0,3,955,480,0,955,480);	
//	}
	
//draw_graph(&UI.referee_draw_patterning_2_struct.grapic_data_struct[5],25,MODIFY,INT,5,YELLOW,30,0,4,1720,745,(control_UI_all.Chassis_Speed)&0x3f,(control_UI_all.Chassis_Speed>>10)&0x7f,(control_UI_all.Chassis_Speed>>21)&0x7f);


	crc16_temp = Get_CRC16_Check((unsigned char*)&UI.referee_draw_patterning_2_struct, 118, 0xFFFF);
	UI.referee_draw_patterning_2_struct.CRC16[0] = crc16_temp & 0xFF;
	UI.referee_draw_patterning_2_struct.CRC16[1] = crc16_temp >> 8;
	
	memcpy(Data_Pack,(unsigned char*)&UI.referee_draw_patterning_2_struct,sizeof(UI.referee_draw_patterning_2_struct));
	
	HAL_UART_Transmit_DMA(&huart3,Data_Pack,120);
}


	
/**
  * @brief  ???????
	*/
void update(uint32_t cnt, uint8_t robot_id, uint8_t change)
{
	if(change)
	{
		if(cnt%350==0)
		{
			referee_graphic_delete(0, ALL_delete, robot_id);
			cnt=0;
		}
	}
}


uint16_t referee_get_receiver_ID(uint32_t sender_id)
{
	switch(sender_id)
	{
		case RobotRedHero: return ClientRedHero; 
		case RobotRedEngineer: return ClientRedEngineer;
		case RobotRedInfantryNO3: return ClientRedInfantryNO3;
		case RobotRedInfantryNO4: return ClientRedInfantryNO4; 
		case RobotRedInfantryNO5: return ClientRedInfantryNO5; 
		case RobotRedAerial: return ClientRedAerial;
		case RobotBlueHero: return ClientBlueHero;
		case RobotBlueEngineer: return ClientBlueEngineer; 
		case RobotBlueInfantryNO3: return ClientBlueInfantryNO3;
		case RobotBlueInfantryNO4: return ClientBlueInfantryNO4; 
		case RobotBlueInfantryNO5: return ClientBlueInfantryNO5; 
		case RobotBlueAerial: return ClientBlueAerial; 
		default: return 0;
	}
}




const unsigned char CRC8__INIT = 0xff;
const unsigned char CRC8__TAB[256] =//8???256?
{
	0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
	0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
	0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
	0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
	0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
	0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
	0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
	0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
	0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
	0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
	0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
	0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
	0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
	0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
	0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
	0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,
};
unsigned char Get_CRC8_Check(unsigned char *pchMessage,unsigned int dwLength,unsigned char ucCRC8)
{
	unsigned char ucIndex;                     //?0??????,?1????
	while (dwLength--)   //ucCRC8?????
	{
		ucIndex = ucCRC8^(*pchMessage++);//???????? 
		ucCRC8 = CRC8__TAB[ucIndex];//???             
	}                                                   
	return(ucCRC8);
}

uint16_t CRC__INIT = 0xffff;
const uint16_t wCRC__Table[256] =
{
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

uint16_t Get_CRC16_Check(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC)
{
	uint8_t chData;
	if (pchMessage == NULL)//????
	{
		return 0xFFFF;
	}
	while(dwLength--)
	{
		chData = *pchMessage++;
		(wCRC) = ((uint16_t)(wCRC) >> 8) ^ wCRC__Table[((uint16_t)(wCRC) ^ (uint16_t)(chData)) &0x00ff];
	}
	return wCRC;
}
