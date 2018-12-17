#include "mod_measspeed.h"

/*
    ���ڵ�->�ߵ�ƽ->������       ���ڵ�->�͵�ƽ->�½���
    
    s1 -> ������1���ڵ�   ������2δ�ڵ�     ->��⵽����
    s2 -> ������1���ڵ�   ������2���ڵ�     ->���������     ����������װ���� / ������1�봫����2���ʱ��
    s3 -> ������1δ�ڵ�   ������2���ڵ�     ->���������     ������1�봫����1���ʱ�� * ����
    s4 -> ������1δ�ڵ�   ������2δ�ڵ�     ->��⳵��ȥ
    
    240km/h = 4km/min = 66.66m/s = 0.066m/ms;

    �������������270*2 = 540mm?
    540mm = 0.540m
    0.540m/10ms = 54m/s = 194km/h
    0.540m/9ms  = 54m/s = 194km/h
*/

#if (MOD_MEASSPEED_DEBUG != FALSE)
    #define S_TRACE_DBG(...)      TRACE_DBG(__VA_ARGS__)
#else
    #define S_TRACE_DBG(...)      (void)(0)
#endif

void Mod_SaveTsAtoTsB(BSP_TIMESAMPLE_TYPE* pst_A,BSP_TIMESAMPLE_TYPE* pst_B)
{
    memcpy(pst_B,pst_A,sizeof(BSP_TIMESAMPLE_TYPE));
}

MOD_MEASSENSN_TYPE st_Sense1 = {
    .uin_Id = 1,
    .uin_UsePinMask = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|
                      GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7,   //ʹ�õ�����
//    .uin_UsePinMask = GPIO_PIN_0,   //ʹ�õ�����
    .uin_PinStatus = 0,//���ŵ�״̬
    .ul_RisingThreshold = 1,    //�л����г�״̬����ֵ ��⵽���ڵ��Ĵ���������
    .ul_FallThreshold =1,       //�л���û��״̬����ֵ ��⵽δ�ڵ��Ĵ���������
    
    .b_Status = FALSE,           //FALSE:û�� TRUE:�г�
    .ul_Count = 0,
    .st_RisingTs = {0,0},
    .st_FallTs = {0,0},
};

MOD_MEASSENSN_TYPE st_Sense2 = {
    .uin_Id = 2,
    .uin_UsePinMask = GPIO_PIN_8|GPIO_PIN_9 |GPIO_PIN_10|GPIO_PIN_11|
                      GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15,   //ʹ�õ�����
//    .uin_UsePinMask = GPIO_PIN_8,   //ʹ�õ�����
    .uin_PinStatus = 0,         //���ŵ�״̬
    .ul_RisingThreshold = 1,    //�л����г�״̬����ֵ ��⵽���ڵ��Ĵ���������
    .ul_FallThreshold = 1,       //�л���û��״̬����ֵ ��⵽δ�ڵ��Ĵ���������
    
    .b_Status = FALSE,          //FALSE:���ź� TRUE:���ź�
    .ul_Count = 0,
    .st_RisingTs = {0,0},
    .st_FallTs = {0,0},
};

MOD_MEASSPEED_TYPE st_MeasSpeed = {
    .pst_Sense1 = &st_Sense1,
    .pst_Sense2 = &st_Sense2,
    .e_State    = e_Idle,
    .e_Dirction = e_Null,
    

    .ul_LeaveDelayMs = 5,
    
    .st_Ts1     = 0,
    .st_Ts2     = 0,
    .st_Ts3     = 0,
    .st_Ts4     = 0,
    
    .ul_Lenth   = 540,
    
    .ul_Count = 0,
    .ul_Speed_mph  = 0,
    .ul_Acc_mps2    = 0,
    .cb_MeasSpeedEvent = NULL,
};

void Mod_SenseEnableRising(MOD_MEASSENSN_TYPE* pst_Sense)
{
    GPIO_InitTypeDef  gpio_init;
    gpio_init.Pin = pst_Sense->uin_UsePinMask;
    gpio_init.Mode = GPIO_MODE_IT_RISING;//GPIO_MODE_IT_RISING_FALLING;
    gpio_init.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOD, &gpio_init);
}

