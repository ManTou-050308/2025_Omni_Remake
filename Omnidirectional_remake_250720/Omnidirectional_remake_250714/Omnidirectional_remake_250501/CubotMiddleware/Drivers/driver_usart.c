/**@file  driver_usart.c
* @brief   �����㣬���ڹ����������ļ����û��ص��ض���
* @details  ��Ҫ�����������ڹ��������ṩ���ڳ�ʼ�����û��ص��ض���
* @author      RyanJiao  any question please send mail to 1095981200@qq.com						 
* @date        2021-8-23
* @version     V1.1
* @copyright    Copyright (c) 2021-2121  �й���ҵ��ѧCUBOTս��
**********************************************************************************
* @attention
* Ӳ��ƽ̨: STM32H750VBT \n
* SDK�汾��-++++
* @par �޸���־:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2021-8-12  <td>1.0      <td>RyanJiao  <td>������ʼ�汾
* </table>
*
**********************************************************************************
 ==============================================================================
                          How to use this driver  
 ==============================================================================
 
	���driver_can.h

	1. ��UARTx_Init() �� ��� �� �û�����Ľ��ջص����� ������UART�ṹ��  ���ص������жԽ��յ������ݽ��� IDʶ�� �� �ϲ����㣩

  2. �û���д UART_RxBuffer������ Ŀ�껺������ַ �� ���ݳ��ȡ�

	3. ����UART_Open() ���� UART_Object �� �û���д UART_RxBuffer��
	
	4. �� UART_Idle_Handler ��ӵ� stm32H7xx_it.c �� USARTx_IRQHandler() �У������û���д��ͬһ�� UART_RxBuffer ��
	
	5. Ӧ�ò��д UART_TxBuffer �����ͻ������ṹ�壩������������ֽ������׵�ַ���ֽڳ���
	
	6. ��UART_Send()���� UART�豸�ṹ�� �� UART_TxBuffer�ṹ�壬�����ݷ��ͳ�ȥ

  ********************************************************************************
	* @attention
	* Ӳ��ƽ̨: STM32H750VBT \n
	* SDK�汾��-++++
  * if you had modified this file, please make sure your code does not have many 
  * bugs, update the version NO., write dowm your name and the date, the most
  * important is make sure the users will have clear and definite understanding 
  * through your new brief.
  ********************************************************************************
	��DMA��NDTR�Ĵ���������
	This register can be written only
	when the stream is disabled. When the stream is enabled, this register is read-only,
	indicating the remaining data items to be transmitted. This register decrements after each
	DMA transfer.
	ָ����DMA�д������ʣ�����ݸ��� ÿ��DMA������ɺ��Զ���һ
	�ο��ֲ��ж�idle�����жϴ���������������
	
*/
#include "driver_usart.h"
#include "referee.h"
#include "usart.h"
#include "dr16.h"
#include "hardware_config.h"
#include "stdio.h"
UART_Object uart1;
UART_Object uart2;
UART_Object uart3;
UART_Object uart4;
UART_Object uart5;
UART_Object uart6;
UART_Object uart7;
UART_Object uart8;
/*-------------- �����Ĵ����Ķ����� ----------------*/
/*
		1.((((DMA_Stream_TypeDef*)hdma_usart1_rx.Instance)->CR & DMA_SxCR_CT) == RESET)
			���ж���佫DMA�������üĴ����벻֪��ʲô������λ�봦���и�ʲô��λ��֪����������
		2.����˫�������Ǹ����õ�
		3.DMAΪʲôҪ�Ӻ�
		4.����1�ж�ΪʲôҪ�������� ����Ϊ���ܻ�����
		
*/

/**
  * @brief   ���ڳ�ʼ����������ͽ��ջص����������ڽṹ��
  */
