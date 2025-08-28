#ifndef _DRIVER_SPI_H_
#define _DRIVER_SPI_H_

#include "gpio.h"
#include "spi.h"
#include <stdlib.h>
#include <stdint.h>
#include "linux_list.h" 

enum SPI_LIST {
    DEVICE_SPI1,
    DEVICE_SPI2,
    DEVICE_SPI3,
    DEVICE_SPI4,
    DEVICE_SPI5,
    DEVICE_SPI6,
    SPI_DEVICE_NUM,
};

/**
 * @brief SPIƬѡ�źŶ�Ӧ��GPIO���ź�
 */
typedef struct
{
    GPIO_TypeDef *gpiox; // Ƭѡ�źŶ�Ӧ��GPIO,��GPIOA,GPIOB�ȵ�
    uint16_t cs_pin;     // Ƭѡ�źŶ�Ӧ�����ź�,GPIO_PIN_1,GPIO_PIN_2�ȵ�
} SPI_Chip_Select_t;

/**
 * @brief   SPI���ջ�����
 */
typedef struct
{
    uint8_t *Data;
    uint16_t Size;
} SPI_RxBuffer_t;

/**
 * @brief	SPI���ͻ�����
 */
typedef struct
{
    uint8_t *Data;
    uint16_t Size;
} SPI_TxBuffer_t;

/**
 * @brief SPI�ӻ��ṹ��
 */
typedef struct _SPI_Slave_t {
    SPI_RxBuffer_t rxBuffer;                      // ���ջ������ṹ��
    SPI_TxBuffer_t txBuffer;                      // ���ͻ������ṹ��
    SPI_Chip_Select_t chipSelect;                 // ƬѡGPIO���Žṹ��
    void (*RxCallBackSPI)(struct _SPI_Slave_t *); // ���ջص�����
	  list_t   list;
	  SPI_HandleTypeDef* spiHandler;
} SPI_Slave_t;

/**
 * @brief SPIʵ���ṹ��
 */
typedef struct _SPI_Instance_t {
	  SPI_HandleTypeDef* spiHandler; 
	  list_t   DevicesList;
} SPI_Instance_t;

/**
 * @brief   SPI�ص�����
 */
typedef void (*SPI_RxCallback)(SPI_Slave_t *);


void SPI_RegisterSlave(SPI_Instance_t *master, SPI_Slave_t *slave);



extern SPI_Instance_t SPI_t[6];
typedef enum {
    BLOCK_MODE, // Ĭ��ʹ������ģʽ
    IT_MODE,
    DMA_MODE,
} Device_MODE;

#endif
