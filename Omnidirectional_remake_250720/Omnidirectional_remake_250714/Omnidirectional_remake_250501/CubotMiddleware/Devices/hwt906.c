#include "hwt906.h"
#include "hardware_config.h"
#include "ins.h"

//HWT906_DATA_t hwt906_data;
Attitude_t *hwt906_data;

/**
  *@brife  usart5接受串口陀螺仪
	*/
uint8_t Usart5_RxBuffer[60];

UART_RxBuffer uart5_buffer={
	.Data = Usart5_RxBuffer,
	.Size = 60
};



void HWT906_Data_Deal(uint8_t *data_receive,Attitude_t *hwt906)
{

	if(data_receive[0]==0x55 && data_receive[1]==0x51)
	{
		hwt906->accel[0] = (float)((int16_t)(data_receive[3]<<8|data_receive[2]))*32768*16*9.8f;
		hwt906->accel[1] = (float)((int16_t)(data_receive[5]<<8|data_receive[4]))*32768*16*9.8f;
		hwt906->accel[2]= (float)((int16_t)(data_receive[7]<<8|data_receive[6]))*32768*16*9.8f;
	}
	
	if(data_receive[11]==0x55 && data_receive[12]==0x52)
	{
		hwt906->gyro[0] = (float)((int16_t)(data_receive[14]<<8|data_receive[13]))/32768*2000;
		hwt906->gyro[1] = (float)((int16_t)(data_receive[16]<<8|data_receive[15]))/32768*2000;
		hwt906->gyro[2] = (float)((int16_t)(data_receive[18]<<8|data_receive[17]))/32768*2000;
	}
	
	if(data_receive[22]==0x55 && data_receive[23] == 0x53)
	{
		hwt906->roll 	= (float)((int16_t)(data_receive[25]<<8|data_receive[24]))/32768*180;
		hwt906->pitch	= (float)((int16_t)(data_receive[27]<<8|data_receive[26]))/32768*180;
		hwt906->yaw		= (float)((int16_t)(data_receive[29]<<8|data_receive[28]))/32768*180;
	}
	
	if(data_receive[33]==0x55 && data_receive[34]==0x59)
	{
		hwt906->q[0] = (float)((int16_t)(data_receive[36]<<8|data_receive[35]))/32768;
		hwt906->q[1] = (float)((int16_t)(data_receive[38]<<8|data_receive[37]))/32768;
		hwt906->q[2] = (float)((int16_t)(data_receive[40]<<8|data_receive[39]))/32768;
		hwt906->q[3] = (float)((int16_t)(data_receive[42]<<8|data_receive[41]))/32768;
	}
 	
}


/**
  * @brief   串口5串口陀螺仪回调函数 
  * @param[in]  
  */
uint8_t Gyro_callback(uint8_t * recBuffer, uint16_t len)
{
	HWT906_Data_Deal(recBuffer,hwt906_data);
	return 0;
}


