#include "app_cfg.h"

//==================================================================================
//                                   ������ƿ鶨��
//==================================================================================
OS_TCB       TaskStartTCB;                                      /*  ��ʼ����            */
OS_TCB       TaskMeasSpeedTCB;                                  /*  ��������            */
OS_TCB       TaskCmlSendTCB;                                    /*  �����е�������      */
OS_TCB       TaskCmlRecvTCB;                                    /*  �����е�������      */
OS_TCB       TaskStdBusTCB;                                     /*  STDBUS����          */

//==================================================================================
//                                   �����ջ����
//==================================================================================
CPU_STK      TaskStartStk       [TASK_START_STK_SIZE];          /*  ��ʼ����            */
CPU_STK      TaskMeasSpeedStk   [TASK_MEASSPEED_STK_SIZE];      /*  ��������            */
CPU_STK      TaskCmlSendStk     [TASK_CML_SEND_STK_SIZE];       /*  �����е�������    */
CPU_STK      TaskCmlRecvStk     [TASK_CML_RECV_STK_SIZE];       /*  �����е�������    */
CPU_STK      TaskStdBusStk      [TASK_STDBUS_STK_SIZE];         /*  STDBUS����        */
//==================================================================================
//                                   ����������
//==================================================================================
void Task_Start (void *p_arg);                                  /*  ��ʼ����    */
void Task_MeasSpeed (void  *p_arg);                             /*  ��������            */
void Task_CmlSend (void  *p_arg);                               /*  �����е�������    */
void Task_CmlRecv (void  *p_arg);                               /*  �����е�������    */
void Task_StdBus (void *p_arg);                                 /*  STDBUS����    */

/*  �ź���     */
#if (OS_CFG_SEM_EN > 0u)
OS_SEM       Sem_CmlSend;                                       /*  �����з��ͼ����ź���    */
#endif


/*  ��������  */
#if (OS_CFG_Q_EN > 0u)
OS_Q         QCmlRecv;
OS_Q         QCmlTrans;
OS_Q         QSpeRecv;
OS_Q         QSpeTrans;
#endif

void AppTaskCreate (void);
void AppObjCreate (void);

//==================================================================================================
//| �������� | Task_Start
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��ʼ����
//|----------|--------------------------------------------------------------------------------------
//| ������� | �� 
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|-------------------------------------------------------------------------------------- 
//| ������� | wjb
//==================================================================================================
void Task_Start (void *p_arg)
{
    OS_ERR  os_err;


   (void)p_arg;

    Bsp_Init();                                                 /* Initialize BSP functions                             */
    //Bsp_TimeSampleInit();
    
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&os_err);                            /* ����ͳ������                                         */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();                                /* �����жϹر�ʱ��                                     */
#endif
    
    AppObjCreate();                                             /* �����ں˶���                                         */
    
    AppTaskCreate();                                            /* ��������                                             */

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
           
        static BOOL b_Led = FALSE;
        
        OSTimeDlyHMSM(0u, 0u, 1u, 0u,
                      OS_OPT_TIME_HMSM_STRICT,
                      &os_err);
        
        //TRACE_DBG(">>DBG:       ��ʼ����\r\n"); 
        
        b_Led = ~b_Led;
        Bsp_RunLed(b_Led);
        Bsp_Laser1(TRUE);
        Bsp_Laser2(TRUE);
        Bsp_Led1(b_Led);
        Bsp_Led2(b_Led);
        
        
#if 0
        {
            static BSP_TIMESAMPLE_TYPE   st_Timing[2] = {0};
            FP32   f_IntervalMs = 0;        
            Bsp_GetTimeSample(&st_Timing[0]);
            Bsp_DelayUs(20);
            Bsp_GetTimeSample(&st_Timing[1]);
            
            f_IntervalMs = Bsp_GetInterval(&st_Timing[0],&st_Timing[1]);
            TRACE_DBG(">>DBG:       ���ʱ�� = %f\r\n",f_IntervalMs);        
        }
#endif

    }
}

