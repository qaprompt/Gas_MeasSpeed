#ifndef __MOD_MEASSPEED_H__
#define __MOD_MEASSPEED_H__

#include "app_cfg.h"

#define     MOD_MEASSPEED_DEBUG     (TRUE)

typedef enum {
    e_Idle, e_MeasSpeed, e_MeasLenth, e_Leave,
}MOD_MEASSTATE_TYPE;

typedef enum {
    e_Null, e_Sense1ToSense2,   e_Sense2ToSense1,
}MOD_MEASSDIRCTION_TYPE;

typedef struct{
    INT16U  uin_Id;
    INT16U  uin_UsePinMask;      //ʹ�õ�����
    INT16U  uin_PinStatus;       //���ŵ�״̬
    INT32U  ul_RisingThreshold;  //�л����г�״̬����ֵ ��⵽���ڵ��Ĵ���������
    INT32U  ul_FallThreshold;    //�л���û��״̬����ֵ ��⵽δ�ڵ��Ĵ���������
    
    BOOL    b_Status;           //FALSE:û�� TRUE:�г�
    INT32U  ul_Count;
    BSP_TIMESAMPLE_TYPE st_RisingTs;
    BSP_TIMESAMPLE_TYPE st_FallTs;
}MOD_MEASSENSN_TYPE;

typedef struct{
    MOD_MEASSENSN_TYPE*     pst_Sense1;
    MOD_MEASSENSN_TYPE*     pst_Sense2;
    MOD_MEASSTATE_TYPE      e_State;
    MOD_MEASSDIRCTION_TYPE  e_Dirction;
    INT32U  ul_Count;
    INT32U  ul_Interval;
    
    BSP_TIMESAMPLE_TYPE     st_Ts1;
    BSP_TIMESAMPLE_TYPE     st_Ts2;
    BSP_TIMESAMPLE_TYPE     st_Ts3;
    BSP_TIMESAMPLE_TYPE     st_Ts4;
    
    INT32U  ul_Lenth;
    INT32U  ul_IntervalTime_us; //���β��ٵļ��ʱ��
    INT32U  ul_Speed1_mph;      //�൱�� km/h�Ŵ�1000�� ����3��С����
    INT32U  ul_Speed2_mph;
    INT32U  ul_SpeedAvg_mph;
    INT32U  ul_Acc_mps2;
    
    void(*cb_MeasSpeedEvent)(void *pst_Mod);
}MOD_MEASSPEED_TYPE;

extern MOD_MEASSENSN_TYPE st_LaserSense1;
extern MOD_MEASSENSN_TYPE st_LaserSense2;
extern MOD_MEASSPEED_TYPE st_MeasSpeed;

void Mod_MeasSpeedPoll(MOD_MEASSPEED_TYPE *psd_Mod,BSP_GPIOEVENT_TYPE* pst_GpioEvent);

#endif
