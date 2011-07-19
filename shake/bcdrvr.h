/****************************************************************
 * File - bcdrvr.h 
 *
 * Driver header file for accessing the bc635/7PCI-V2/PCIe module
 * This file may not be distributed -- it may only be used for 
 * development or evaluation purposes.
 * For details refer to SW_License.txt.
 *
 * Copyright (c) Symmetricom, 2000-2009
 *
 ***************************************************************/
#ifndef _BCDRVR_H_
#define _BCDRVR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "windrvr.h"

typedef struct BC_PCI_STRUCT *BC_PCI_HANDLE;
typedef void (*BC_PCI_INT_HANDLER)(BC_PCI_HANDLE hBC_PCI, DWORD dwSource);

///////////////////////////////////////////////////////////////////
// Device Driver Structures
//////////////////////////////////////////////////////////////////
typedef struct
{
    WD_INTERRUPT Int;
    HANDLE hThread;
    WD_TRANSFER Trans[3];
    BC_PCI_INT_HANDLER funcIntHandler;
} BC_PCI_INT_INTERRUPT;

typedef struct
{
    DWORD index;
    BOOL  fIsMemory;
    BOOL  fActive;
} BC_PCI_ADDR_DESC;

typedef struct BC_PCI_STRUCT
{
    HANDLE hWD;
    BC_PCI_INT_INTERRUPT Int;
    WD_PCI_SLOT pciSlot;
    BC_PCI_ADDR_DESC addrDesc[7]; // BC_PCI_ITEMS
    WD_CARD_REGISTER cardReg;
} BC_PCI_STRUCT;

// The Revision ID for V2 hardware
#define BC_PCI_V2_REV_ID_START  0x20
#define BC_PCI_V2_REV_ID_END    0x2F


#ifdef __cplusplus
}
#endif

#endif //_BCDRVR_H_