//==================================================================================================
//| �������� | AppTaskCreate
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��������
//|----------|--------------------------------------------------------------------------------------
//| ������� | �� 
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|-------------------------------------------------------------------------------------- 
//| ������� | wjb
//==================================================================================================
void AppTaskCreate (void)
{
    OS_ERR  os_err;
    
    /* �����ȼ��ӵ͵��߿�ʼ���� */

    /* �����е��� */
    OSTaskCreate((OS_TCB       *)&TaskCmlSendTCB,                               /* ����������ƿ� */
                 (CPU_CHAR     *)"Debug Task",                                  /* �������� */
                 (OS_TASK_PTR   )Task_CmlSend,                                  /* ������ */    
                 (void         *)0u,                                            /* ������� */
                 (OS_PRIO       )TASK_CML_SEND_PRIO,                            /* �������ȼ� */
                 (CPU_STK      *)&TaskCmlSendStk[0u],                           /* ������ص�ַ */    
                 (CPU_STK_SIZE  )TASK_CML_SEND_STK_SIZE / 10u,                  /* ����ջ������ */        
                 (CPU_STK_SIZE  )TASK_CML_SEND_STK_SIZE,                        /* �����ջ��С */ 
                 (OS_MSG_QTY    )25u,                                          /* �ڲ���Ϣ���е������Ϣ��Ŀ */
                 (OS_TICK       )0u,                                            /* ʱ��Ƭ��ѯ��ʱ��Ƭ�� */
                 (void         *)0u,                                            /* �û�����洢�� */
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR       *)&os_err);                                      /* ��Ŵ���ֵ */ 

    /* STDBUS */
    OSTaskCreate((OS_TCB       *)&TaskStdBusTCB,                                /* ����������ƿ� */
                 (CPU_CHAR     *)"StdBus Task",                                 /* �������� */
                 (OS_TASK_PTR   )Task_StdBus,                                   /* ������ */    
                 (void         *)0u,                                            /* ������� */
                 (OS_PRIO       )TASK_STDBUS_PRIO,                              /* �������ȼ� */
                 (CPU_STK      *)&TaskStdBusStk[0u],                            /* ������ص�ַ */    
                 (CPU_STK_SIZE  )TASK_STDBUS_STK_SIZE / 10u,                    /* ����ջ������ */        
                 (CPU_STK_SIZE  )TASK_STDBUS_STK_SIZE,                          /* �����ջ��С */ 
                 (OS_MSG_QTY    )100u,                                           /* �ڲ���Ϣ���е������Ϣ��Ŀ */
                 (OS_TICK       )0u,                                            /* ʱ��Ƭ��ѯ��ʱ��Ƭ�� */
                 (void         *)0u,                                            /* �û�����洢�� */
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR       *)&os_err);                                      /* ��Ŵ���ֵ */
    

    /* ���ٴ������� */
    OSTaskCreate((OS_TCB       *)&TaskMeasSpeedTCB,                               /* ����������ƿ� */
                 (CPU_CHAR     *)"Meas Speed",                                 /* �������� */
                 (OS_TASK_PTR   )Task_MeasSpeed,                                  /* ������ */    
                 (void         *)0u,                                            /* ������� */
                 (OS_PRIO       )TASK_MEASSPEED_PRIO,                             /* �������ȼ� */
                 (CPU_STK      *)&TaskMeasSpeedStk[0u],                           /* ������ص�ַ */    
                 (CPU_STK_SIZE  )TASK_MEASSPEED_STK_SIZE / 10u,                   /* ����ջ������ */        
                 (CPU_STK_SIZE  )TASK_MEASSPEED_STK_SIZE,                         /* �����ջ��С */ 
                 (OS_MSG_QTY    )10u,                                           /* �ڲ���Ϣ���е������Ϣ��Ŀ */
                 (OS_TICK       )0u,                                            /* ʱ��Ƭ��ѯ��ʱ��Ƭ�� */
                 (void         *)0u,                                            /* �û�����洢�� */
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR       *)&os_err);                                      /* ��Ŵ���ֵ */
#if 0 
    /* �Ҷȴ��� */
    OSTaskCreate((OS_TCB       *)&TaskGreyProcTCB,                              /* ����������ƿ� */
                 (CPU_CHAR     *)"Grey Process",                                /* �������� */
                 (OS_TASK_PTR   )Task_GreyProc,                                 /* ������ */    
                 (void         *)0u,                                            /* ������� */
                 (OS_PRIO       )TASK_GREYPROC_PRIO,                            /* �������ȼ� */
                 (CPU_STK      *)&TaskGreyProcStk[0u],                          /* ������ص�ַ */    
                 (CPU_STK_SIZE  )TASK_GREYPROC_STK_SIZE / 10u,                  /* ����ջ������ */        
                 (CPU_STK_SIZE  )TASK_GREYPROC_STK_SIZE,                        /* �����ջ��С */ 
                 (OS_MSG_QTY    )10u,                                           /* �ڲ���Ϣ���е������Ϣ��Ŀ */
                 (OS_TICK       )0u,                                            /* ʱ��Ƭ��ѯ��ʱ��Ƭ�� */
                 (void         *)0u,                                            /* �û�����洢�� */
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR       *)&os_err);                                      /* ��Ŵ���ֵ */
    
    
    /* ������ͨ�� */
    OSTaskCreate((OS_TCB       *)&TaskUsbHostTCB,                               /* ����������ƿ� */
                 (CPU_CHAR     *)"USB HOST",                                    /* �������� */
                 (OS_TASK_PTR   )Task_UsbHost,                                  /* ������ */    
                 (void         *)0u,                                            /* ������� */
                 (OS_PRIO       )TASK_USB_HOST_PRIO,                            /* �������ȼ� */
                 (CPU_STK      *)&TaskUsbHostStk[0u],                           /* ������ص�ַ */    
                 (CPU_STK_SIZE  )TASK_USB_HOST_STK_SIZE / 10u,                  /* ����ջ������ */        
                 (CPU_STK_SIZE  )TASK_USB_HOST_STK_SIZE,                        /* �����ջ��С */ 
                 (OS_MSG_QTY    )10u,                                           /* �ڲ���Ϣ���е������Ϣ��Ŀ */
                 (OS_TICK       )0u,                                            /* ʱ��Ƭ��ѯ��ʱ��Ƭ�� */
                 (void         *)0u,                                            /* �û�����洢�� */
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR       *)&os_err);                                      /* ��Ŵ���ֵ */ 


    /* �����е��� */
    OSTaskCreate((OS_TCB       *)&AppTaskCmlRecvTCB,                           /* ����������ƿ� */
                 (CPU_CHAR     *)"App Task CML Recv",                          /* �������� */
                 (OS_TASK_PTR   )AppTaskCmlRecv,                               /* ������ */    
                 (void         *)0u,                                            /* ������� */
                 (OS_PRIO       )APP_CFG_TASK_CML_RECV_PRIO,                   /* �������ȼ� */
                 (CPU_STK      *)&AppTaskCmlRecvStk[0u],                       /* ������ص�ַ */    
                 (CPU_STK_SIZE  )APP_CFG_TASK_CML_RECV_STK_SIZE / 10u,         /* ����ջ������ */        
                 (CPU_STK_SIZE  )APP_CFG_TASK_CML_RECV_STK_SIZE,               /* �����ջ��С */ 
                 (OS_MSG_QTY    )0u,                                            /* �ڲ���Ϣ���е������Ϣ��Ŀ */
                 (OS_TICK       )0u,                                            /* ʱ��Ƭ��ѯ��ʱ��Ƭ�� */
                 (void         *)0u,                                            /* �û�����洢�� */
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR       *)&os_err);                                      /* ��Ŵ���ֵ */ 
#endif

}

//==================================================================================================
//| �������� | AppObjCreate
//|----------|--------------------------------------------------------------------------------------
//| �������� | �����ں˶���
//|----------|--------------------------------------------------------------------------------------
//| ������� | �� 
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|-------------------------------------------------------------------------------------- 
//| ������� | wjb
//==================================================================================================
void AppObjCreate (void)
{
    OS_ERR  os_err;
#if (OS_CFG_SEM_EN > 0u)
    OSSemCreate(&Sem_CmlSend,
                "Sem Test",
                 1u,
                 &os_err);
#endif
    
#if (OS_CFG_Q_EN > 0u)
    OSQCreate(&QCmlRecv,
              "Queue Command Line Recv",
               10,
              &os_err);
    
    OSQCreate(&QCmlTrans,
              "Queue Command Line Trans",
               10,
              &os_err);
    
    OSQCreate(&QSpeRecv,
              "Queue Spectrometer Recv",
               5,
              &os_err);
    
    OSQCreate(&QSpeTrans,
              "Queue Spectrometer Trans",
               5,
              &os_err);
#endif
}

