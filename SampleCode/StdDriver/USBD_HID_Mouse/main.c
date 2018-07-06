/******************************************************************************
 * @file     main.c
 * @brief
 *           Show how to implement a USB mouse device. 
 *           The mouse cursor will move automatically when this mouse device connecting to PC by USB.
 * @note
 * Copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "hid_mouse.h"

#define CRYSTAL_LESS        1
#define TRIM_INIT           (GCR_BASE+0x118)

void EnableCLKO(uint32_t u32ClkSrc, uint32_t u32ClkDiv)
{
    /* CLKO = clock source / 2^(u32ClkDiv + 1) */
    CLK->FRQDIV = CLK_FRQDIV_DIVIDER_EN_Msk | u32ClkDiv;

    /* Enable CLKO clock source */
    CLK->APBCLK |= CLK_APBCLK_FDIV_EN_Msk;

    /* Select CLKO clock source */
    CLK->CLKSEL2 = (CLK->CLKSEL2 & (~CLK_CLKSEL2_FRQDIV_S_Msk)) | u32ClkSrc;
}

/*--------------------------------------------------------------------------*/
void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable Internal RC 22.1184 MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

#if (!CRYSTAL_LESS)
    /* Enable external XTAL 12 MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for external XTAL clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock */
    CLK_SetCoreClock(72000000);
    
    /* Select module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HIRC, CLK_CLKDIV_UART(1));
    CLK_SetModuleClock(USBD_MODULE, CLK_CLKSEL0_USB_S_PLL, CLK_CLKDIV_USB(3));
#else
    /* Enable Internal RC 48MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC48M_EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC48M_STB_Msk);

    /* Select module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HIRC, CLK_CLKDIV_UART(1));
    CLK_SetModuleClock(USBD_MODULE, CLK_CLKSEL0_USB_S_RC48M, CLK_CLKDIV_USB(1));
#endif

    /* Enable module clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(USBD_MODULE);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GPB multi-function pins for UART0 RXD and TXD, and Clock Output */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk | SYS_GPB_MFP_PB8_Msk);
    SYS->ALT_MFP &= ~SYS_ALT_MFP_PB8_Msk;
    SYS->GPB_MFP |= (SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD | SYS_GPB_MFP_PB8_CLKO);
    SYS->ALT_MFP |=  SYS_ALT_MFP_PB8_CLKO;

    /* Enable CLKO (PB.8) for monitor HCLK. CLKO = HCLK/8 Hz*/
    EnableCLKO((2 << CLK_CLKSEL2_FRQDIV_S_Pos), 2);
}


void UART0_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset IP */
    SYS->IPRSTC2 |=  SYS_IPRSTC2_UART0_RST_Msk;
    SYS->IPRSTC2 &= ~SYS_IPRSTC2_UART0_RST_Msk;

    /* Configure UART0 and set UART0 Baudrate */
    UART0->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HIRC, 115200);
    UART0->LCR = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    uint32_t u32TrimInit;

    /* Unlock protected registers */
    SYS_UnlockReg();

    SYS_Init();

    UART0_Init();

    printf("\n");
    printf("+-----------------------------------------------------+\n");
    printf("|          NuMicro USB HID Mouse Sample Code          |\n");
    printf("+-----------------------------------------------------+\n");

    USBD_Open(&gsInfo, HID_ClassRequest, NULL);

    /* Endpoint configuration */
    HID_Init();
    USBD_Start();

#if CRYSTAL_LESS
    /* Backup init trim */
    u32TrimInit = M32(TRIM_INIT);

    /* Enable USB crystal-less */
    SYS->HIRCTCTL = 0x201 | (31 << SYS_HIRCTCTL_BOUNDARY_Pos);
#endif

    NVIC_EnableIRQ(USBD_IRQn);

    while(1)
    {
#if CRYSTAL_LESS
        /* Re-start crystal-less when any error found */
        if (SYS->HIRCTSTS & (SYS_HIRCTSTS_CLKERIF_Msk | SYS_HIRCTSTS_TFAILIF_Msk))
        {
            SYS->HIRCTSTS = SYS_HIRCTSTS_CLKERIF_Msk | SYS_HIRCTSTS_TFAILIF_Msk;

            if((u32TrimInit < 0x1E6) || (u32TrimInit > 0x253))
                /* Re-enable crystal-less */
                SYS->HIRCTCTL = 0x201 | (1 << SYS_HIRCTCTL_BOUNDARY_Pos);
            else
                /* Re-enable crystal-less */
                SYS->HIRCTCTL = 0x201 | (31 << SYS_HIRCTCTL_BOUNDARY_Pos);
            //printf("USB trim fail. Just retry. SYS->HIRCTSTS = 0x%x, SYS->HIRCTCTL = 0x%x\n", SYS->HIRCTSTS, SYS->HIRCTCTL);
        }
#endif

        HID_UpdateMouseData();
    }
}



/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/

