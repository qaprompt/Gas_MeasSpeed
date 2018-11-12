#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

#include "bsp.h"

#include  <cpu.h>
#include  <lib_math.h>
#include  <lib_mem.h>
#include  <os.h>
#include  <os_app_hooks.h>


#include  "mod_measspeed.h"

#include  "task_cml.h"

#include  "app_save.h"

//==================================================================================
//                                   �������ȼ�    
//==================================================================================
#define  TASK_START_PRIO                OS_CFG_PRIO_MAX-4u   
#define  TASK_MEASSPEED_PRIO             4u

#define  TASK_STDBUS_PRIO               10u

#define  TASK_CML_RECV_PRIO             20u
#define  TASK_CML_SEND_PRIO             21u


//==================================================================================
//                                   �����ջ��С 
//==================================================================================
#define  TASK_START_STK_SIZE            256u
#define  TASK_MEASSPEED_STK_SIZE        512u
#define  TASK_CML_SEND_STK_SIZE         256u
#define  TASK_CML_RECV_STK_SIZE         256u
#define  TASK_STDBUS_STK_SIZE           512u


//==================================================================================
//                                   ������ƿ�����
//==================================================================================
extern  OS_TCB       TaskStartTCB;           /*  ��ʼ����    */
extern  OS_TCB       TaskMeasSpeedTCB;       /*  ��������            */
extern  OS_TCB       TaskCmlSendTCB;         /*  �����е�������    */
extern  OS_TCB       TaskCmlRecvTCB;         /*  �����е�������    */
extern  OS_TCB       TaskStdBusTCB;          /*  STDBUS����    */

//==================================================================================
//                                   �����ջ����
//==================================================================================
extern  CPU_STK      TaskStartStk   [TASK_START_STK_SIZE];                  /*  ��ʼ����    */
extern  CPU_STK      TaskCmlSendStk [TASK_CML_SEND_STK_SIZE];               /*  �����е�������    */
extern  CPU_STK      TaskCmlRecvStk [TASK_CML_RECV_STK_SIZE];               /*  �����е�������    */
extern  CPU_STK      TaskStdBusStk  [TASK_STDBUS_STK_SIZE];                 /*  STDBUS����    */


//==================================================================================
//                                   ����������
//==================================================================================
extern void Task_Start (void  *p_arg);               /*  ��ʼ����    */
extern void Task_TransCml (void  *p_arg);            /*  �����е�������    */
extern void Task_RecvCml (void  *p_arg);             /*  �����е�������    */
extern void Task_StdBus (void  *p_arg);              /*  STDBUS����ͨѶ    */


//==================================================================================
//                                   ��������
//==================================================================================
#if (OS_CFG_Q_EN > 0u)
extern  OS_Q         QCmlRecv;
extern  OS_Q         QCmlTrans;
extern  OS_Q         QSpeRecv;
extern  OS_Q         QSpeTrans;
#endif

//==================================================================================
//                                   �ź���
//==================================================================================
#if (OS_CFG_SEM_EN > 0u)
extern  OS_SEM       AppTaskObjSem;
#endif

//==================================================================================
//                                   �����ź���
//==================================================================================
#if (OS_CFG_MUTEX_EN > 0u)
static  OS_MUTEX     AppTaskObjMutex;
#endif

//==================================================================================
//                                   ����
//==================================================================================
#if (OS_CFG_Q_EN > 0u)
static  OS_Q         AppTaskObjQ;
#endif

//==================================================================================
//                                   �¼����
//==================================================================================
#if (OS_CFG_FLAG_EN > 0u)
static  OS_FLAG_GRP  AppTaskObjFlag;
#endif

//==================================================================================
//                                 ���ٵ�������
//==================================================================================
#ifndef  TRACE_LEVEL_OFF
#define  TRACE_LEVEL_OFF                        0u
#endif

#ifndef  TRACE_LEVEL_INFO
#define  TRACE_LEVEL_INFO                       1u
#endif

#ifndef  TRACE_LEVEL_DBG
#define  TRACE_LEVEL_DBG                        2u
#endif


#define  TRACE_LEVEL                            TRACE_LEVEL_DBG

#if 0
#define  TRACE(x)                               Bsp_Printf(x)//Task_CmlSendMsg(x,strlen(x))   //printf(x)                       

#define  TRACE_INFO(x)                          ((TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(TRACE(x)) : (void)0)
#define  TRACE_DBG(x)                           ((TRACE_LEVEL >= TRACE_LEVEL_DBG)   ? (void)(TRACE(x)) : (void)0)
#endif

#define  TRACE_DBG(...)                         Bsp_Printf(__VA_ARGS__) 

#endif