void Mod_SenseEnableFalling(MOD_MEASSENSN_TYPE* pst_Sense)
{
    GPIO_InitTypeDef  gpio_init;
    gpio_init.Pin = pst_Sense->uin_UsePinMask;
    gpio_init.Mode = GPIO_MODE_IT_FALLING;//GPIO_MODE_IT_RISING_FALLING;
    gpio_init.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOD, &gpio_init);
}


void Mod_SenseEventHandle(MOD_MEASSENSN_TYPE* pst_Sense,BSP_GPIOEVENT_TYPE* pst_GpioEvent)
{   
    /* �ж��Ƿ���ʹ�õ����� */
    if((pst_Sense->uin_UsePinMask & pst_GpioEvent->uin_GpioPin) != 0)
    {
        if(pst_Sense->b_Status == TRUE)
        {
            if(HAL_GPIO_ReadPin(pst_GpioEvent->vp_GpioPort,pst_GpioEvent->uin_GpioPin) == 0)    //��ȡ��ƽ����
            {
                if(pst_GpioEvent->b_IsRising == FALSE)      //����½���
                {
                    if(++pst_Sense->ul_Count >= pst_Sense->ul_FallThreshold)
                    {
                        pst_Sense->b_Status = FALSE;
                        pst_Sense->ul_Count = 0;
                        Mod_SaveTsAtoTsB(pst_GpioEvent->st_Ts,
                                         &pst_Sense->st_FallTs);
                        Mod_SenseEnableRising(pst_Sense);  //�л������������

                    }   
                }
            }
        }
        else
        {
            if(HAL_GPIO_ReadPin(pst_GpioEvent->vp_GpioPort,pst_GpioEvent->uin_GpioPin) == 1)    //��ȡ��ƽ����
            {
                if(pst_GpioEvent->b_IsRising == TRUE)       //���������
                {
                    if(++pst_Sense->ul_Count >= pst_Sense->ul_RisingThreshold)
                    {
                        pst_Sense->b_Status = TRUE;
                        pst_Sense->ul_Count = 0;
                        Mod_SaveTsAtoTsB(pst_GpioEvent->st_Ts,
                                         &pst_Sense->st_RisingTs);
                        Mod_SenseEnableFalling(pst_Sense);  //�л�������½���
                    }
                }
            }
        }
    } 
}

void Mod_SenseSetLow(MOD_MEASSENSN_TYPE* pst_Sense)
{
    pst_Sense->b_Status = FALSE;
    pst_Sense->ul_Count = 0;
    Mod_SenseEnableRising(pst_Sense);  //�л������������
}

void Mod_SenseSetHigh(MOD_MEASSENSN_TYPE* pst_Sense)
{
    pst_Sense->b_Status = TRUE;
    pst_Sense->ul_Count = 0;
    Mod_SenseEnableFalling(pst_Sense);  //�л�������½���
}

void Mod_MeasSpeedInit(MOD_MEASSPEED_TYPE *pst_Mod)
{
    FP32 af_LowVolt[13] = {0};
    FP32 af_HightVolt[13] = {0};
    
    Mod_SenseSetLow(pst_Mod->pst_Sense1);       //����Ϊ�͵�ƽ״̬�ȴ�������
    Mod_SenseSetLow(pst_Mod->pst_Sense2);       //����Ϊ�͵�ƽ״̬�ȴ�������
    
    Bsp_Laser1(e_LaserOff);
    Bsp_Laser2(e_LaserOff); 
    
    Bsp_DelayMs(100);
    memcpy(af_LowVolt,(const void *)af_ADCVoltFilte,sizeof(af_LowVolt));
    
    Bsp_Laser1(e_LaserOn);
    Bsp_Laser2(e_LaserOn);
    
    Bsp_DelayMs(100);
    memcpy(af_HightVolt,(const void *)af_ADCVoltFilte,sizeof(af_HightVolt));
}

