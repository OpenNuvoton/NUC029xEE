/***************************************************************************//**
 * @file     main.c
 * @brief
 *           Demonstrate how to upgrade firmware between USB device and PC through USB DFU (Device Firmware Upgrade) class.
 *           A Windows tool is also included in this sample code to connect with USB device.
 *
 * @copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "fmc_user.h"
#include "dfu_transfer.h"

#define TRIM_INIT           (GCR_BASE+0x118)

#define V6M_AIRCR_VECTKEY_DATA    0x05FA0000UL
#define V6M_AIRCR_SYSRESETREQ     0x00000004UL

#define DetectPin   PC0

uint32_t g_apromSize;

uint32_t GetApromSize()
{
    //the smallest of APROM size is 2K
    uint32_t size = 0x800, data;
    int result;

    do
    {
        result = FMC_Read_User(size, &data);

        if(result < 0)
        {
            return size;
        }
        else
        {
            size *= 2;
        }
    }
    while(1);
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable Internal RC 48MHz clock */
    CLK->PWRCON |= CLK_PWRCON_OSC48M_EN_Msk;

    /* Waiting for Internal RC clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_OSC48M_STB_Msk));

    /* Select HCLK clock source as HIRC and HCLK clock divider as 1 */
    CLK->CLKSEL0 = (CLK->CLKSEL0 & (~CLK_CLKSEL0_HCLK_S_Msk)) | CLK_CLKSEL0_HCLK_S_HIRC;
    CLK->CLKDIV = (CLK->CLKDIV & (~CLK_CLKDIV_HCLK_N_Msk)) | CLK_CLKDIV_HCLK(1);

    /* Use HIRC48 as USB clock source */
    CLK->CLKSEL0 = (CLK->CLKSEL0 & (~CLK_CLKSEL0_USB_S_Msk)) | CLK_CLKSEL0_USB_S_RC48M;
    CLK->CLKDIV = (CLK->CLKDIV & (~CLK_CLKDIV_USB_N_Msk)) | CLK_CLKDIV_USB(1);

    /* Enable module clock */
    CLK->APBCLK |= CLK_APBCLK_USBD_EN_Msk;
}

void USBD_IRQHandler(void);
/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    uint32_t u32TrimInit;

    /* Unlock write-protected registers */
    SYS_UnlockReg();

    /* Init system and multi-funcition I/O */
    SYS_Init();

    CLK->AHBCLK |= CLK_AHBCLK_ISP_EN_Msk;
    FMC->ISPCON |= FMC_ISPCON_ISPEN_Msk | FMC_ISPCON_APUEN_Msk | FMC_ISPCON_ISPFF_Msk;
    g_apromSize = GetApromSize();

    /* Open USB controller */
    USBD_Open(&gsInfo, DFU_ClassRequest, NULL);

    /*Init Endpoint configuration for DFU */
    DFU_Init();

    /* Start USB device */
    USBD_Start();

    /* Backup default trim */
    u32TrimInit = M32(TRIM_INIT);

    /* polling USBD interrupt flag */
    while(DetectPin == 0)
    {
        /* Start USB trim if it is not enabled. */
        if((SYS->HIRCTCTL & SYS_HIRCTCTL_FREQSEL_Msk) != 1)
        {
            /* Re-enable crystal-less */
            SYS->HIRCTCTL = 0x201 | (31 << SYS_HIRCTCTL_BOUNDARY_Pos);
        }

        /* Disable USB Trim when error */
        if(SYS->HIRCTSTS & (SYS_HIRCTSTS_CLKERIF_Msk | SYS_HIRCTSTS_TFAILIF_Msk))
        {
            /* Init TRIM */
            M32(TRIM_INIT) = u32TrimInit;

            if((u32TrimInit < 0x1E6) || (u32TrimInit > 0x253))
                /* Re-enable crystal-less */
                SYS->HIRCTCTL = 0x201 | (1 << SYS_HIRCTCTL_BOUNDARY_Pos);

            /* Clear error flags */
            SYS->HIRCTSTS = SYS_HIRCTSTS_CLKERIF_Msk | SYS_HIRCTSTS_TFAILIF_Msk;
        }

        USBD_IRQHandler();
    }

    SYS->RSTSRC = (SYS_RSTSRC_RSTS_POR_Msk | SYS_RSTSRC_RSTS_RESET_Msk);//clear bit
    FMC->ISPCON &=  ~(FMC_ISPCON_ISPEN_Msk | FMC_ISPCON_BS_Msk);
    SCB->AIRCR = (V6M_AIRCR_VECTKEY_DATA | V6M_AIRCR_SYSRESETREQ);

    /* Trap the CPU */
    while(1);
}

/*** (C) COPYRIGHT 2019 Nuvoton Technology Corp. ***/
