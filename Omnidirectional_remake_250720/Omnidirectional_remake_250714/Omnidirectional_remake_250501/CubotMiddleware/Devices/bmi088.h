#ifndef _BMI088_H_ // ��ֹ�ظ�����
#define _BMI088_H_

#include "ins.h"
#include "driver_spi.h"
#include "driver_counter.h"
#include "bmi088_regNdef.h"

// ���ֶ�У׼���޸�����
#define BMI088_GxOFFSET (-0.00418030564f)
#define BMI088_GyOFFSET (-0.0052329856f)
#define BMI088_GzOFFSET (-0.000287621311f)
#define BMI088_gNORM    10.2522392f

#define BMI088_USE_SPI

typedef struct {
    IMU_InitData_t bmi088_Data;
	  SPI_Slave_t bmi088Accel;
    SPI_Slave_t bmi088Gyro;
	
} BMI088_t;

/**
 * @brief ��ʼ��BMI088,�������ӵ�SPI����handle,�Լ��Ƿ�������߱궨
 *
 * @param bmi088_SPI handle
 * @param calibrate  1Ϊ�������߱궨,0ʹ����������
 * @return uint8_t   �ɹ��򷵻�BMI088_NO_ERROR
 */
extern uint8_t BMI088_Init(IMU_InitData_t *bmi088_data);

/**
 * @brief ���ټƳ�ʼ��
 *
 * @return uint8_t
 */
extern uint8_t BMI088_AccInit(void);

/**
 * @brief �����ǳ�ʼ��
 *
 * @return uint8_t
 */
extern uint8_t BMI088_GyroInit(void);

/**
 * @brief ��ȡһ��BMI088������,����gyro��accel
 *
 * @param bmi088_data ����BMI088ʵ��(�ṹ��)
 */
 void BMI088_Read(IMU_InitData_t *bmi088_data,Device_MODE mode);
 void BMI088_gyro_read_muli_reg_IT(uint8_t reg,uint8_t * data, uint8_t len) ;
 void BMI088_accel_read_muli_reg_IT(uint8_t reg,uint8_t * data, uint8_t len) ;
 	void bmi088Accel_CallBack(struct _SPI_Slave_t *);
	void bmi088Gyro_CallBack(struct _SPI_Slave_t *);
extern BMI088_t bmi088;

#if defined(BMI088_USE_SPI)

#elif defined(BMI088_USE_IIC)

#endif

#endif
