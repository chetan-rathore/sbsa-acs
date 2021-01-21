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
    }
  }

  sbsa_print(AVS_PRINT_ERR, L"DTB %x \n", DTB);

  if (DTB) {
    // Can't get fdt address from firmware, check if user has passed it.
   sbsa_print(AVS_PRINT_ERR, L"g_dt_ptr %x \n", g_dt_ptr);

   if (g_dt_ptr)
        return 0; //No fdt addr found
   else
        DTB = (VOID *) g_dt_ptr;
  }

  Status = fdt_check_header(DTB);
  sbsa_print(AVS_PRINT_ERR, L"fdt hdr check status %x \n", Status);

  if (!Status) {
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
    pal_get_dt_ptr();
    if (PeTable == NULL)
        return;
}
