#include "driver_iic.h"
#include "i2c.h"


void I2C_Error(I2C_HandleTypeDef* hi2c)
{
	/* 恢复I2C寄存器为默认值 */
	HAL_I2C_DeInit(hi2c);
	/* 重新初始化I2C外设 */
	HAL_I2C_Init(hi2c);
}



/**
  * @brief  写寄存器，这是提供给上层的接口
  */
uint8_t Sensors_I2C_WriteRegister(I2C_HandleTypeDef* hi2c, uint8_t slave_addr, uint8_t reg_addr, uint16_t len, uint8_t* data_ptr)
{
	HAL_StatusTypeDef status = HAL_OK;
	status = HAL_I2C_Mem_Write(hi2c, slave_addr, reg_addr, I2C_MEMADD_SIZE_8BIT, data_ptr, len, I2Cx_FLAG_TIMEOUT);
	
	/* 检查通讯状态 */
	if (status != HAL_OK)
	{
		/* 总线出错处理 */
		I2C_Error(hi2c);
	  
	}
	while (HAL_I2C_GetState(hi2c) != HAL_I2C_STATE_READY)
	{

	}

	/* 检查SENSOR是否就绪进行下一次读写操作 */
	while (HAL_I2C_IsDeviceReady(hi2c, slave_addr, I2Cx_FLAG_TIMEOUT, I2Cx_FLAG_TIMEOUT) == HAL_TIMEOUT);
	
	/* 等待传输结束 */
	while (HAL_I2C_GetState(hi2c) != HAL_I2C_STATE_READY)
	{

	}
	
	return 0;
}


/**
  * @brief  读寄存器，这是提供给上层的接口
  */
uint8_t Sensors_I2C_ReadRegister(I2C_HandleTypeDef* hi2c, uint8_t slave_addr, uint8_t reg_addr, uint16_t len, uint8_t* data_ptr)
{
	HAL_StatusTypeDef status = HAL_OK;
	status = HAL_I2C_Mem_Read(hi2c, slave_addr, reg_addr, I2C_MEMADD_SIZE_8BIT, data_ptr, len, I2Cx_FLAG_TIMEOUT);
	/* 检查通讯状态 */
	if (status != HAL_OK)
	{
		/* 总线出错处理 */
		I2C_Error(hi2c);
	}
	
	while (HAL_I2C_GetState(hi2c) != HAL_I2C_STATE_READY)
	{

	}
	
	/* 检查SENSOR是否就绪进行下一次读写操作 */
	while (HAL_I2C_IsDeviceReady(hi2c, slave_addr, I2Cx_FLAG_TIMEOUT, I2Cx_FLAG_TIMEOUT) == HAL_TIMEOUT);
	
	/* 等待传输结束 */
	while (HAL_I2C_GetState(hi2c) != HAL_I2C_STATE_READY)
	{

	}
	
	return status;
}


