//==================================================================================================
//| �ļ����� | Bsp.c
//|----------|--------------------------------------------------------------------------------------
//| �ļ����� | Bsp.c �弶�������ܵ�ʵ��
//|----------|--------------------------------------------------------------------------------------
//| ��Ȩ���� | 
//|----------|--------------------------------------------------------------------------------------
//|  �汾    |  ʱ��       |  ����     | ����
//|----------|-------------|-----------|------------------------------------------------------------
//|  V1.0    | 2018.10.31  |  wjb      | ����
//==================================================================================================
#include  "bsp.h"

void Bsp_Init (void)
{
     
#ifdef  OS_SUPPORT 
    BSP_OSTickInit();                                           /* Initialize OS periodic time source                   */
#else
    //HAL_SetTickFreq(1000);                                    /* 1ms */
#endif    
    
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    
    Bsp_GpioInit();                                 //GPIO��ʼ��
    
    //Bsp_SramInit();
    
    /* COM1->485���� */
    COM1.ul_BaudRate   = 230400;
    COM1.ul_WordLength = UART_WORDLENGTH_8B;
    COM1.ul_StopBits   = UART_STOPBITS_1;
    COM1.ul_Parity     = UART_PARITY_NONE;
    COM1.ul_HwFlowCtl  = UART_HWCONTROL_NONE;
    COM1.ul_Mode       = UART_MODE_TX_RX;
    Bsp_UartOpen(&COM1); 
    
    Bsp_DacInit(&Bsp_Dac1);
    Bsp_DacSetVolt(&Bsp_Dac1,1.15f); 
    
    /* COM4->���Դ��� */
    COM4.ul_BaudRate   = 230400;
    COM4.ul_WordLength = UART_WORDLENGTH_8B;
    COM4.ul_StopBits   = UART_STOPBITS_1;
    COM4.ul_Parity     = UART_PARITY_NONE;
    COM4.ul_HwFlowCtl  = UART_HWCONTROL_NONE;
    COM4.ul_Mode       = UART_MODE_TX_RX;
    Bsp_UartOpen(&COM4);
    
    Bsp_TimeSampleInit();                           //ʱ�������У׼
    
    Bsp_ADC1Init();
    
    while(0)
    {
        Bsp_Sync1(TRUE);
        Bsp_Sync2(TRUE);  
        Bsp_DelayMs(1000);
        Bsp_Sync1(FALSE);
        Bsp_Sync2(FALSE); 
        Bsp_DelayMs(1000);
    }
}
