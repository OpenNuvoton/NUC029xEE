/******************************************************************************
 * @file     Smpl_DrvFMC.c
 * @version  V2.00
 * $Revision: 4 $
 * $Date: 15/05/21 3:54p $
 * @brief    NUC029xEE Series Flash Memory Controller Driver Sample Code
 *
 * @note
 * Copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

#define PLLCON_SETTING      CLK_PLLCON_72MHz_HXT
#define PLL_CLOCK           72000000

#if !defined(__ICCARM__) && !defined(__GNUC__)
extern uint32_t Image$$RO$$Base;
#endif

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable Internal RC 22.1184MHz clock */
    CLK->PWRCON |= CLK_PWRCON_OSC22M_EN_Msk;

    /* Waiting for Internal RC clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_OSC22M_STB_Msk));

    /* Switch HCLK clock source to Internal RC and and HCLK source divide 1 */
    CLK->CLKSEL0 &= ~CLK_CLKSEL0_HCLK_S_Msk;
    CLK->CLKSEL0 |= CLK_CLKSEL0_HCLK_S_HIRC;
    CLK->CLKDIV &= ~CLK_CLKDIV_HCLK_N_Msk;
    CLK->CLKDIV |= CLK_CLKDIV_HCLK(1);

    /* Enable external XTAL 12MHz clock */
    CLK->PWRCON |= CLK_PWRCON_XTL12M_EN_Msk;

    /* Waiting for external XTAL clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_XTL12M_STB_Msk));

    /* Set core clock as PLL_CLOCK from PLL */
    CLK->PLLCON = PLLCON_SETTING;
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_PLL_STB_Msk));
    CLK->CLKSEL0 &= (~CLK_CLKSEL0_HCLK_S_Msk);
    CLK->CLKSEL0 |= CLK_CLKSEL0_HCLK_S_PLL;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    //SystemCoreClockUpdate();
    PllClock        = PLL_CLOCK;            // PLL
    SystemCoreClock = PLL_CLOCK / 1;        // HCLK
    CyclesPerUs     = PLL_CLOCK / 1000000;  // For SYS_SysTickDelay()

    /* Enable UART module clock */
    CLK->APBCLK |= CLK_APBCLK_UART0_EN_Msk;

    /* Select UART module clock source */
    CLK->CLKSEL1 &= ~CLK_CLKSEL1_UART_S_Msk;
    CLK->CLKSEL1 |= CLK_CLKSEL1_UART_S_HXT;

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set GPB multi-function pins for UART0 RXD and TXD  */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD;
}

void UART0_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART0 */
    SYS->IPRSTC2 |=  SYS_IPRSTC2_UART0_RST_Msk;
    SYS->IPRSTC2 &= ~SYS_IPRSTC2_UART0_RST_Msk;

    /* Configure UART0 and set UART0 Baudrate */
    UART0->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HXT, 115200);
    UART0->LCR = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
	volatile uint32_t u32BootAddr;

    uint8_t ch;
    uint32_t u32Data;
    uint32_t u32Cfg;

    /* Unlock protected registers for ISP function */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init UART0 for printf */
    UART0_Init();

    /* Enable ISP function */
    FMC->ISPCON |= FMC_ISPCON_ISPEN_Msk;

    /*
        This sample code shows how to boot with different firmware images in APROM.
        In the code, VECMAP is used to implement multi-boot function. Software set VECMAP
        to remap page of VECMAP to 0x0~0x1ff.
        NOTE: VECMAP only valid when CBS = 00'b or 10'b.

        To use this sample code, please:
        1. Build all targets and download to device individually. The targets are:
            FMC_MultiBoot, RO=0x0
            FMC_Boot0, RO=0x1000
            FMC_Boot1, RO=0x2000
            FMC_Boot2, RO=0x3000
            FMC_Boot3, RO=0x4000
        2. Reset MCU to execute FMC_MultiBoot.

    */

    printf("\n\n");
    printf("+-----------------------------------------------------+\n");
    printf("|    NUC029xEE Multi-Boot Sample Code                 |\n");
    printf("+-----------------------------------------------------+\n");

    printf("\nCPU @ %dHz\n\n", SystemCoreClock);

#if defined(__BASE__)
    printf("Boot from 0\n");
#endif
#if defined(__BOOT0__)
    printf("Boot from 0x1000\n");
#endif
#if defined(__BOOT1__)
    printf("Boot from 0x2000\n");
#endif
#if defined(__BOOT2__)
    printf("Boot from 0x3000\n");
#endif
#if defined(__BOOT3__)
    printf("Boot from 0x4000\n");
#endif

#if defined(__ICCARM__) || defined(__GNUC__)
    printf("VECMAP = 0x%x\n", FMC_GetVECMAP());
#else
    printf("Current RO Base = 0x%x, VECMAP = 0x%x\n", (uint32_t)&Image$$RO$$Base, FMC_GetVECMAP());
#endif

    /* Check IAP mode */
    u32Cfg = FMC_Read(FMC_CONFIG_BASE);
    if((u32Cfg & 0xc0) != 0x80) {
        printf("Do you want to set to new IAP mode (APROM boot + LDROM)?\n");
        if(getchar() == 'y') {
            FMC->ISPCON |= FMC_ISPCON_CFGUEN_Msk; /* Enable user configuration update */

            /* Set CBS to b'10 */
            u32Cfg &= ~0xc0ul;
            u32Cfg |= 0x80;
            u32Data = FMC_Read(FMC_CONFIG_BASE + 0x4); /* Backup the data of config1 */
            FMC_Erase(FMC_CONFIG_BASE);
            FMC_Write(FMC_CONFIG_BASE, u32Cfg);
            FMC_Write(FMC_CONFIG_BASE + 0x4, u32Data);

            printf("Press any key to reset system to enable new IAP mode ...\n");
            getchar();
            SYS->IPRSTC1 = 0x1; /* Reset MCU */
            while(1);
        } else {
            printf("VECMAP only valid in new IAP mode. CBS = 10'b or 00'b\n");
            goto lexit;
        }
    }

    printf("Select one boot image: \n");
    printf("[0] Boot 0, base = 0x1000\n");
    printf("[1] Boot 1, base = 0x2000\n");
    printf("[2] Boot 2, base = 0x3000\n");
    printf("[3] Boot 3, base = 0x4000\n");
    printf("[Others] Boot, base = 0x0\n");

    ch = getchar();
    switch(ch)
    {
        case '0':
            u32BootAddr = 0x1000;
            break;
        case '1':
            u32BootAddr = 0x2000;
            break;
        case '2':
            u32BootAddr = 0x3000;
            break;
        case '3':
            u32BootAddr = 0x4000;
            break;
        default:
            u32BootAddr = 0x0000;
            break;
    }

    /* Reset CPU only to reset to new vector page */
    SYS->IPRSTC1 |= SYS_IPRSTC1_CPU_RST_Msk;

    /* Reset System to reset to new vector page. */
    //NVIC_SystemReset();

    while(1);

lexit:

    /* Disable ISP function */
    FMC->ISPCON &= ~FMC_ISPCON_ISPEN_Msk;

    /* Lock protected registers */
    SYS_LockReg();

    printf("\nDone\n");
    while(SYS->PDID) __WFI();
}




