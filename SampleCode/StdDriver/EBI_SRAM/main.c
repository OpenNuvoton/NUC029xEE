/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * $Revision: 3 $
 * $Date: 15/04/10 4:09p $
 * @brief    NUC029xEE Series EBI Driver Sample Code
 *
 * @note
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

#define PLL_CLOCK           72000000


extern void SRAM_BS616LV4017(void);
void AccessEBIWithPDMA(void);


void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable Internal RC 22.1184MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

    /* Enable external XTAL 12MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);
    
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);    
    
    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));
    
    /* Enable EBI module clock */
    CLK_EnableModuleClock(EBI_MODULE);    

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD, TXD */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= (SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD);

    /* Set multi-function pins for EBI AD0 ~ AD7 */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB14_Msk | SYS_GPB_MFP_PB13_Msk | SYS_GPB_MFP_PB15_Msk);
    SYS->ALT_MFP &= ~(SYS_ALT_MFP_PB14_Msk | SYS_ALT_MFP_PB13_Msk | SYS_ALT_MFP_PB15_Msk);
    SYS->GPC_MFP &= ~(SYS_GPC_MFP_PC14_Msk | SYS_GPC_MFP_PC15_Msk);
    SYS->ALT_MFP &= ~(SYS_ALT_MFP_PC14_Msk | SYS_ALT_MFP_PC15_Msk);
    SYS->GPC_MFP &= ~(SYS_GPC_MFP_PC6_Msk | SYS_GPC_MFP_PC7_Msk);
    SYS->ALT_MFP &= ~(SYS_ALT_MFP_PC6_Msk | SYS_ALT_MFP_PC7_Msk);
    SYS->GPA_MFP &= ~(SYS_GPA_MFP_PA6_Msk);
    SYS->ALT_MFP &= ~(SYS_ALT_MFP_PA6_Msk);
    SYS->ALT_MFP2 &= ~(SYS_ALT_MFP2_PB14_Msk | SYS_ALT_MFP2_PB15_Msk);
    SYS->GPB_MFP |= (SYS_GPB_MFP_PB14_AD0 | SYS_GPB_MFP_PB13_AD1 | SYS_GPB_MFP_PB15_AD6);
    SYS->ALT_MFP |= (SYS_ALT_MFP_PB14_AD0 | SYS_ALT_MFP_PB13_AD1 | SYS_ALT_MFP_PB15_AD6);
    SYS->GPC_MFP |= (SYS_GPC_MFP_PC14_AD2 | SYS_GPC_MFP_PC15_AD3);
    SYS->ALT_MFP |= (SYS_ALT_MFP_PC14_AD2 | SYS_ALT_MFP_PC15_AD3);
    SYS->GPC_MFP |= (SYS_GPC_MFP_PC6_AD4 | SYS_GPC_MFP_PC7_AD5);
    SYS->ALT_MFP |= (SYS_ALT_MFP_PC6_AD4 | SYS_ALT_MFP_PC7_AD5);
    SYS->GPA_MFP |= (SYS_GPA_MFP_PA6_AD7);
    SYS->ALT_MFP |= (SYS_ALT_MFP_PA6_AD7);
    SYS->ALT_MFP2 |= (SYS_ALT_MFP2_PB14_AD0 | SYS_ALT_MFP2_PB15_AD6); 
   
    /* Set multi-function pins for EBI AD8 ~ AD15 */
    SYS->GPA_MFP &= ~(SYS_GPA_MFP_PA5_Msk | SYS_GPA_MFP_PA4_Msk |
                      SYS_GPA_MFP_PA3_Msk | SYS_GPA_MFP_PA2_Msk |
                      SYS_GPA_MFP_PA1_Msk | SYS_GPA_MFP_PA12_Msk |
                      SYS_GPA_MFP_PA13_Msk | SYS_GPA_MFP_PA14_Msk);
    SYS->GPA_MFP |= (SYS_GPA_MFP_PA5_AD8 | SYS_GPA_MFP_PA4_AD9 |
                     SYS_GPA_MFP_PA3_AD10 | SYS_GPA_MFP_PA2_AD11 |
                     SYS_GPA_MFP_PA1_AD12 | SYS_GPA_MFP_PA12_AD13 |
                     SYS_GPA_MFP_PA13_AD14 | SYS_GPA_MFP_PA14_AD15);
    SYS->ALT_MFP &= ~(SYS_ALT_MFP_PA5_Msk | SYS_ALT_MFP_PA4_Msk |
                      SYS_ALT_MFP_PA3_Msk | SYS_ALT_MFP_PA2_Msk |
                      SYS_ALT_MFP_PA1_Msk | SYS_ALT_MFP_PA12_Msk |
                      SYS_ALT_MFP_PA13_Msk | SYS_ALT_MFP_PA14_Msk);
    SYS->ALT_MFP |= (SYS_ALT_MFP_PA5_AD8 | SYS_ALT_MFP_PA4_AD9 |
                     SYS_ALT_MFP_PA3_AD10 | SYS_ALT_MFP_PA2_AD11 |
                     SYS_ALT_MFP_PA1_AD12 | SYS_ALT_MFP_PA12_AD13 |
                     SYS_ALT_MFP_PA13_AD14 | SYS_ALT_MFP_PA14_AD15);
                         
    /* Set multi-function pins for EBI nCS, ALE and MCLK */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB7_Msk | SYS_GPB_MFP_PB6_Msk);
    SYS->ALT_MFP &= ~(SYS_ALT_MFP_PB7_Msk | SYS_ALT_MFP_PB6_Msk);
    SYS->GPB_MFP |= (SYS_GPB_MFP_PB7_nCS | SYS_GPB_MFP_PB6_ALE);
    SYS->ALT_MFP |= (SYS_ALT_MFP_PB7_nCS | SYS_ALT_MFP_PB6_ALE);
    SYS->GPC_MFP &= ~SYS_GPC_MFP_PC8_Msk;
    SYS->ALT_MFP &= ~SYS_ALT_MFP_PC8_Msk;
    SYS->GPC_MFP |= SYS_GPC_MFP_PC8_MCLK;
    SYS->ALT_MFP |= SYS_ALT_MFP_PC8_MCLK;

    /* Set multi-function pins for EBI nWR, nRD, nWRL and nWRH */
    SYS->GPA_MFP &= ~(SYS_GPA_MFP_PA10_Msk | SYS_GPA_MFP_PA11_Msk);
    SYS->ALT_MFP &= ~(SYS_ALT_MFP_PA10_Msk | SYS_ALT_MFP_PA11_Msk);
    SYS->GPA_MFP |= (SYS_GPA_MFP_PA10_nWR | SYS_GPA_MFP_PA11_nRD);
    SYS->ALT_MFP |= (SYS_ALT_MFP_PA10_nWR | SYS_ALT_MFP_PA11_nRD);
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB2_Msk | SYS_GPB_MFP_PB3_Msk);
    SYS->ALT_MFP &= ~(SYS_ALT_MFP_PB2_Msk | SYS_ALT_MFP_PB3_Msk);
    SYS->GPB_MFP |= SYS_GPB_MFP_PB2_nWRL | SYS_GPB_MFP_PB3_nWRH;
    SYS->ALT_MFP |= SYS_ALT_MFP_PB2_nWRL | SYS_ALT_MFP_PB3_nWRH;
}

