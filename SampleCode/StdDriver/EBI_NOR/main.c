/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * $Revision: 2 $
 * $Date: 15/04/10 3:54p $
 * @brief    NUC029xEE Series EBI Driver Sample Code
 *
 * @note
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "ebi_nor.h"

#define PLL_CLOCK           72000000


/*---------------------------------------------------------------------------------------------------------*/
/* Program Continue Data to NOR Flash                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t ProgramContinueDataTest(void)
{
    uint8_t u8DataIn, u8DataOut;
    uint32_t u32NORAddr;

    printf("    >> Start to program flash ... \n");

    /* Write */
    for(u32NORAddr = 0; u32NORAddr < EBI_MAX_SIZE; u32NORAddr++)
    {
        u8DataIn = (u32NORAddr % 256) + u32NORAddr;
        if(NOR_WriteData(u32NORAddr, u8DataIn) == FALSE)
        {
            printf("Program [0x%05X]:[0x%02X] FAIL !!! \n\n", u32NORAddr, u8DataIn);
            return FALSE;
        }
    }

    /* Read */
    for(u32NORAddr = 0; u32NORAddr < EBI_MAX_SIZE; u32NORAddr++)
    {
        u8DataIn = (u32NORAddr % 256) + u32NORAddr;
        u8DataOut = NOR_ReadData(u32NORAddr);
        if(u8DataOut != u8DataIn)
        {
            printf("Read [0x%05X]:[0x%02X] FAIL !!! (Got [0x%02X]) \n\n", u32NORAddr, u8DataIn, u8DataOut);
            printf("Program flash FAIL !!! \n\n");
            return FALSE;
        }
    }
    printf("    >> Continue Data Program OK !!! \n\n");

    return TRUE;
}

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
    uint32_t u32i;
    uint32_t u32NORIDInfo;
    uint8_t u8ReadOutData;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();

    printf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    printf("+---------------------------------+\n");
    printf("|    EBI NOR Flash Sample Code    |\n");
    printf("+---------------------------------+\n\n");

    printf("*****************************************************************************\n");
    printf("* Please connect W39L040P to NUC029xEE Series EBI bus before EBI testing !! *\n");
    printf("*****************************************************************************\n\n");

    /* Enable EBI function and bus width to 8-bit */
    EBI_Open(0, EBI_BUSWIDTH_8BIT, EBI_TIMING_VERYSLOW, 0, 0);

    /* Initial NOR flash and check ID */
    NOR_Init();
    u32NORIDInfo = NOR_GetID();
    if(u32NORIDInfo == 0xDAB6)
    {
        printf("NOR W39L040P initial OK ! Manufacture ID:0x%X, Device ID:0x%X.\n", (u32NORIDInfo >> 8), (u32NORIDInfo & 0xFF));
    }
    else
    {
        printf("NOR W39L040P initial fail ! (ID:0x%X)\n\n", u32NORIDInfo);
        while(1);
    }

    /* Erase flash */
    NOR_Erase();
    for(u32i = 0; u32i < EBI_MAX_SIZE; u32i++)
    {
        u8ReadOutData = NOR_ReadData(u32i);
        if(u8ReadOutData != 0xFF)
        {
            printf("    >> Chip Erase Fail !! Addr:0x%X, Data:0x%X.\n\n", u32i, u8ReadOutData);
            while(1);
        }
    }
    printf("    >> Chip Erase OK !!!\n");

    /* Start to program NOR flash test */
    ProgramContinueDataTest();

    /* Disable EBI function */
    EBI_Close(0);

    /* Disable EBI clock */
    CLK->AHBCLK &= ~CLK_AHBCLK_EBI_EN_Msk;

    printf("*** NOR Flash Test OK ***\n");

    while(1);
}

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/
