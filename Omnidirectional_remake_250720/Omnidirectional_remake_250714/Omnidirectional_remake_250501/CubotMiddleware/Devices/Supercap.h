#ifndef SUPERCAP_H
#define SUPERCAP_H
#include "hardware_config.h"

#include "driver_can.h"

typedef struct	SuperCap_T
{
	struct
	{
		float    Voltage;
		float    Current;
		uint8_t  Supercap_Mode;
		uint8_t  Supercap_Flag;
		uint8_t  Supercap_Charge;
		uint8_t  Supercap_Charge_mode;
		uint8_t  voltage_flag;
		uint8_t  Turn_flag;
		uint8_t  Speed_flag;
		uint8_t  Rolling_flag;
		
		uint8_t Supercap_Charge_mode_Real;//实际充放电状态
		uint8_t SupercapIsOnline;
	}cap_state;
	
}SuperCap_t;



uint8_t Supercap_rxCallBack(CAN_Object canx, CAN_RxBuffer rxBuffer ,SuperCap_t* Cap);
void SupercapControl(CAN_Object can, RC_Ctrl_t* rc_ctrl , SuperCap_t* Cap,Omni_Chassis_t* chassis);
void Energy_Cal(void);
#endif
