/**************************************************************************//**
 * @file     ebi_nor.h
 * @version  V2.00
 * $Revision: 1 $
 * $Date: 14/10/14 2:54p $
 * @brief    NOR Flash - W39L040P Driver Header File
 *
 * @note
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __EBI_NOR_H__
#define __EBI_NOR_H__

#ifdef __cplusplus
extern "C"
{
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Define EBI_NOR Functions Prototype                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
void NOR_Init(void);
void NOR_Reset(void);
uint32_t NOR_GetID(void);
uint8_t NOR_Erase(void);
uint8_t NOR_CheckCMDComplete(uint32_t u32DestAddr, uint8_t u8Data);
uint8_t NOR_ReadData(uint32_t u32DestAddr);
uint8_t NOR_WriteData(uint32_t u32DestAddr, uint8_t u8Data);

#ifdef __cplusplus
}
#endif

#endif //__EBI_NOR_H__

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/