void UARTx_Init(UART_HandleTypeDef* handle, UART_RxIdleCallback rxIdleCallback)
{
	// ��ʼ��uart1
	if (handle->Instance == USART1)
	{
		uart1.Handle = handle;
		uart1.RxIdleCallback = rxIdleCallback;
	}
	
	// ��ʼ��uart2
	if (handle->Instance == USART2)
	{
		uart2.Handle = handle;
		uart2.RxIdleCallback = rxIdleCallback;
	}
	
	// ��ʼ��uart3
	if (handle->Instance == USART3)
	{
		uart3.Handle = handle;
		uart3.RxIdleCallback = rxIdleCallback;
	}
	
	// ��ʼ��uart4
	if (handle->Instance == UART4)
	{
		uart4.Handle = handle;
		uart4.RxIdleCallback = rxIdleCallback;
	}
	
	// ��ʼ��uart5
	if (handle->Instance == UART5)
	{
		uart5.Handle = handle;
		uart5.RxIdleCallback = rxIdleCallback;
	}
	
	// ��ʼ��uart6
	if (handle->Instance == USART6)
	{
		uart6.Handle = handle;
		uart6.RxIdleCallback = rxIdleCallback;
	}
		// ��ʼ��uart7
	if (handle->Instance == UART7)
	{
		uart7.Handle = handle;
		uart7.RxIdleCallback = rxIdleCallback;
	}
		// ��ʼ��uart8
	if (handle->Instance == UART8)
	{
		uart8.Handle = handle;
		uart8.RxIdleCallback = rxIdleCallback;
	}
}


/**
  * @brief  ���ڹ������ṹ������Ѿ�Ԥ����д�õĴ����豸��ʼ��
  */
void UART_Receive_DMA(UART_Object* uart, UART_RxBuffer* rxBuffer)
{
	HAL_UART_Receive_DMA(uart->Handle, rxBuffer->Data, rxBuffer->Size);
//	__HAL_UART_ENABLE_IT(uart->Handle, UART_IT_IDLE);   		//<�����ж�ǰ����ص��ȽϺ�
} 


/**
  * @brief  ���ڹ������ṹ������Ѿ�Ԥ����д�õĴ����豸��ʼ��     //���ֲ����ֿ�ʹ�ã��Ӻ�DMA����
  */
void UART_ENABLE_IT(UART_Object* uart, UART_RxBuffer* rxBuffer)
{
//	HAL_UART_Receive_DMA(uart->Handle, rxBuffer->Data, rxBuffer->Size);
	__HAL_UART_ENABLE_IT(uart->Handle, UART_IT_IDLE);   		//<�����ж�ǰ����ص��ȽϺ�
}

/**
  * @brief  ���ڵ���dma�������ݣ���������Ϊ�ֽ�
  */
uint32_t UART_Send(UART_Object* uart,UART_TxBuffer* txBuffer)
{
	return HAL_UART_Transmit_DMA(uart->Handle,txBuffer->Data, txBuffer->DataSize);
}



/**
  * @brief  �����豸�жϺ�����ִ���ж�DMA���������ô����û��ص����� 
  */
void UART_Idle_Handler(UART_Object* uart, UART_RxBuffer* rxBuffer)
{
	//�жϲ����Ƿ�Ϸ��������ǵĴ���ûʹ�������
	assert_param(uart != NULL);
	
	uint16_t usart_rx_num;
	
	if((__HAL_UART_GET_FLAG(uart->Handle, UART_FLAG_IDLE) != RESET)&&uart->Handle->Instance != NULL)
	{
		HAL_UART_DMAStop(uart->Handle);								//< �ر�DMA����ֹ������������ݸ��£���ɶ�ʧ����															
		__HAL_UART_CLEAR_IDLEFLAG(uart->Handle);					//< ���idle��־λ����ֹ�ٴν����ж�			
		__HAL_UART_CLEAR_OREFLAG(uart->Handle);
//		if ((((DMA_Stream_TypeDef*)uartManage->huart.hdmarx->Instance)->NDTR) == uartManage->rxBufferSize)	//< �жϻ��������� �Ƿ���� DMA��δ�������ݳ��ȡ���Ϊ���ڣ����ݸ�����һ�Ρ� 
		
		//�ܼ�����ȥδ��������ݸ������õ��Ѿ����յ����ݸ���
		usart_rx_num = rxBuffer->Size - ((DMA_Stream_TypeDef*)uart->Handle->hdmarx->Instance)->NDTR;
		{
			if((*uart).RxIdleCallback!=NULL)//< ������ͷ�������������շ���ʼ���պ��Ƿ���Ҫƥ��ͷ�ֽں�β�ֽ�
				uart->RxIdleCallback(rxBuffer->Data, usart_rx_num); 	//<�û��ص�
		}
		HAL_UART_DMAResume(uart->Handle);
		HAL_UART_Receive_DMA(uart->Handle, rxBuffer->Data, rxBuffer->Size);
	}
}

