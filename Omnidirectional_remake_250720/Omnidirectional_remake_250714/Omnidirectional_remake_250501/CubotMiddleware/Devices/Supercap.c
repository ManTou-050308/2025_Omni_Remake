#include "hardware_config.h"
#include "omnichassis.h"
#include "Supercap.h"
#include "referee.h"
#include "check.h"

#include "fdcan.h"
#include "driver_can.h"

CAN_TxBuffer txBuffer0x6FFforCAN2={
	.Identifier = 0x6ff
};


/*		能量计算参数		*/
float Power_Chassis = 0;
float Last_Power_Chassis = 0;
float Power_Chassis_Static = 0;
float Last_Power_Chassis_Static = 0;
float Powerto10000_Chassis = 0;
uint16_t Power_Limt;
int cnt_static =0 ;
int cnt_move =0 ;
int Last_cnt_static =0 ;
int Last_cnt_move =0 ;


uint8_t Supercap_rxCallBack(CAN_Object canx, CAN_RxBuffer rxBuffer ,SuperCap_t* Cap)
{
	if(rxBuffer.Header.Identifier == 0x601)
	{
		Cap->cap_state.Voltage = ((rxBuffer.Data[0]<<8)+rxBuffer.Data[1])*0.01f;
		Cap->cap_state.Current = ((rxBuffer.Data[2]<<8)+rxBuffer.Data[3]);
		
		Cap->cap_state.Supercap_Charge_mode_Real = rxBuffer.Data[6];//0充1放
		check_robot_state.SuperCap_state = 0;
	}
	return 0;
}

void SupercapControl(CAN_Object can, RC_Ctrl_t* rc_ctrl , SuperCap_t* Cap,Omni_Chassis_t* chassis)
{
	Cap->cap_state.voltage_flag = 1;
	
	if(Cap->cap_state.Voltage < 15.0f)
	{
		Cap->cap_state.voltage_flag=0;
	}
	
	if(Cap->cap_state.Supercap_Mode == 1 && Cap->cap_state.voltage_flag==1)
	{
		txBuffer0x6FFforCAN2.Data[0] = 1;
		Cap->cap_state.Supercap_Flag = 1;
	}
	else
	{
		txBuffer0x6FFforCAN2.Data[0] = 0;//0
		Cap->cap_state.Supercap_Flag = 0;
	}
	if(Cap->cap_state.Supercap_Charge_mode==0)
		Cap->cap_state.Supercap_Charge=0;
	else if (Cap->cap_state.Supercap_Charge_mode==1)
		Cap->cap_state.Supercap_Charge=1;

	if(referee2024.robot_status_t.chassis_power_limit > 100)Power_Limt = 100;
	else Power_Limt = referee2024.robot_status_t.chassis_power_limit;
	
	txBuffer0x6FFforCAN2.Data[1] = Power_Limt * 0.9f;
	txBuffer0x6FFforCAN2.Data[2] = chassis->Power.PowerToSurperCap;
	txBuffer0x6FFforCAN2.Data[3] = referee2024.power_heat_data_t.buffer_energy;
	txBuffer0x6FFforCAN2.Data[4] = referee2024.power_heat_data_t.chassis_current;
	txBuffer0x6FFforCAN2.Data[5] = referee2024.power_heat_data_t.chassis_voltage;
	txBuffer0x6FFforCAN2.Data[6] = (uint8_t)referee2024.power_heat_data_t.chassis_power >> 8;
	txBuffer0x6FFforCAN2.Data[7] = 0xff & (uint8_t)referee2024.power_heat_data_t.chassis_power;
	
	CAN_Send(&can, &txBuffer0x6FFforCAN2);   
}

/*		能量测试		*/
void Energy_Cal(void){
		if(Rc_Ctrl.Chassis_X_Integ !=0 || Rc_Ctrl.Chassis_Y_Integ != 0 || OmniChassis.Flag.RollingFlag == 1)
		{   
			if(cnt_static !=0) Last_cnt_static = cnt_static;
			cnt_move++;
			   
			if(cnt_move == 1)	Last_Power_Chassis_Static = Power_Chassis_Static;
			Power_Chassis_Static = 0;
			
			Power_Chassis += referee2024.power_heat_data_t.chassis_power * 0.001f;
			
			if(cnt_move % 10000 == 0)
			{
				Powerto10000_Chassis = Power_Chassis;
				Power_Chassis = 0;	
			}
			cnt_static =0;
		}
		else{
			if(cnt_move !=0)	Last_cnt_move = cnt_move;
			cnt_static++;
			if (cnt_static == 1) Last_Power_Chassis = Power_Chassis;
			Power_Chassis = 0;
			Power_Chassis_Static += referee2024.power_heat_data_t.chassis_power * 0.001f;
			cnt_move =0;
		}
	}
