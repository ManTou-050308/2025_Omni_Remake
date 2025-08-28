#ifndef HARDWARE_H_
#define HARDWARE_H_
#include "stdint.h"
#include "driver_usart.h"
#include "pid.h"
#include "SMC.h"

#define myABS(x) ( (x)>0?(x):-(x) )


/*	前置声明		*/
typedef struct RC_Ctrl_T 			RC_Ctrl_t;
typedef struct VT13_T 				VT13_t;
typedef struct Holder_T 			Holder_t;
typedef struct Omni_Chassis_T	Omni_Chassis_t;
typedef struct Trace_T 				Trace_t;
typedef struct CubotBrain_T 	CubotBrain_t;
typedef struct SuperCap_T 		Super_Cap_t;
typedef struct Loader_T				Loader_t;
/*	实例化结构体全局声明	*/
extern Super_Cap_t			Super_Cap;
extern Holder_t					Holder;
extern Omni_Chassis_t 	OmniChassis;
extern RC_Ctrl_t 				Rc_Ctrl;
extern Trace_t					Vision_Info;
extern Loader_t					Loader;
extern VT13_t 					VT13;

extern UART_RxBuffer uart8_buffer;
//extern UART_RxBuffer uart6_buffer;

void HardwareConfig(void);
#endif
