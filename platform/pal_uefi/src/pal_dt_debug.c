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
    sbsa_print(AVS_PRINT_ERR, L"    MPIDR    :%x\n", PeTable->pe_info[Index].mpidr);
    sbsa_print(AVS_PRINT_ERR, L"    PMU GSIV :%x\n", PeTable->pe_info[Index].pmu_gsiv);
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
    sbsa_print(AVS_PRINT_ERR, L"    CONTROL BASE  :%x\n", WdTable->wd_info[Index].wd_ctrl_base);
    sbsa_print(AVS_PRINT_ERR, L"    GSIV          :%x\n", WdTable->wd_info[Index].wd_gsiv);
    sbsa_print(AVS_PRINT_ERR, L"    FLAGS         :%x\n", WdTable->wd_info[Index].wd_flags);
    Index++;
  }
}

/**
  @brief  This API is use to dump PCIE_INFO Table after filling from DT

  @param  WdTable  - Address where the PCIE information needs to be filled.

  @return  None
**/
VOID
dt_dump_pcie_table(PCIE_INFO_TABLE *PcieTable)
{
  UINT32 Index = 0;

  sbsa_print(AVS_PRINT_ERR, L"************PCIE TABLE************ \n");
  sbsa_print(AVS_PRINT_ERR, L" NUM ECAM %d \n", PcieTable->num_entries);

  while (Index < PcieTable->num_entries) {
    sbsa_print(AVS_PRINT_ERR, L" ECAM BASE     :%x\n", PcieTable->block[Index].ecam_base);
    sbsa_print(AVS_PRINT_ERR, L"      START BUS     :%x\n", PcieTable->block[Index].start_bus_num);
    sbsa_print(AVS_PRINT_ERR, L"      END BUS       :%x\n", PcieTable->block[Index].end_bus_num);
    sbsa_print(AVS_PRINT_ERR, L"      SEGMENT NUM   :%x\n", PcieTable->block[Index].segment_num);
    Index++;
  }
}

/**
  @brief  This API is use to dump GIC_INFO Table after filling from DT

  @param  WdTable  - Address where the PCIE information needs to be filled.

  @return  None
**/
VOID
dt_dump_gic_table(GIC_INFO_TABLE *GicTable)
{
  UINT32 Index = 0;

  sbsa_print(AVS_PRINT_ERR, L"************GIC TABLE************ \n");
  sbsa_print(AVS_PRINT_ERR, L" GIC version %d \n", GicTable->header.gic_version);

  while (Index < 5) {
    sbsa_print(AVS_PRINT_ERR, L" GIC TYPE     :%x\n", GicTable->gic_info[Index].type);
    sbsa_print(AVS_PRINT_ERR, L"     BASE     :%x\n", GicTable->gic_info[Index].base);
    sbsa_print(AVS_PRINT_ERR, L"     LEN      :%x\n", GicTable->gic_info[Index].length);
    Index++;
  }
}

/**
  @brief  This API is use to dump GIC_INFO Table after filling from DT

  @param  WdTable  - Address where the PCIE information needs to be filled.

  @return  None
**/
VOID
dt_dump_memory_table(MEMORY_INFO_TABLE *memoryInfoTable)
{
  UINT32 Index = 0;

  sbsa_print(AVS_PRINT_ERR, L"************MEMORY TABLE************ \n");

  while (Index < 5) {
    sbsa_print(AVS_PRINT_ERR, L" MEMORY Type      :%x\n", memoryInfoTable->info[Index].type);
    sbsa_print(AVS_PRINT_ERR, L"        PHY addr  :%x\n", memoryInfoTable->info[Index].phy_addr);
    sbsa_print(AVS_PRINT_ERR, L"        VIRT addr :%x\n", memoryInfoTable->info[Index].virt_addr);
    sbsa_print(AVS_PRINT_ERR, L"        size      :%x\n", memoryInfoTable->info[Index].size);
    Index++;
  }
}
