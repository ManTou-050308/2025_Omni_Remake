#ifndef DRV_IIC_H
#define DRV_IIC_H

#include "stm32h7xx_hal.h"
#include "i2c.h"
#include "driver_iic.h"

#define I2Cx_FLAG_TIMEOUT             ((uint32_t) 1000) //0x1100
#define I2Cx_LONG_TIMEOUT             ((uint32_t) (300 * I2Cx_FLAG_TIMEOUT)) //was300


/**
  * @brief  д�Ĵ����������ṩ���ϲ�Ľӿ�
	* @param  slave_addr: �ӻ���ַ
	* @param 	reg_addr:�Ĵ�����ַ
	* @param len��д��ĳ���
	*	@param data_ptr:ָ��Ҫд�������
  * @retval ����Ϊ0��������Ϊ��0
  */
uint8_t Sensors_I2C_WriteRegister(I2C_HandleTypeDef* hi2c, uint8_t slave_addr, uint8_t reg_addr, uint16_t len, uint8_t* data_ptr);

	
/**
  * @brief  ���Ĵ����������ṩ���ϲ�Ľӿ�
	* @param  slave_addr: �ӻ���ַ
	* @param 	reg_addr:�Ĵ�����ַ
	* @param len��Ҫ��ȡ�ĳ���
	*	@param data_ptr:ָ��Ҫ�洢���ݵ�ָ��
  * @retval ����Ϊ0��������Ϊ��0
  */
uint8_t Sensors_I2C_ReadRegister(I2C_HandleTypeDef* hi2c, uint8_t slave_addr, uint8_t reg_addr, uint16_t len, uint8_t* data_ptr);


#endif 