void USART1_Idle_Handler(UART_Object* uart, UART_RxBuffer* rxBuffer)
{	
	assert_param(uart != NULL);
	
	uint16_t usart_rx_num;
	
	if((__HAL_UART_GET_FLAG(uart->Handle, UART_FLAG_IDLE) != RESET))
	{	
		HAL_UART_DMAStop(uart->Handle);
		__HAL_UART_CLEAR_IDLEFLAG(uart->Handle);																																//< ���idle��־λ����ֹ�ٴν����ж�			
		__HAL_UART_CLEAR_OREFLAG(uart->Handle);	

		if ((((DMA_Stream_TypeDef*)hdma_usart1_rx.Instance)->CR & DMA_SxCR_CT) == RESET)
        {
            /* Current memory buffer used is Memory 0 */
    
            //disable DMA
            //ʧЧDMA
            __HAL_DMA_DISABLE(&hdma_usart1_rx);

            //get receive data length, length = set_data_length - remain_length
            //��ȡ�������ݳ���,���� = �趨���� - ʣ�೤��
            usart_rx_num = 36 - ((DMA_Stream_TypeDef*)hdma_usart1_rx.Instance)->NDTR;

            //reset set_data_lenght
            //�����趨���ݳ���
            ((DMA_Stream_TypeDef*)hdma_usart1_rx.Instance)->NDTR = 36;

            //set memory buffer 1
            //�趨������1
            ((DMA_Stream_TypeDef*)hdma_usart1_rx.Instance)->CR |= DMA_SxCR_CT;
            
            //enable DMA
            //ʹ��DMA
            __HAL_DMA_ENABLE(&hdma_usart1_rx);

            if(usart_rx_num == 18)
            {
                DR16_callback(DR16_RxBuffer1, 18);
            }
        }
        else
        {
            /* Current memory buffer used is Memory 1 */
            //disable DMA
            //ʧЧDMA
            __HAL_DMA_DISABLE(&hdma_usart1_rx);

            //get receive data length, length = set_data_length - remain_length
            //��ȡ�������ݳ���,���� = �趨���� - ʣ�೤��
            usart_rx_num = 36 - ((DMA_Stream_TypeDef*)hdma_usart1_rx.Instance)->NDTR;

            //reset set_data_lenght
            //�����趨���ݳ���
            ((DMA_Stream_TypeDef*)hdma_usart1_rx.Instance)->NDTR = 36;

            //set memory buffer 0
            //�趨������0
            DMA1_Stream0->CR &= ~(DMA_SxCR_CT);
            
            //enable DMA
            //ʹ��DMA
            __HAL_DMA_ENABLE(&hdma_usart1_rx);

            if(usart_rx_num == 18)
            {
                //����ң��������
                DR16_callback(DR16_RxBuffer0, 18);
            }
        }
//		Online_State_Update(&Online_State_debug,uart->uart_number);                                             //< �����жϼ��
				
				HAL_UART_DMAResume(uart->Handle);		
				HAL_UART_Receive_DMA(uart->Handle, rxBuffer->Data, rxBuffer->Size);                                     //< DMA����
	}
}

	void USART1_DoubleBuffer_init(uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num)
{
    //enable the DMA transfer for the receiver request
    //ʹ��DMA���ڽ���
    SET_BIT(huart1.Instance->CR3, USART_CR3_DMAR);

    //enalbe idle interrupt
    //ʹ�ܿ����ж�
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);

    //disable DMA
    //ʧЧDMA
    __HAL_DMA_DISABLE(&hdma_usart1_rx);
    while(((DMA_Stream_TypeDef*)hdma_usart1_rx.Instance)->CR & DMA_SxCR_EN)
    {
        __HAL_DMA_DISABLE(&hdma_usart1_rx);
    }

    ((DMA_Stream_TypeDef*)hdma_usart1_rx.Instance)->PAR = (uint32_t) & (USART1->RDR);
    //memory buffer 1
    //�ڴ滺����1
    ((DMA_Stream_TypeDef*)hdma_usart1_rx.Instance)->M0AR = (uint32_t)(rx1_buf);
    //memory buffer 2
    //�ڴ滺����2
    ((DMA_Stream_TypeDef*)hdma_usart1_rx.Instance)->M1AR = (uint32_t)(rx2_buf);
    //data length
    //���ݳ���
    ((DMA_Stream_TypeDef*)hdma_usart1_rx.Instance)->NDTR = dma_buf_num;
    //enable double memory buffer
    //ʹ��˫������
    SET_BIT(((DMA_Stream_TypeDef*)hdma_usart1_rx.Instance)->CR, DMA_SxCR_DBM);

    //enable DMA
    //ʹ��DMA
    __HAL_DMA_ENABLE(&hdma_usart1_rx);

}

