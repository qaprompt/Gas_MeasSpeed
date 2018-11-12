#include  "bsp.h"



void Bsp_DelayUs(uint32_t us)
{
    uint32_t ticks;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt = 0;
    uint32_t reload;
       
    reload = SysTick->LOAD;                
    ticks = us * (SystemCoreClock / 1000000);	    //������Ҫ��ʱ��Tick��

    tcnt = 0;
    told = SysTick->VAL;             

    while (tcnt < ticks)             
    {
        tnow = SysTick->VAL;                        //��ȡ�µ�Tickֵ
        if (tnow != told)
        {
              if (tnow < told)                      //�µ�Tickֵ < �ϵ�Tickֵ ����  SysTick��һ���ݼ��ļ�����             
              {
                  tcnt += told - tnow;              //�ۼ����ε�Tick����
              }
              else
              {
                  tcnt += reload - tnow + told;     //�µ�Tickֵ > �ϵ�Tickֵ ���������
              }        
              told = tnow;
        }  
    }
} 

void Bsp_DelayMs(INT32U ms)
{
    while(ms--)
        Bsp_DelayUs(1000);
}

static INT32U ul_OffsetUs = 0;
void Bsp_TimeSampleInit(void)
{   
    //��ȡ������ʱ���������Ҫ��ʱ��
    BSP_TIMESAMPLE_TYPE   st_Timing[2] = {0};
    INT32U i=0,j=0;
    ul_OffsetUs = 0;
    for(i = 0; i < 10; i++)
    {
        Bsp_IntDis();
        Bsp_GetTimeSample(&st_Timing[0]);
        Bsp_GetTimeSample(&st_Timing[1]);
        j += Bsp_GetInterval(&st_Timing[0],&st_Timing[1]);
        Bsp_IntEn();
    }
    ul_OffsetUs = j/10;
    TRACE_DBG(">>DBG:       ƫ��ʱ�� = %f\r\n",ul_OffsetUs);
    
}

void Bsp_GetTimeSample(BSP_TIMESAMPLE_TYPE* pst_Ts)
{
    INT32U ul_Cnt0 = 0;
    INT32U ul_Cnt1 = 0;
    INT32U ul_Tick0 = 0;
    INT32U ul_Tick1 = 0;
    
    ul_Tick0 = HAL_GetTick();   //��ȡSysTick���жϼ��� �����ж����������ı�      1
    ul_Cnt0 = SysTick->VAL;     //�Ȼ�ȡһ�μ���ֵ                                  2      
    Bsp_IntDis();               //                                                  3
    ul_Tick1 = HAL_GetTick();   //��ȡSysTick���жϼ��� �����ж����������ı�      4
    ul_Cnt1 = SysTick->VAL;     //�ٻ�ȡһ�μ���ֵ                                  5
    Bsp_IntEn();                //                                                  6
      
    /* Systick �Ǹ��ݼ��ļ����� */
    if(ul_Tick1 > ul_Tick0 || ul_Tick1 < ul_Tick0 )
    {
        /* ��1 2�䷢�����ж� */
        pst_Ts->ul_Cnt = 0;     
        pst_Ts->ul_Tick = ul_Tick1;
    }
    else if(ul_Tick1 == ul_Tick0 && ul_Cnt1 > ul_Cnt0 )
    {
        /* ��3 4�䷢�����ж� Tickֵû�б����� Cntֵ������ */
        pst_Ts->ul_Cnt = 0;
        pst_Ts->ul_Tick = ul_Tick0+1;
    }
    else //if(ul_Tick1 == ul_Tick0 && ul_Cnt1 < ul_Cnt0 )
    {
        /* һ������ ����Tick�м�һ��û�з����ж� */
        pst_Ts->ul_Cnt = SysTick->LOAD - ul_Cnt0;       //Systick��һ��һ���ݼ������� ����ֵ-��ǰֵ = �Ѿ���������ֵ
        pst_Ts->ul_Tick = ul_Tick1; //����Tick��һ����
    }
}

INT32U Bsp_GetInterval(BSP_TIMESAMPLE_TYPE* pst_TsOld,BSP_TIMESAMPLE_TYPE * pst_TsNew)
{    
    //2^32 = 4294967296Us = 4294967.296Ms =  4294.967296S
    INT32U  ul_Freq = HAL_RCC_GetSysClockFreq() / 1000000;
    INT32U  ul_IntervalUs = (pst_TsNew->ul_Cnt - pst_TsOld->ul_Cnt) / ul_Freq;
    INT32U  ul_IntervalMs = pst_TsNew->ul_Tick - pst_TsOld->ul_Tick;

    if(ul_IntervalMs >= 4294965) //7.296 - 5  = 2.296Ms����
    {
        return 4294965000ul;//4294965 * 1000
    }
    
    ul_IntervalUs += ul_IntervalMs*1000;
    
    if(ul_IntervalUs <= ul_OffsetUs)
        return 0;
    else
        return ul_IntervalUs - ul_OffsetUs;
}

void HAL_Delay(INT32U Delay)
{
    Bsp_DelayMs(Delay);
}

