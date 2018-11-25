#include "app_cfg.h"

void DoorCloseFnct (void *p_tmr,  void *p_arg);
OS_TMR CloseDoorTmr;


void Task_MeasEvent(void *pst_Mod);

//==================================================================================================
//| �������� | Task_MeasSpeed
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��������
//|----------|--------------------------------------------------------------------------------------
//| ������� | �� 
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|-------------------------------------------------------------------------------------- 
//| ������� | wjb
//==================================================================================================
void Task_MeasSpeed (void *p_arg)
{
    OS_ERR err;
    OSTmrCreate(&CloseDoorTmr,          /* p_tmr */
                "MeasSpeedTimeOut",     /* p_name */
                30,                     /* dly 30 * 0.1s = 3s*/
                0,                      /* period */
                OS_OPT_TMR_ONE_SHOT,    /* opt */
                DoorCloseFnct,          /* p_callback */
                0,                      /* p_callback_arg */
                &err);                  /* p_err */
    
    Mod_MeasSpeedInit(&st_MeasSpeed);
    st_MeasSpeed.cb_MeasSpeedEvent = Task_MeasEvent;
    
    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
        OS_ERR  os_err;
        INT16U  uin_MsgSize = 0;
        void * pv_Msg;
        pv_Msg = OSTaskQPend(0, OS_OPT_PEND_BLOCKING, &uin_MsgSize, 0, &os_err);
        
        if(os_err != OS_ERR_NONE)
            continue;
        
        Mod_MeasSpeedPoll(&st_MeasSpeed,pv_Msg);
                        
        TRACE_DBG(">>DBG:       ��������\r\n");        
    }
}


void Task_MeasEvent(void *pst_Mod)
{
    OS_ERR err;
    
    MOD_MEASSPEED_TYPE *p = (MOD_MEASSPEED_TYPE*)pst_Mod;
    
    switch(p->e_State)
    {
    case e_Idle:
        //ת�����״̬ �رճ�ʱ   ������������δ���ڵ�  
        //������ȫ�뿪
        //һ�β������
        Bsp_Sync1(FALSE);
        Bsp_Sync2(FALSE);
        
        OSTmrStop(&CloseDoorTmr,OS_OPT_TMR_NONE,NULL,&err);
        if(err != OS_ERR_NONE)
        {
            
        }
        
        break;
    case e_MeasSpeed:
        //ת�����1������������ ���ڵ�
        //���Ӹոս���
        //���������ʱ��
        OSTmrStart(&CloseDoorTmr,&err);
        if(err != OS_ERR_NONE)
        {

        }
        
        break;
    case e_MeasLenth:
        //ת�����2������������ ���ڵ�
        //������ȫ����
        break;
    case e_Leave:
        //ת�����1���������뿪 ���ڵ�
        //���Ӹ�Ҫ�뿪
        Bsp_Sync1(TRUE);
        Bsp_Sync2(TRUE);
        break;
    default:
        break;
    }

}

void DoorCloseFnct (void *p_tmr,  void *p_arg)
{
    /* Close the door! */
    Mod_MeasSpeedTimeOut(&st_MeasSpeed);
    Bsp_Sync1(FALSE);
    Bsp_Sync2(FALSE);
}