void Mod_MeasSpeedCal(MOD_MEASSPEED_TYPE *pst_Mod)
{
    /* lenth/ts2-ts1 = speed1 lenth/ts4-ts3 = speed2 */
    /* 1. um/us = m/s  2.m/s * 3600 = m/h */
  
    INT32U ul_IntervalUs1 = 0;
    INT32U ul_IntervalUs2 = 0;
    INT32U ul_IntervalUs3 = 0;
    FP64 f_mps1,f_mps2;
    ul_IntervalUs1 = Bsp_GetInterval(&pst_Mod->st_Ts1,&pst_Mod->st_Ts2);
    f_mps1 = pst_Mod->ul_Lenth * 1000.0 / ul_IntervalUs1; 
  
    ul_IntervalUs2 = Bsp_GetInterval(&pst_Mod->st_Ts3,&pst_Mod->st_Ts4);
    f_mps2 = pst_Mod->ul_Lenth * 1000.0 / ul_IntervalUs2;
    
    ul_IntervalUs3 = Bsp_GetInterval(&pst_Mod->st_Ts2,&pst_Mod->st_Ts3);
    ul_IntervalUs3 = ul_IntervalUs3 + (ul_IntervalUs1 + ul_IntervalUs2) / 2;
    
    pst_Mod->ul_Acc_mps2 = (f_mps2 - f_mps1) * 1000000 / ul_IntervalUs3;
    
    pst_Mod->ul_Speed_mph = (f_mps2 + f_mps1) * 3600/2;

    if(pst_Mod->pst_Sense1->uin_Id == 1)
    {
        pst_Mod->e_Dirction = e_Sense1ToSense2;
    }
    else if(pst_Mod->pst_Sense1->uin_Id == 2)
    {
        pst_Mod->e_Dirction = e_Sense2ToSense1;
    }
    pst_Mod->ul_Count++;
    
    
    S_TRACE_DBG(">>MeasSpeed:    �������\r\n");

    S_TRACE_DBG(">>MeasSpeed:    �������%u\r\n",pst_Mod->ul_Count);
    S_TRACE_DBG(">>MeasSpeed:    ��������%u\r\n",pst_Mod->e_Dirction);
    S_TRACE_DBG(">>MeasSpeed:    �����ٶ�%f(m/h)\r\n",pst_Mod->ul_Speed_mph);
    S_TRACE_DBG(">>MeasSpeed:    �������ٶ�%f(m/s^2)\r\n",pst_Mod->ul_Acc_mps2);
}

