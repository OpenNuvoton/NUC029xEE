/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * $Revision: 2 $
 * $Date: 15/04/09 3:46p $
 * @brief    NUC029xEE Series EBI Driver Sample Code
 *
 * @note
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "ebi_nor.h"

#define PLLCON_SETTING      CLK_PLLCON_72MHz_HXT


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
    /* Enable IRC22M clock */
    CLK->PWRCON |= CLK_PWRCON_IRC22M_EN_Msk;

    /* Waiting for IRC22M clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_IRC22M_STB_Msk));

    /* Switch HCLK clock source to HIRC */
    CLK->CLKSEL0 = CLK_CLKSEL0_HCLK_S_HIRC;

    /* Set PLL to Power-down mode and PLL_STB bit in CLKSTATUS register will be cleared by hardware.*/
    CLK->PLLCON |= CLK_PLLCON_PD_Msk;

    /* Enable external 12 MHz XTAL */
    CLK->PWRCON |= CLK_PWRCON_XTL12M_EN_Msk;

    /* Waiting for clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_XTL12M_STB_Msk));

    /* Enable PLL and Set PLL frequency */
    CLK->PLLCON = PLLCON_SETTING;

    /* Waiting for clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_PLL_STB_Msk));

    /* System optimization when CPU runs at 72MHz */
    FMC->FATCON |= 0x50;

    /* Switch HCLK clock source to PLL, STCLK to HCLK/2 */
    CLK->CLKSEL0 = CLK_CLKSEL0_STCLK_S_HCLK_DIV2 | CLK_CLKSEL0_HCLK_S_PLL;

    /* Enable EBI clock */
    CLK->AHBCLK |= CLK_AHBCLK_EBI_EN_Msk;

    /* Enable UART clock */
    CLK->APBCLK = CLK_APBCLK_UART0_EN_Msk;

    /* UART clock source */
    CLK->CLKSEL1 = CLK_CLKSEL1_UART_S_PLL;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CyclesPerUs automatically. */
    SystemCoreClockUpdate();

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
    /* Reset UART */
    SYS->IPRSTC2 |=  SYS_IPRSTC2_UART0_RST_Msk;
    SYS->IPRSTC2 &= ~SYS_IPRSTC2_UART0_RST_Msk;

    /* Configure UART0 and set UART0 Baudrate */
    UART0->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(PllClock, 115200);
    UART0->LCR = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;
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

    /* Enable EBI function and bus width to 8-bit, MCLK is HCLK/4 */
    EBI->EBICON = (EBI_MCLKDIV_4 << EBI_EBICON_MCLKDIV_Pos) | EBI_EBICON_ExtEN_Msk |
                  (0x3 << EBI_EBICON_ExttALE_Pos) ;
    EBI->EXTIME = 0x03003318;

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
        goto lexit;
    }

    /* Erase flash */
    NOR_Erase();
    for(u32i = 0; u32i < EBI_MAX_SIZE; u32i++)
    {
        u8ReadOutData = NOR_ReadData(u32i);
        if(u8ReadOutData != 0xFF)
        {
            printf("    >> Chip Erase Fail !! Addr:0x%X, Data:0x%X.\n\n", u32i, u8ReadOutData);
            goto lexit;
        }
    }
    printf("    >> Chip Erase OK !!!\n");

    /* Start to program NOR flash test */
    if( ProgramContinueDataTest() == TRUE )
    {
        printf("*** NOR Flash Test OK ***\n");
    }

lexit:

    /* Disable EBI function */
    EBI->EBICON &= ~EBI_EBICON_ExtEN_Msk;

    /* Disable EBI clock */
    CLK->AHBCLK &= ~CLK_AHBCLK_EBI_EN_Msk;

    while(1);
}

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/
