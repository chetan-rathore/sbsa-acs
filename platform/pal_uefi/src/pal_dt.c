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
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ShellLib.h>
#include <Library/DtPlatformDtbLoaderLib.h>

#include <Include/libfdt.h>
#include "Include/Guid/Acpi.h"
#include <Protocol/AcpiTable.h>
#include "Include/IndustryStandard/Acpi61.h"

#include "include/pal_uefi.h"
#include "include/pal_dt.h"
#include "include/pal_dt_spec.h"

extern UINT64 g_dt_ptr;

/**
  @brief   Use UEFI System Table to look up FdtTableGuid and returns the FDT Blob Address

  @param  None

  @return Returns 64-bit FDT blob address
*/
UINT64
pal_get_dt_ptr()
{
  VOID                       *DTB = NULL;
  EFI_STATUS                 Status;
  UINT32                     Index;

  for (Index = 0; Index < gST->NumberOfTableEntries; Index++) {
    if (CompareGuid (&gFdtTableGuid, &(gST->ConfigurationTable[Index].VendorGuid))) {
      DTB = gST->ConfigurationTable[Index].VendorTable;
      sbsa_print(AVS_PRINT_ERR, L"DTB %x \n", DTB);
    }
  }

  if (!DTB) {
    // Can't get fdt address from firmware, check if user has passed it.
   sbsa_print(AVS_PRINT_ERR, L"g_dt_ptr %x \n", g_dt_ptr);

   if (!g_dt_ptr)
        return 0; //No fdt addr found
   else
        DTB = (VOID *) g_dt_ptr;
  }

  Status = fdt_check_header(DTB);
  sbsa_print(AVS_PRINT_ERR, L"fdt hdr check status %s \n", (Status ? "FAILED" : "PASSED"));

  if (Status) {
    return 0;
  }
  return (UINT64) DTB;
}

/**
  @brief  This API fills in the PE_INFO Table with information about the PEs in the
          system. This is achieved by parsing the DT blob.

  @param  PeTable  - Address where the PE information needs to be filled.

  @return  None
**/
VOID
pal_pe_create_info_table_dt(PE_INFO_TABLE *PeTable)
{
  PE_INFO_ENTRY                 *ptr = NULL;
  struct fdt_property *reg_prop = NULL;
  int fdt_err, addr_cell, size_cell;
  int reg_val[2] = {0, 0};
  UINT64 dt_ptr = 0;
  int offset, parent_offset;

  if (PeTable == NULL)
    return;

  dt_ptr = pal_get_dt_ptr();
  if (dt_ptr == 0) {
    sbsa_print(AVS_PRINT_ERR, L"dt_ptr is NULL \n");
    return;
  }

  PeTable->header.num_of_pe = 0;
  ptr = PeTable->pe_info;

  offset = fdt_node_offset_by_prop_value((const void*) dt_ptr, -1, "device_type", "cpu", 4);
  sbsa_print(AVS_PRINT_ERR, L" cpu node offset %d \n", offset);

  if (offset != -FDT_ERR_NOTFOUND) {
      parent_offset = fdt_parent_offset((const void*) dt_ptr, offset);

      size_cell = fdt_size_cells((const void*) dt_ptr, parent_offset);
      sbsa_print(AVS_PRINT_ERR, L" cpu node size cell %d \n", size_cell);
      if (size_cell != 0) {
        sbsa_print(AVS_PRINT_ERR, L"Invalid size cell for offset %d \n", parent_offset);
        return;
      }

      addr_cell = fdt_address_cells((const void*) dt_ptr, offset);
      sbsa_print(AVS_PRINT_ERR, L" cpu node addr cell %d \n", addr_cell);
      if (addr_cell <= 0 || addr_cell > 2 ){
        sbsa_print(AVS_PRINT_ERR, L"Invalid address cell for offset %x \n", parent_offset);
        return;
      }
  }

  while (offset != -FDT_ERR_NOTFOUND) {
      sbsa_print(AVS_PRINT_ERR, L" cpu node%d offset %x \n", PeTable->header.num_of_pe, offset);

      reg_prop = fdt_get_property_w((void *)dt_ptr, offset, "reg", &fdt_err);
      if (NULL == reg_prop || fdt_err < 0) {
        sbsa_print(AVS_PRINT_ERR, L"reg property %x, Error %d \n", offset,fdt_err);
        return;
      }

      memcpy(reg_val, reg_prop->data, addr_cell*sizeof(int));
      sbsa_print(AVS_PRINT_ERR, L"reg_val<0> =  %x \n", reg_val[0]);
      if (addr_cell == 2) {
        sbsa_print(AVS_PRINT_ERR, L"reg_val<1> =  %x \n", reg_val[1]);
        ptr->mpidr = (((INT64)(reg_val[0] & PROPERTY_MASK_REG1) << 32) | (reg_val[1] & PROPERTY_MASK_REG0));
      } else {
        ptr->mpidr = (reg_val[0] & PROPERTY_MASK_REG0);
      }

      offset = fdt_node_offset_by_prop_value((const void*) dt_ptr, offset, "device_type", "cpu", 4);
      ptr->pe_num   = PeTable->header.num_of_pe;
      ptr->pmu_gsiv = 0;  //TBD
      ptr++;
      PeTable->header.num_of_pe++;
  }
  dt_dump_pe_table(PeTable);
}

/**
  @brief  This API fills in the GIC_INFO Table with information about the GIC in the
          system. This is achieved by parsing the DT blob.

  @param  PeTable  - Address where the GIC information needs to be filled.

  @return  None
**/
VOID
pal_gic_create_info_table_dt(GIC_INFO_TABLE *GicTable)
{
    sbsa_print(AVS_PRINT_ERR, L"Dummy GIC DT print \n");
}

/**
  @brief  This API fills in the PCIE_INFO Table with information about the pcie in the
          system. This is achieved by parsing the DT blob.

  @param  PeTable  - Address where the PCIe information needs to be filled.

  @return  None
**/
VOID
pal_pcie_create_info_table_dt(PCIE_INFO_TABLE *PcieTable)
{
    sbsa_print(AVS_PRINT_ERR, L"Dummy PCIE DT print \n");
}

/**
  @brief  This API fills in the TIMER_INFO Table with information about the timer in the
          system. This is achieved by parsing the DT blob.

  @param  PeTable  - Address where the TIMER information needs to be filled.

  @return  None
**/
VOID
pal_timer_create_info_table_dt(TIMER_INFO_TABLE *TimerTable)
{
    sbsa_print(AVS_PRINT_ERR, L"Dummy Timer DT print \n");
}

/**
  @brief  This API fills in the WD_INFO Table with information about the watchdog in the
          system. This is achieved by parsing the DT blob.

  @param  PeTable  - Address where the WD information needs to be filled.

  @return  None
**/
VOID
pal_wd_create_info_table_dt(WD_INFO_TABLE *WdTable)
{
    sbsa_print(AVS_PRINT_ERR, L"Dummy WD DT print \n");
}
