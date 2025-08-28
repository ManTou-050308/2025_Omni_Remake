#ifndef VT13_H__
#define VT13_H__

#include "stm32h7xx.h"
#include "cmsis_armcc.h"
#include "driver_usart.h"
#include "dr16.h"

#define Key_Filter_Num_vT13 2       //< 按键检测消抖滤波时间(ms)
#define VT13_rxBufferLengh 36	 //< dr16接收缓存区数据长度
/**
  * @brief  详细按键定义，补充接收机结构体内容
  */
#define key_W_vT13        key[0]
#define key_A_vT13        key[1]
#define key_S_vT13        key[2]
#define key_D_vT13        key[3]
#define key_shift_vT13    key[4]
#define key_ctrl_vT13     key[5]
#define key_Q_vT13        key[6]
#define key_E_vT13        key[7]
#define key_V_vT13        key[8]
#define key_F_vT13        key[9]
#define key_G_vT13        key[10]
#define key_C_vT13        key[11]
#define key_R_vT13        key[12]
#define key_B_vT13        key[13]
#define key_Z_vT13        key[14]
#define key_X_vT13        key[15]
 
#define key_W_flag_vT13        keyflag[0]
#define key_A_flag_vT13        keyflag[1]
#define key_S_flag_vT13        keyflag[2]
#define key_D_flag_vT13        keyflag[3]
#define key_shift_flag_vT13    keyflag[4]
#define key_ctrl_flag_vT13     keyflag[5]
#define key_Q_flag_vT13        keyflag[6]
#define key_E_flag_vT13        keyflag[7]
#define key_V_flag_vT13        keyflag[8]
#define key_F_flag_vT13        keyflag[9]
#define key_G_flag_vT13        keyflag[10]
#define key_C_flag_vT13        keyflag[11]
#define key_R_flag_vT13        keyflag[12]
#define key_B_flag_vT13        keyflag[13]
#define key_Z_flag_vT13        keyflag[14]
#define key_X_flag_vT13        keyflag[15]

#define W_Num_vT13        0
#define A_Num_vT13        1
#define S_Num_vT13        2
#define D_Num_vT13        3
#define shift_Num_vT13    4
#define ctrl_Num_vT13     5
#define Q_Num_vT13        6
#define E_Num_vT13        7
#define V_Num_vT13        8
#define F_Num_vT13        9
#define G_Num_vT13        10
#define C_Num_vT13        11
#define R_Num_vT13        12
#define B_Num_vT13        13
#define Z_Num_vT13        14
#define X_Num_vT13        15

typedef struct VT13_T{ 
 struct {       
	 uint16_t ch0;       
	 uint16_t ch1;       
	 uint16_t ch2;      
	 uint16_t ch3;
	 uint16_t sw; 
	 uint8_t mode_sw;
	 uint8_t go_home;
	 uint8_t Fn;
	 uint8_t change;
	     
	 uint8_t photo;
	}rc; 

 struct {       
	 int16_t x;       
	 int16_t y;       
	 int16_t z;         
	 uint8_t press_l;     
	 uint8_t press_m;    
	 uint8_t press_r; 
	 uint8_t press_l_flag;  
	 uint8_t press_m_flag; 	 
	 uint8_t press_r_flag; 
 }mouse; 

	struct {     
		uint8_t keyboard14;
    uint8_t keyboard15;	
		uint8_t s1ands1;
		uint8_t slands2;
	}keyboard;
	
	uint8_t  key[18];   
	uint8_t  keyflag[18];	
	uint8_t  isUnpackaging;  	 //< 解算状态标志位，解算过程中不读取数据
	uint8_t  isOnline;
	uint32_t onlineCheckCnt;
	
	float Chassis_Z_Integ;
	float Chassis_Y_Integ;//斜坡积分变量
  float Chassis_X_Integ;
	int RestFlag;  
}VT13_t;

void vt13_recv_datas_modify(uint8_t *pdata,VT13_t* vt13 );
void PC_keybroad_filter_vt13(VT13_t* RC_Ctl);
void KeyBoard_DataUnpack_vt13(VT13_t * vt13,uint8_t * recBuffer, Trace_t* Info_Vision , Omni_Chassis_t* chassis , Super_Cap_t* Cap ,Loader_t* loader);
extern 	VT13_t VT13;
extern UART_RxBuffer uart3_buffer;
uint8_t VT13_callback(uint8_t * recBuffer);

#endif