/***************************************************************************//**
 * @file     targetdev.h
 * @brief    ISP support function header file
 * @version  0x31
 *
 * @note
 * Copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include "NuMicro.h"
#include "uart_transfer.h"
#include "isp_user.h"

extern __align(4) uint8_t response_buff[64];
extern __align(4) uint8_t usb_rcvbuf[64];
extern uint8_t bUsbDataReady;


/* rename for uart_transfer.c */
#define UART_N					UART1
#define UART_N_IRQHandler		UART1_IRQHandler
#define UART_N_IRQn				UART1_IRQn

/*** (C) COPYRIGHT 2019 Nuvoton Technology Corp. ***/