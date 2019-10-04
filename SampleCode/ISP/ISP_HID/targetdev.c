/***************************************************************************//**
 * @file     targetdev.c
 * @brief    ISP support function source file
 * @version  0x32
 *
 * @note
 * Copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include "targetdev.h"
#include "isp_user.h"

uint32_t GetApromSize()
{
    return 0x20000; // 128K
}

// Data Flash is shared with APROM.
// The size and start address are defined in CONFIG1.
void GetDataFlashInfo(uint32_t *addr, uint32_t *size)
{
    uint32_t uData;
    *size = 0;
    FMC_Read_User(Config0, &uData);

    if((uData & 0x01) == 0)    //DFEN enable
    {
        FMC_Read_User(Config1, &uData);

        // Filter the reserved bits in CONFIG1
        uData &= 0x000FFFFF;

        if(uData > g_apromSize || uData & (FMC_FLASH_PAGE_SIZE - 1))    //avoid config1 value from error
        {
            uData = g_apromSize;
        }

        *addr = uData;
        *size = g_apromSize - uData;
    }
    else
    {
        *addr = g_apromSize;
        *size = 0;
    }
}

/*** (C) COPYRIGHT 2019 Nuvoton Technology Corp. ***/
