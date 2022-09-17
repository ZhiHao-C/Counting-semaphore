//#include "stm32f10x.h"                  // Device header
#include "string.h"
#include <stdio.h>

#include "bps_led.h"
#include "bps_usart.h"
#include "key.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/**************************** ȫ�ֱ��� ********************************/



/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
 /* ���������� */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* ���������� */
static TaskHandle_t  Take_Task_Handle = NULL;
/* ���������� */
static TaskHandle_t  Give_Task_Handle = NULL;

//�ź������
SemaphoreHandle_t CountSem_Handle;



//��������
static void Take_Task(void* parameter);
static void Give_Task(void* parameter);
static void AppTaskCreate(void);

static void BSP_Init(void)
{
	/* 
	* STM32 �ж����ȼ�����Ϊ 4���� 4bit ��������ʾ��ռ���ȼ�����ΧΪ��0~15 
	* ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ� 
	* ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ� 
	*/ 
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 ); 
	LED_GPIO_Config();
	KEY1_GPIO_Config();
	KEY2_GPIO_Config();
	USART_Config();
	
	//����
//	led_G(on);
//	printf("���ڲ���");
}

int main()
{
	BaseType_t xReturn = NULL;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	
	//����һ�������ź���
	 CountSem_Handle=xSemaphoreCreateCounting(5,5);
	
	BSP_Init();
	printf("����ȫϵ�п�����-FreeRTOS-��̬��������!\r\n");
	printf("��λĬ��ֵΪ 5 �������� KEY1 ���복λ������ KEY2 �ͷų�λ��\n\n");


	

	
	  /* ����AppTaskCreate���� */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
                        (const char*    )"AppTaskCreate",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )1, /* ��������ȼ� */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
																							
	if(xReturn==pdPASS)
	{
		printf("��ʼ���񴴽��ɹ�\r\n");
		vTaskStartScheduler();
	}
	else 
	{
		return -1;
	}
	while(1)
	{
		
	}

}


//����������
static void Take_Task(void* parameter)
{
	BaseType_t xReturn = pdTRUE;/* ����һ��������Ϣ����ֵ��Ĭ��Ϊ pdTRUE */
	
	while(1)
	{
		if(key_scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN)==1)
		{
			//���ȴ�û����Ч�ź������˳�
			xReturn=xSemaphoreTake(CountSem_Handle,0);
			if(xReturn==pdTRUE)
			{
				printf( "KEY1 �����£��ɹ����뵽ͣ��λ��\n" ); 
			}
			else
			{
				printf( "KEY1 �����£�������˼������ͣ����������\n" );
			}
		}
	}    
}

//����������
static void Give_Task(void* parameter)
{
	BaseType_t xReturn = pdTRUE;/* ����һ��������Ϣ����ֵ��Ĭ��Ϊ pdTRUE */
	while(1)
	{
		if(key_scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN)==1)
		{
			xReturn=xSemaphoreGive(CountSem_Handle);
			if ( xReturn == pdTRUE ) 
			{
				printf( "KEY2 �����£��ͷ� 1 ��ͣ��λ��\n" ); 
			}
			else
			{
				printf( "�ͷ�ʧ�ܣ�ȫ����λ������ʹ�á�\n" ); 
			}
		}
		vTaskDelay(20);/* ��ʱ 20 �� tick */ 
		
	}    
}



static void AppTaskCreate(void)
{
	BaseType_t xReturn = NULL;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	
	taskENTER_CRITICAL();           //�����ٽ���
	

	//��������������
  xReturn=xTaskCreate((TaskFunction_t	)Take_Task,		//������
															(const char* 	)"Take_Task",		//��������
															(uint16_t 		)512,	//�����ջ��С
															(void* 		  	)NULL,				//���ݸ��������Ĳ���
															(UBaseType_t 	)3, 	//�������ȼ�
															(TaskHandle_t*  )&Take_Task_Handle);/* ������ƿ�ָ�� */ 	
															
	if(xReturn == pdPASS)/* �����ɹ� */
		printf("Take_Task���񴴽��ɹ�!\n");
	else
		printf("Take_Task���񴴽�ʧ��!\n");
	
	
	 //���� ���� ����
	 xReturn=xTaskCreate((TaskFunction_t	)Give_Task,		//������
															(const char* 	)"Give_Task",		//��������
															(uint16_t 		)512,	//�����ջ��С
															(void* 		  	)NULL,				//���ݸ��������Ĳ���
															(UBaseType_t 	)4, 	//�������ȼ�
															(TaskHandle_t*  )&Give_Task_Handle);/* ������ƿ�ָ�� */ 
															
	if(xReturn == pdPASS)/* �����ɹ� */
		printf("Give_Task���񴴽��ɹ�!\n");
	else
		printf("Give_Task���񴴽�ʧ��!\n");
	
	vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
	
	taskEXIT_CRITICAL();            //�˳��ٽ���
}


//��̬�����������Ҫ
///**
//  **********************************************************************
//  * @brief  ��ȡ��������������ջ��������ƿ��ڴ�
//	*					ppxTimerTaskTCBBuffer	:		������ƿ��ڴ�
//	*					ppxTimerTaskStackBuffer	:	�����ջ�ڴ�
//	*					pulTimerTaskStackSize	:		�����ջ��С
//  * @author  fire
//  * @version V1.0
//  * @date    2018-xx-xx
//  **********************************************************************
//  */ 
//void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
//								   StackType_t **ppxIdleTaskStackBuffer, 
//								   uint32_t *pulIdleTaskStackSize)
//{
//	*ppxIdleTaskTCBBuffer=&Idle_Task_TCB;/* ������ƿ��ڴ� */
//	*ppxIdleTaskStackBuffer=Idle_Task_Stack;/* �����ջ�ڴ� */
//	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;/* �����ջ��С */
//}



///**
//  *********************************************************************
//  * @brief  ��ȡ��ʱ������������ջ��������ƿ��ڴ�
//	*					ppxTimerTaskTCBBuffer	:		������ƿ��ڴ�
//	*					ppxTimerTaskStackBuffer	:	�����ջ�ڴ�
//	*					pulTimerTaskStackSize	:		�����ջ��С
//  * @author  fire
//  * @version V1.0
//  * @date    2018-xx-xx
//  **********************************************************************
//  */ 
//void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, 
//									StackType_t **ppxTimerTaskStackBuffer, 
//									uint32_t *pulTimerTaskStackSize)
//{
//	*ppxTimerTaskTCBBuffer=&Timer_Task_TCB;/* ������ƿ��ڴ� */
//	*ppxTimerTaskStackBuffer=Timer_Task_Stack;/* �����ջ�ڴ� */
//	*pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;/* �����ջ��С */
//}
