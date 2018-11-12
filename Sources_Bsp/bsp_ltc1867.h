//==================================================================================================
//| 文件名称 | Bsp_LT1867.c
//|--------- |--------------------------------------------------------------------------------------
//| 文件描述 | 板级LT1867驱动 STM32版本
//|--------- |--------------------------------------------------------------------------------------
//| 版权声明 | 
//|----------|--------------------------------------------------------------------------------------
//|  版本    |  时间       |  作者     | 描述
//|--------- |-------------|-----------|------------------------------------------------------------
//|  V1.0    | 2018.10.31  |  wjb      | 初版
//==================================================================================================

#ifndef __BSP_LTC1867_H__
#define __BSP_LTC1867_H__

#include "Bsp.h"

typedef struct {
    void * pv_SpiHandle;
    void (*CS)(BOOL);
}DEV_LTC1867;

extern DEV_LTC1867 st_LTC1867A;
extern DEV_LTC1867 st_LTC1867B;

void Bsp_LTC1867Init(void* pv_Dev);

INT16U Bsp_LTC1867SampleOne(void* pv_Dev,INT16U uin_CH);

INT16U Bsp_LTC1867SampleAvg(void* pv_Dev,INT16U uin_CH,INT16U uin_Avg);

FP64 Bsp_LTC1867HexToVolt(INT16U hex);

#endif /* _BSP_LTC1867_H_ */