void UART0_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART0 */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();

    printf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    printf("+----------------------------+\n");
    printf("|    EBI SRAM Sample Code    |\n");
    printf("+----------------------------+\n\n");

    printf("********************************************************************************\n");
    printf("* Please connect BS616LV4017 to NUC029xEE Series EBI bus before EBI testing !! *\n");
    printf("********************************************************************************\n\n");

    /* Enable EBI function and bus width to 16-bit */
    EBI_Open(0, EBI_BUSWIDTH_16BIT, EBI_TIMING_VERYSLOW, 0, 0);

    /* Start SRAM test */
    SRAM_BS616LV4017();

    /* EBI sram with PDMA test */
    AccessEBIWithPDMA();

    /* Disable EBI function */
    EBI_Close(0);

    /* Disable EBI clock */
    CLK_DisableModuleClock(EBI_MODULE);

    printf("*** SRAM Test OK ***\n");

    while(1);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables for PDMA                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t PDMA_TEST_LENGTH = 64;
uint32_t SrcArray[64];
uint32_t DestArray[64];
uint32_t volatile u32IsTestOver = 0xFF;

/**
 * @brief       DMA IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The DMA default IRQ, declared in startup_nuc230_240.s.
 */
void PDMA_IRQHandler(void)
{
    uint32_t status = PDMA_GET_INT_STATUS();

    if(status & 0x1) {  /* CH0 */
        if(PDMA_GET_CH_INT_STS(0) & 0x2)
            u32IsTestOver = 0;
        PDMA_CLR_CH_INT_FLAG(0, PDMA_ISR_BLKD_IF_Msk);
    } else if(status & 0x2) {  /* CH1 */
        if(PDMA_GET_CH_INT_STS(1) & 0x2)
            u32IsTestOver = 1;
        PDMA_CLR_CH_INT_FLAG(1, PDMA_ISR_BLKD_IF_Msk);
    } else if(status & 0x4) {  /* CH2 */
        if(PDMA_GET_CH_INT_STS(2) & 0x2)
            u32IsTestOver = 2;
        PDMA_CLR_CH_INT_FLAG(2, PDMA_ISR_BLKD_IF_Msk);
    } else if(status & 0x8) {  /* CH3 */
        if(PDMA_GET_CH_INT_STS(3) & 0x2)
            u32IsTestOver = 3;
        PDMA_CLR_CH_INT_FLAG(3, PDMA_ISR_BLKD_IF_Msk);
    } else if(status & 0x10) {  /* CH4 */
        if(PDMA_GET_CH_INT_STS(4) & 0x2)
            u32IsTestOver = 4;
        PDMA_CLR_CH_INT_FLAG(4, PDMA_ISR_BLKD_IF_Msk);
    } else if(status & 0x20) {  /* CH5 */
        if(PDMA_GET_CH_INT_STS(5) & 0x2)
            u32IsTestOver = 5;
        PDMA_CLR_CH_INT_FLAG(5, PDMA_ISR_BLKD_IF_Msk);
    } else if(status & 0x40) {  /* CH6 */
        if(PDMA_GET_CH_INT_STS(6) & 0x2)
            u32IsTestOver = 6;
        PDMA_CLR_CH_INT_FLAG(6, PDMA_ISR_BLKD_IF_Msk);
    } else if(status & 0x80) {  /* CH7 */
        if(PDMA_GET_CH_INT_STS(7) & 0x2)
            u32IsTestOver = 7;
        PDMA_CLR_CH_INT_FLAG(7, PDMA_ISR_BLKD_IF_Msk);
    } else if(status & 0x100) {  /* CH8 */
        if(PDMA_GET_CH_INT_STS(8) & 0x2)
            u32IsTestOver = 8;
        PDMA_CLR_CH_INT_FLAG(8, PDMA_ISR_BLKD_IF_Msk);
    } else
        printf("unknown interrupt !!\n");
}

