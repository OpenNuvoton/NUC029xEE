/***************************************************************************//**
 * @file     main.c
 * @brief    ISP tool main function
 *
 * @note
 * Copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "targetdev.h"

#define TRIM_INIT           (GCR_BASE+0x118)

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

    /* Init system and multi-function I/O */
    SYS_Init();
    CLK->AHBCLK |= CLK_AHBCLK_ISP_EN_Msk;
    FMC->ISPCON |= FMC_ISPCON_ISPEN_Msk;

    /* Get APROM size, data flash size and address */
    g_apromSize = GetApromSize();
    GetDataFlashInfo(&g_dataFlashAddr, &g_dataFlashSize);

    while(DetectPin == 0)
    {
        /* Open USB controller */
        USBD_Open(&gsInfo, HID_ClassRequest, NULL);
        /*Init Endpoint configuration for HID */
        HID_Init();
        /* Start USB device */
        USBD_Start();

        /* Backup default trim */
        u32TrimInit = M32(TRIM_INIT);

        /* DO NOT Enable USB device interrupt */
        // NVIC_EnableIRQ(USBD_IRQn);

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

            // polling USBD interrupt flag
            USBD_IRQHandler();

            if(bUsbDataReady == TRUE)
            {
                ParseCmd((uint8_t *)usb_rcvbuf, 64);
                EP2_Handler();
                bUsbDataReady = FALSE;
            }
        }
    }

    SYS->RSTSRC = (SYS_RSTSRC_RSTS_POR_Msk | SYS_RSTSRC_RSTS_RESET_Msk);//clear bit
    FMC->ISPCON &=  ~(FMC_ISPCON_ISPEN_Msk | FMC_ISPCON_BS_Msk);
    SCB->AIRCR = (V6M_AIRCR_VECTKEY_DATA | V6M_AIRCR_SYSRESETREQ);

    /* Trap the CPU */
    while(1);
}

/*** (C) COPYRIGHT 2019 Nuvoton Technology Corp. ***/
