////**
// **********************************************************************************
// * @file        driver_spi.c
// * @brief       �����㣬����SPI�ӻ��ĳ�ʼ�����������̣������ο�HNU�Ŀ�Դ�Դ���������޸�
// * @details
// * @date        2024-07-26
// * @version     V1.0
// * @copyright   Copyright CUMT-CUBOT (c) 2024
// **********************************************************************************
// * @attention
// * Ӳ��ƽ̨: STM32H750VBT \n
// * SDK�汾��-++++
// * @par �޸���־:
// * <table>
// * <tr><th>Date         <th>Version     <th>Author      <th>Description
// * <tr><td>2024-07-26   <td>1.0         <td>EmberLuo    <td>������ʼ�汾
// * </table>
// *
// **********************************************************************************
// ==============================================================================
//                            How to use this driver
// ==============================================================================

//    1. Include the "driver_spi.h" header file in your project.
//        �������Ŀ�а���"driver_spi.h"ͷ�ļ���
//    2. Create an instance of SPI_Slave_t for each SPI slave device you want to communicate with.
//        ΪҪ��֮ͨ�ŵ�ÿ��SPI���豸����һ��SPI_Slave_tʵ����
//    3. Initialize each SPI slave using the SPI_Slavelnit() function, providing the necessary parameters.
//        ʹ��SPI_Slavelnit()������ʼ��ÿ��SPI�������ṩ��Ҫ�Ĳ�������SPI������������úͽ��ջص�������
//    4. Set the SPI mode for each slave using SPL_SetMode(), choosing between DMA, interrupt, or blocking modes.
//        ʹ��SPL_SetMode()����ÿ���ӻ���SPIģʽ����DMA���жϻ�����ģʽ֮�����ѡ��
//    5. Use the provided functions to communicate with the SPI slaves:SPI_Transmit() to send data SPI_Recv() to receive data SPI_TransRecv() for fullduplex communication.
//        ʹ���ṩ�ĺ�����SPI��վͨ��:SPI_Transmit()��������SPI_Recv()��������SPI_TransRecv()����ȫ˫��ͨ��
//    6. Implement the necessary callback functions to handle received data or transmission completion events.
//        ʵ�ֱ�Ҫ�Ļص�������������յ������ݻ�������¼���

// **********************************************************************************
// * @attention
// * Ӳ��ƽ̨: STM32H750VBT \n
// * SDK�汾��-++++
// * if you had modified this file, please make sure your code does not have many
// * bugs, update the version NO., write dowm your name and the date, the most
// * important is make sure the users will have clear and definite understanding
// * through your new brief.
// **********************************************************************************
// * @exception   SPI_NSS������ģʽ��SPI_NSS_Hard��SPI_NSS_Soft��
// *              SPI_NSS_Hard��Ӳ���Զ���������Ƭѡ������������Զ�������ʽ����Ҫ�ߵģ�
// *              ȱ���ǵ�STM32Ϊ���豸ʱ��ͬһ��SPI����ֻ�ܽ�һ�����豸��
// *              SPI_NSS_Soft��������ƣ�GPIO����Ƭѡ�������ͣ��ŵ���һ��SPI������ԹҶ���豸��
// *              SPI_NSS_SoftҲ��ʹ�ý϶�ķ�ʽ, ��������ʱֻ�������ģʽ��
// * @todo        ���ǽ��ӻ���������
// **********************************************************************************
// */

#include "driver_spi.h"
SPI_Instance_t SPI_t[6]={{.spiHandler=&hspi1,.DevicesList={&(SPI_t[0].DevicesList),&(SPI_t[0].DevicesList)}}};

void SPI_RegisterSlave(SPI_Instance_t *master, SPI_Slave_t *slave)
{
   list_add(&slave->list, (&master->DevicesList));
	slave->spiHandler=master->spiHandler;
}


