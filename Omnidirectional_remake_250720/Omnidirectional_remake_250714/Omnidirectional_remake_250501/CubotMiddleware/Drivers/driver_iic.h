#ifndef DRV_IIC_H
#define DRV_IIC_H

#include "stm32h7xx_hal.h"
#include "i2c.h"
#include "driver_iic.h"

#define I2Cx_FLAG_TIMEOUT             ((uint32_t) 1000) //0x1100
#define I2Cx_LONG_TIMEOUT             ((uint32_t) (300 * I2Cx_FLAG_TIMEOUT)) //was300


/**
  * @brief  写寄存器，这是提供给上层的接口
	* @param  slave_addr: 从机地址
	* @param 	reg_addr:寄存器地址
	* @param len：写入的长度
	*	@param data_ptr:指向要写入的数据
  * @retval 正常为0，不正常为非0
  */
uint8_t Sensors_I2C_WriteRegister(I2C_HandleTypeDef* hi2c, uint8_t slave_addr, uint8_t reg_addr, uint16_t len, uint8_t* data_ptr);

	
/**
  * @brief  读寄存器，这是提供给上层的接口
	* @param  slave_addr: 从机地址
	* @param 	reg_addr:寄存器地址
	* @param len：要读取的长度
	*	@param data_ptr:指向要存储数据的指针
  * @retval 正常为0，不正常为非0
  */
uint8_t Sensors_I2C_ReadRegister(I2C_HandleTypeDef* hi2c, uint8_t slave_addr, uint8_t reg_addr, uint16_t len, uint8_t* data_ptr);


#endif 

