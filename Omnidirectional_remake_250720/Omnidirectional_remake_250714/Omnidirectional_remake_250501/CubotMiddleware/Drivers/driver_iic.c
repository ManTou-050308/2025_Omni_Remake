#include "driver_iic.h"
#include "i2c.h"


void I2C_Error(I2C_HandleTypeDef* hi2c)
{
	/* �ָ�I2C�Ĵ���ΪĬ��ֵ */
	HAL_I2C_DeInit(hi2c);
	/* ���³�ʼ��I2C���� */
	HAL_I2C_Init(hi2c);
}



/**
  * @brief  д�Ĵ����������ṩ���ϲ�Ľӿ�
  */
uint8_t Sensors_I2C_WriteRegister(I2C_HandleTypeDef* hi2c, uint8_t slave_addr, uint8_t reg_addr, uint16_t len, uint8_t* data_ptr)
{
	HAL_StatusTypeDef status = HAL_OK;
	status = HAL_I2C_Mem_Write(hi2c, slave_addr, reg_addr, I2C_MEMADD_SIZE_8BIT, data_ptr, len, I2Cx_FLAG_TIMEOUT);
	
	/* ���ͨѶ״̬ */
	if (status != HAL_OK)
	{
		/* ���߳����� */
		I2C_Error(hi2c);
	  
	}
	while (HAL_I2C_GetState(hi2c) != HAL_I2C_STATE_READY)
	{

	}

	/* ���SENSOR�Ƿ����������һ�ζ�д���� */
	while (HAL_I2C_IsDeviceReady(hi2c, slave_addr, I2Cx_FLAG_TIMEOUT, I2Cx_FLAG_TIMEOUT) == HAL_TIMEOUT);
	
	/* �ȴ�������� */
	while (HAL_I2C_GetState(hi2c) != HAL_I2C_STATE_READY)
	{

	}
	
	return 0;
}


/**
  * @brief  ���Ĵ����������ṩ���ϲ�Ľӿ�
  */
uint8_t Sensors_I2C_ReadRegister(I2C_HandleTypeDef* hi2c, uint8_t slave_addr, uint8_t reg_addr, uint16_t len, uint8_t* data_ptr)
{
	HAL_StatusTypeDef status = HAL_OK;
	status = HAL_I2C_Mem_Read(hi2c, slave_addr, reg_addr, I2C_MEMADD_SIZE_8BIT, data_ptr, len, I2Cx_FLAG_TIMEOUT);
	/* ���ͨѶ״̬ */
	if (status != HAL_OK)
	{
		/* ���߳����� */
		I2C_Error(hi2c);
	}
	
	while (HAL_I2C_GetState(hi2c) != HAL_I2C_STATE_READY)
	{

	}
	
	/* ���SENSOR�Ƿ����������һ�ζ�д���� */
	while (HAL_I2C_IsDeviceReady(hi2c, slave_addr, I2Cx_FLAG_TIMEOUT, I2Cx_FLAG_TIMEOUT) == HAL_TIMEOUT);
	
	/* �ȴ�������� */
	while (HAL_I2C_GetState(hi2c) != HAL_I2C_STATE_READY)
	{

	}
	
	return status;
}