void AccessEBIWithPDMA(void)
{
    uint32_t i;
    uint32_t u32Result0 = 0x5A5A, u32Result1 = 0x5A5A;
    
    printf("[[ Access EBI with PDMA ]]\n");
    
    /* Enable PDMA clock source */
    CLK_EnableModuleClock(PDMA_MODULE);
    
    /* Reset PDMA module */
    SYS_ResetModule(PDMA_RST);

    for(i=0; i<64; i++) {
        SrcArray[i] = 0x76570000 + i;
        u32Result0 += SrcArray[i];
    }

    /* Open Channel 2 */    
    PDMA_Open(1 << 2);
    PDMA_SetTransferCnt(2, PDMA_WIDTH_32, PDMA_TEST_LENGTH);
    PDMA_SetTransferAddr(2, (uint32_t)SrcArray, PDMA_SAR_INC, EBI_BASE_ADDR, PDMA_DAR_INC);
    PDMA_EnableInt(2, PDMA_IER_BLKD_IE_Msk);
    NVIC_EnableIRQ(PDMA_IRQn);
    u32IsTestOver = 0xFF;
    PDMA_Trigger(2);
    while(u32IsTestOver == 0xFF);
    /* Transfer internal SRAM to EBI SRAM done */
       
    /* Clear internal SRAM data */
    for(i=0; i<64; i++) {
        SrcArray[i] = 0x0;
    }

    /* transfer width is one word(32 bit) */
    PDMA_SetTransferCnt(2, PDMA_WIDTH_32, PDMA_TEST_LENGTH);
    PDMA_SetTransferAddr(2, EBI_BASE_ADDR, PDMA_SAR_INC, (uint32_t)SrcArray, PDMA_DAR_INC);
    PDMA_EnableInt(2, PDMA_IER_BLKD_IE_Msk);
    NVIC_EnableIRQ(PDMA_IRQn);
    u32IsTestOver = 0xFF;
    PDMA_Trigger(2);
    while(u32IsTestOver == 0xFF);
    /* Transfer EBI SRAM to internal SRAM done */
    for(i=0; i<64; i++) {
        u32Result1 += SrcArray[i];
    }
    
    if(u32IsTestOver == 2) {
        if((u32Result0 == u32Result1) && (u32Result0 != 0x5A5A)) {
            printf("        PASS (0x%X)\n\n", u32Result0);
        }else {
            printf("        FAIL - data matched (0x%X)\n\n", u32Result0);
            while(1);
        }
    }else {
            printf("        PDMA fail\n\n");
            while(1);
    }
        
    PDMA_Close();
}

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/
