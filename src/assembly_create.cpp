//
// AMX Mod X, based on AMX Mod by Aleksander Naszko ("OLO").
// Copyright (C) The AMX Mod X Development Team.
//
// This software is licensed under the GNU General Public License, version 3 or higher.
// Additional exceptions apply. For full license details, see LICENSE.txt or visit:
//     https://alliedmods.net/amxmodx-license

//
// Okapi Module
//

#include "assembly_create.h"
#include "memory.h"

#define INST_DATA(T, ...) unsigned char T::data[] = {__VA_ARGS__}; size_t T::size = sizeof(data) / sizeof(*data);

INST_DATA(Inst_Mov_EDX_VAL, 0xBA, 0x0, 0x0, 0x0, 0x0)
INST_DATA(Inst_Mov_EAX_VAL, 0xB8, 0x0, 0x0, 0x0, 0x0)
INST_DATA(Inst_Mov_ECX_VAL, 0xB9, 0x0, 0x0, 0x0, 0x0)
INST_DATA(Inst_Mov_ptrEDX_EAX, 0x89, 0x2)
INST_DATA(Inst_Mov_ptrEDX_ECX, 0x89, 0xA)
INST_DATA(Inst_Mov_ptrEDXpVAL_EAX, 0x89, 0x42, 0x0)
INST_DATA(Inst_Mov_ptrEDXpVAL_ECX, 0x89, 0x4A, 0x0)
INST_DATA(Inst_Mov_EDX_ptrESPpVAL, 0x8B, 0x54, 0x24, 0x0)
INST_DATA(Inst_Mov_ptrESP_EDX, 0x89, 0x14, 0x24)
INST_DATA(Inst_Pop_EDX, 0x5A)
INST_DATA(Inst_Push_ECX, 0x51)
INST_DATA(Inst_Push_ESP, 0x54)
INST_DATA(Inst_Push_VAL, 0x68, 0x0, 0x0, 0x0, 0x0)
INST_DATA(Inst_Push_EDX, 0x52)
INST_DATA(Inst_Enter, 0xC8, 0x0, 0x0, 0x0)
INST_DATA(Inst_Call, 0xE8, 0x0, 0x0, 0x0, 0x0)
INST_DATA(Inst_Leave, 0xC9)
INST_DATA(Inst_Add_ESP_Val, 0x83, 0xC4, 0xC)
INST_DATA(Inst_Ret, 0xC3)
INST_DATA(Inst_RetN, 0xC2, 0x0, 0x0)
INST_DATA(Inst_Fst, 0xD9, 0x15, 0x0, 0x0, 0x0, 0x0)
INST_DATA(Inst_Fstp, 0xD9, 0x1D, 0x0, 0x0, 0x0, 0x0)
INST_DATA(Inst_Fld, 0xD9, 0x5, 0x0, 0x0, 0x0, 0x0)