void Mod_MeasSpeedPoll(MOD_MEASSPEED_TYPE *pst_Mod,BSP_GPIOEVENT_TYPE* pst_GpioEvent)
{        
    Mod_SenseEventHandle(pst_Mod->pst_Sense1,pst_GpioEvent);
    Mod_SenseEventHandle(pst_Mod->pst_Sense2,pst_GpioEvent);
    switch (pst_Mod->e_State)
    {
    case e_Idle:        //�ȴ���һ��������  
        if(pst_Mod->pst_Sense1->b_Status == TRUE && pst_Mod->pst_Sense2->b_Status == TRUE)
        {
            //���߶����ź�
            Mod_SenseSetLow(pst_Mod->pst_Sense1);       //����Ϊ�͵�ƽ״̬�ȴ�������
            Mod_SenseSetLow(pst_Mod->pst_Sense2);       //����Ϊ�͵�ƽ״̬�ȴ�������
            S_TRACE_DBG(">>MeasSpeed:   ���߶����ź�\r\n");
        }
        else if(pst_Mod->pst_Sense1->b_Status == TRUE && pst_Mod->pst_Sense2->b_Status == FALSE)
        {
            Mod_SaveTsAtoTsB(&pst_Mod->pst_Sense1->st_RisingTs,
                             &pst_Mod->st_Ts1);
            pst_Mod->e_State = e_MeasSpeed;
            
            if( pst_Mod->cb_MeasSpeedEvent != NULL)
                pst_Mod->cb_MeasSpeedEvent(pst_Mod);
            
            S_TRACE_DBG(">>MeasSpeed:   ��һ�������� ������%u����\r\n",pst_Mod->pst_Sense1->uin_Id);
        }
        else if(pst_Mod->pst_Sense1->b_Status == FALSE && pst_Mod->pst_Sense2->b_Status == TRUE)
        {
            /* ����ָ�� Sense1�������ȴ������Ǹ� */
            void* p = pst_Mod->pst_Sense2;
            pst_Mod->pst_Sense2 = pst_Mod->pst_Sense1;
            pst_Mod->pst_Sense1 = p;
          
            Mod_SaveTsAtoTsB(&pst_Mod->pst_Sense1->st_RisingTs,
                             &pst_Mod->st_Ts1);
            pst_Mod->e_State = e_MeasSpeed;
            
            if( pst_Mod->cb_MeasSpeedEvent != NULL)
                pst_Mod->cb_MeasSpeedEvent(pst_Mod);
            
            S_TRACE_DBG(">>MeasSpeed:    ��һ�������� ������%u����\r\n",pst_Mod->pst_Sense1->uin_Id);
        }
        break;
    case e_MeasSpeed:       //�ȴ��ڶ���������
        if(pst_Mod->pst_Sense2->b_Status == TRUE)
        {
            Mod_SaveTsAtoTsB(&pst_Mod->pst_Sense2->st_RisingTs,
                             &pst_Mod->st_Ts2); 
            pst_Mod->e_State = e_MeasLenth;
            
            if( pst_Mod->cb_MeasSpeedEvent != NULL)
                pst_Mod->cb_MeasSpeedEvent(pst_Mod);
            
            S_TRACE_DBG(">>MeasSpeed:   �ڶ��������� ������%u����\r\n",pst_Mod->pst_Sense2->uin_Id);
        }
        break;
    case e_MeasLenth:       //�ȴ���һ��������
        if(pst_Mod->pst_Sense1->b_Status == FALSE)
        {
            Mod_SaveTsAtoTsB(&pst_Mod->pst_Sense1->st_FallTs,
                             &pst_Mod->st_Ts3);
            pst_Mod->e_State = e_Leave;
            
            if( pst_Mod->cb_MeasSpeedEvent != NULL)
                pst_Mod->cb_MeasSpeedEvent(pst_Mod);
            
            S_TRACE_DBG(">>MeasSpeed:   ��һ�������� ������%u����\r\n",pst_Mod->pst_Sense1->uin_Id);
        }
        break;
    case e_Leave:           //
        if(pst_Mod->pst_Sense2->b_Status == FALSE)
        {
            Mod_SaveTsAtoTsB(&pst_Mod->pst_Sense2->st_FallTs,
                             &pst_Mod->st_Ts4);

            pst_Mod->e_State = e_Idle;

            Mod_MeasSpeedCal(pst_Mod);  //�����ٶȼ��ٶ�
            
            if( pst_Mod->cb_MeasSpeedEvent != NULL)
                pst_Mod->cb_MeasSpeedEvent(pst_Mod);  
            
            if(pst_Mod->ul_LeaveDelayMs > 0 )
            {
                OS_ERR os_err;
                OSTimeDlyHMSM(0u, 0u, 0u, pst_Mod->ul_LeaveDelayMs,
                              OS_OPT_TIME_HMSM_NON_STRICT,
                              &os_err);
            }
            
            S_TRACE_DBG(">>MeasSpeed:   �ڶ��������� ������%u����\r\n",pst_Mod->pst_Sense2->uin_Id);
        }
        break;
    default:
        break;
    }
}

void Mod_MeasSpeedTimeOut(MOD_MEASSPEED_TYPE *pst_Mod)
{
    pst_Mod->e_State    = e_Idle;
    pst_Mod->e_Dirction = e_Null;
    Mod_SenseSetLow(pst_Mod->pst_Sense1);       //����Ϊ�͵�ƽ״̬�ȴ�������
    Mod_SenseSetLow(pst_Mod->pst_Sense2);       //����Ϊ�͵�ƽ״̬�ȴ�������
}