/**
  * @brief  ����ת�����ַ���ASCii�� 
  */
uint8_t* itoa(int  num, uint8_t* str, int radix)
{
    uint8_t index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"; //<������
    unsigned unum;																	  			//<���Ҫת���������ľ���ֵ,ת�������������Ǹ���
    int i=0,j,k;																					  //<i����ָʾ�����ַ�����Ӧλ��ת��֮��i��ʵ�����ַ����ĳ��ȣ�ת����˳��������ģ��������������k����ָʾ����˳��Ŀ�ʼλ��;j����ָʾ����˳��ʱ�Ľ�����
																														//<��ȡҪת���������ľ���ֵ
    if(radix==10&&num<0)																		//<Ҫת����ʮ�����������Ǹ���
    {
        unum=(unsigned)-num;																//<��num�ľ���ֵ����unum
        str[i++]='-';																				//<���ַ�����ǰ������Ϊ'-'�ţ�����������1
    }
    else unum=(unsigned)num;																//<����numΪ����ֱ�Ӹ�ֵ��unum
		
    do																							  			//<ת�����֣�ע��ת�����������
    {
        str[i++]=index[unum%(unsigned)radix];							//<ȡunum�����һλ��������Ϊstr��Ӧλ��ָʾ������1
        unum/=radix;																				//<unumȥ�����һλ
 
    }while(unum);																						//<ֱ��unumΪ0�˳�ѭ��
 
    str[i]='\0';																						//<���ַ���������'\0'�ַ���c�����ַ�����'\0'������
 
																														//<��˳���������
    if(str[0]=='-') k=1;																		//<����Ǹ��������Ų��õ������ӷ��ź��濪ʼ����
    else k=0;																								//<���Ǹ�����ȫ����Ҫ����
 
    uint8_t temp;																						//<��ʱ��������������ֵʱ�õ�
    for(j=k;j<=(i-1)/2;j++)																	//<ͷβһһ�Գƽ�����i��ʵ�����ַ����ĳ��ȣ��������ֵ�ȳ�����1
    {
        temp=str[j];																				//<ͷ����ֵ����ʱ����
        str[j]=str[i-1+k-j];																//<β����ֵ��ͷ��
        str[i-1+k-j]=temp;																	//<����ʱ������ֵ(��ʵ����֮ǰ��ͷ��ֵ)����β��
    }
    return str;																							//<����ת������ַ���
 
}
