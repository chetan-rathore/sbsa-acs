/** @file
 * Copyright (c) 2016-2018, 2020 Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include "include/pal_uefi.h"
#include "include/pal_dt.h"

/**
  @brief  This API is use to dump PE_INFO Table after filling from DT

  @param  PeTable  - Address where the PE information needs to be filled.

  @return  None
**/
VOID
dt_dump_pe_table(PE_INFO_TABLE *PeTable)
{
  UINT32 Index = 0;

  sbsa_print(AVS_PRINT_ERR, L"************PE TABLE************ \n");
  sbsa_print(AVS_PRINT_ERR, L" NUM PE %d \n", PeTable->header.num_of_pe);

  while (Index < PeTable->header.num_of_pe) {
    sbsa_print(AVS_PRINT_ERR, L" PE NUM      :%x\n", PeTable->pe_info[Index].pe_num);
    sbsa_print(AVS_PRINT_ERR, L" PE MPIDR    :%x\n", PeTable->pe_info[Index].mpidr);
    sbsa_print(AVS_PRINT_ERR, L" PE PMU GSIV :%x\n", PeTable->pe_info[Index].pmu_gsiv);
    Index++;
  }
}


/**
  @brief  This API is use to dump WD_INFO Table after filling from DT

  @param  WdTable  - Address where the WD information needs to be filled.

  @return  None
**/
VOID
dt_dump_wd_table(WD_INFO_TABLE *WdTable)
{
  UINT32 Index = 0;

  sbsa_print(AVS_PRINT_ERR, L"************WD TABLE************ \n");
  sbsa_print(AVS_PRINT_ERR, L" NUM WD %d \n", WdTable->header.num_wd);

  while (Index < WdTable->header.num_wd) {
    sbsa_print(AVS_PRINT_ERR, L" WD REFRESH BASE  :%x\n", WdTable->wd_info[Index].wd_refresh_base);
    sbsa_print(AVS_PRINT_ERR, L" WD CONTROL BASE  :%x\n", WdTable->wd_info[Index].wd_ctrl_base);
    sbsa_print(AVS_PRINT_ERR, L" WD GSIV          :%x\n", WdTable->wd_info[Index].wd_gsiv);
    sbsa_print(AVS_PRINT_ERR, L" WD FLAGS         :%x\n", WdTable->wd_info[Index].wd_flags);
    Index++;
  }
}
