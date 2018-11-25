//==================================================================================================
//| �ļ����� | Bsp.h
//|--------- |--------------------------------------------------------------------------------------
//| �ļ����� | Bsp.c����Ӧ��ͷ�ļ�
//|--------- |--------------------------------------------------------------------------------------
//| ��Ȩ���� | 
//|----------|--------------------------------------------------------------------------------------
//|  �汾    |  ʱ��       |  ����     | ����
//|--------- |-------------|-----------|------------------------------------------------------------
//|  V1.0    | 2018.10.31  |  wjb      | ����V1.0
//|  V1.01   | 2018.10.31  |  wjb      | �ڳ��������ע�� ��ӿ����ж� �� ��ӡ���� V1.01    
//==================================================================================================
#ifndef  __BSP_H__
#define  __BSP_H__

#ifdef __cplusplus
extern  "C" {                                  /* See Note #1.                                         */
#endif
  
#ifdef      OS_SUPPROT
#include  "bsp_os.h"
#include  <cpu_core.h>
#endif
  
#include  <stdarg.h>
#include  <stdint.h>
#include  <stdio.h>
#include  <string.h>
#include  <math.h>
#include  <stm32f1xx_hal.h>
#include  "stm32f1xx_ll_adc.h"
#include  "stm32f1xx_ll_usart.h"
#include  "stm32f1xx_ll_spi.h"

#include  "bsp_datatype.h"

#include  "bsp_converter.h"

#include  "bsp_timer.h"
#include  "bsp_gpio.h"
#include  "bsp_clock.h"
#include  "bsp_usart.h"
#include  "bsp_spi.h"
#include  "bsp_dac.h"
#include  "bsp_adc.h"
  
 
  
#include  <app_cfg.h>
  
  
/*__CMSIS_IAR_H__ ���г��õĺ궨�� */
  
void Bsp_Init(void);

void Bsp_Init(void);

__STATIC_INLINE void Bsp_Nop(void){;}
__STATIC_INLINE void Bsp_IntDis(void){ __disable_irq(); }
__STATIC_INLINE void Bsp_IntEn(void){ __enable_irq(); }


#define	Bsp_Printf		Bsp_UartPrintf

//#define	TRACE_DBG 	    //Bsp_UartPrintf

#ifdef __cplusplus
}                                              /* End of 'extern'al C lang linkage.                    */
#endif

#endif                                                          /* End of module include.                               */

