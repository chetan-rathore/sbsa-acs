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

int fdt_interrupt_cells(const void *fdt, int nodeoffset)
{
        const fdt32_t *ic;
        int val;
        int len;

        ic = fdt_getprop(fdt, nodeoffset, "#interrupt-cells", &len);
        if (!ic)
                return 2;

        if (len != sizeof(*ic))
                return -FDT_ERR_BADNCELLS;

        val = fdt32_to_cpu(*ic);
        if (val < 0)
                return -FDT_ERR_BADNCELLS;

        return val;
}

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
      sbsa_print(AVS_PRINT_ERR, L"DTB %x \n\n\n\n", DTB);
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
  sbsa_print(AVS_PRINT_ERR, L"fdt hdr check status %d \n", Status);

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
        ptr->mpidr = (((INT64)(reg_val[0] & PROPERTY_MASK_PE_REG1) << 32) | (reg_val[1] & PROPERTY_MASK_PE_REG0));
      } else {
        ptr->mpidr = (reg_val[0] & PROPERTY_MASK_PE_REG0);
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
  @brief  This API fills in the WD_INFO Table with information about the WDs in the
          system. This is achieved by parsing the DT blob.

  @param  WdTable  - Address where the WD information needs to be filled.

  @return  None
**/
VOID
pal_wd_create_info_table_dt(WD_INFO_TABLE *WdTable)
{
  WD_INFO_BLOCK *WdEntry = NULL;
  struct fdt_property *reg_prop = NULL , *intr_prop = NULL;
  int fdt_err;
  int reg_val[PROPERTY_WD_REG_SIZE_MAX];
  int intr_val[PROPERTY_WD_INTR_SIZE_MAX];
  UINT64 dt_ptr = 0;
  int offset;
  int wd_mode, wd_polarity;

  if (WdTable == NULL)
    return;

  WdEntry = WdTable->wd_info;
  WdTable->header.num_wd = 0;

  dt_ptr = pal_get_dt_ptr();
  if (dt_ptr == 0) {
    sbsa_print(AVS_PRINT_ERR, L"dt_ptr is NULL \n");
    return;
  }

  offset = fdt_node_offset_by_compatible((const void *)dt_ptr, -1, "arm,sbsa-gwdt");
  sbsa_print(AVS_PRINT_ERR, L" WD node offset %d \n", offset);

  while (offset != -FDT_ERR_NOTFOUND) {
      sbsa_print(AVS_PRINT_ERR, L" WD node%d offset %x \n", WdTable->header.num_wd, offset);

      reg_prop = fdt_get_property_w((void *)dt_ptr, offset, "reg", &fdt_err);
      if (NULL == reg_prop || fdt_err < 0) {
        sbsa_print(AVS_PRINT_ERR, L"reg property %x, Error %d \n", offset,fdt_err);
        return;
      }

      memcpy(reg_val, reg_prop->data, PROPERTY_WD_REG_SIZE_MAX*sizeof(int));

      intr_prop = fdt_get_property_w((void *)dt_ptr, offset, "interrupts ", &fdt_err);
      if (NULL == intr_prop || fdt_err < 0) {
        sbsa_print(AVS_PRINT_ERR, L"interrupt property %x, Error %d \n", offset,fdt_err);
        return;
      }

      memcpy(intr_val, intr_prop->data, PROPERTY_WD_INTR_SIZE_MAX*sizeof(int));

      offset = fdt_node_offset_by_compatible((const void *)dt_ptr, offset, "arm,sbsa-gwdt");
      WdTable->header.num_wd++;
      WdEntry->wd_refresh_base = ((INT64) reg_val[4]<<32) | reg_val[5];
      WdEntry->wd_ctrl_base    = ((INT64) reg_val[0]<<32) | reg_val[1];
      WdEntry->wd_gsiv         = intr_val[1];

      switch(intr_val[2])
      {
      case IRQ_TYPE_NONE:
        sbsa_print(AVS_PRINT_ERR, L"interrupt type none ");
        return;
        break;
      case IRQ_TYPE_EDGE_RISING:
        wd_mode = INTERRUPT_IS_EDGE_TRIGGERED;
        wd_polarity = INTERRUPT_IS_ACTIVE_HIGH;
        break;
      case IRQ_TYPE_EDGE_FALLING:
        wd_mode = INTERRUPT_IS_EDGE_TRIGGERED;
        wd_polarity = INTERRUPT_IS_ACTIVE_LOW;
        break;
      case IRQ_TYPE_LEVEL_HIGH:
        wd_mode = INTERRUPT_IS_LEVEL_TRIGGERED;
        wd_polarity = INTERRUPT_IS_ACTIVE_HIGH;
        break;
      case IRQ_TYPE_LEVEL_LOW:
        wd_mode = INTERRUPT_IS_LEVEL_TRIGGERED;
        wd_polarity = INTERRUPT_IS_ACTIVE_LOW;
        break;
      default:
        sbsa_print(AVS_PRINT_ERR, L"interrupt type invalid %d", intr_val[2]);
        return;
      }
      WdEntry->wd_flags = ((wd_polarity << 1) | (wd_mode << 0));
      WdEntry++;
  }
  dt_dump_wd_table(WdTable);

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
  GIC_INFO_ENTRY                *ptr = NULL;
  struct fdt_property *reg_prop = NULL;
  int fdt_err, interrupt_cell, rd;
  int its_val[4] = {0, 0, 0, 0};
  int gic_type[4] = {0x1001, 0x1002, 0x1003, 0x1004};
  int reg_val[PROPERTY_GIC_REG_SIZE_MAX] = {0};
  UINT64 dt_ptr = 0;
  int offset, i = 0, j = 0;

  if (GicTable == NULL)
    return;

  dt_ptr = pal_get_dt_ptr();
  if (dt_ptr == 0) {
    sbsa_print(AVS_PRINT_ERR, L"dt_ptr is NULL \n");
    return;
  }

  ptr = GicTable->gic_info;

  offset = fdt_node_offset_by_compatible((const void *)dt_ptr, -1, "arm,gic-v3");
  sbsa_print(AVS_PRINT_ERR, L" gic node offset %d \n", offset);

  while (offset != -FDT_ERR_NOTFOUND) {
      GicTable->header.gic_version = 3;
      interrupt_cell = fdt_interrupt_cells((const void*) dt_ptr, offset);
      sbsa_print(AVS_PRINT_ERR, L" gic node interrupt cell %d \n", interrupt_cell);
      if (interrupt_cell < 3){
        sbsa_print(AVS_PRINT_ERR, L"Invalid interrupt cell for offset %d \n", offset);
        return;
      }

      reg_prop = fdt_get_property_w((void *)dt_ptr, offset, "address-cells", &fdt_err);
      if (NULL == reg_prop || fdt_err < 0) {
        sbsa_print(AVS_PRINT_ERR, L"reg property %x, Error %d \n", offset,fdt_err);
        return;
      }

      reg_prop = fdt_get_property_w((void *)dt_ptr, offset, "size-cells", &fdt_err);
      if (NULL == reg_prop || fdt_err < 0) {
        sbsa_print(AVS_PRINT_ERR, L"reg property %x, Error %d \n", offset,fdt_err);
        return;
      }

      reg_prop = fdt_get_property_w((void *)dt_ptr, offset, "reg", &fdt_err);
      if (NULL == reg_prop || fdt_err < 0) {
        sbsa_print(AVS_PRINT_ERR, L"reg property %x, Error %d \n", offset,fdt_err);
        return;
      }

      reg_prop = fdt_get_property_w((void *)dt_ptr, offset, "redistributor-regions", &fdt_err);
      if (NULL == reg_prop || fdt_err < 0)
          rd = 1;
      else
          rd = 2;//reg_prop->data;

      memcpy(reg_val, reg_prop->data, PROPERTY_GIC_REG_SIZE_MAX*sizeof(int));
      while(i < (8+(rd*4))){
        ptr->type = gic_type[j];
        ptr->base = ((UINT64)((UINT64)reg_val[i]  << 32) | reg_val[i+1]);
        ptr->length = reg_val[i+3];
        if (rd != 0) {
            i = (i+4)*(rd+1);
            rd = 0;
        }
        else
            i++;
       ptr++;
       j++;
     }
      i = 4;
      while(i <= (rd*4)){
         ptr->type = gic_type[2];
         ptr->base = ((UINT64)((UINT64)reg_val[i]  << 32) | reg_val[i+1]);
         ptr->length = reg_val[i+3];
         i++;
         ptr++;
      }

      offset = fdt_node_offset_by_compatible((const void *)dt_ptr, offset, "arm,gic-v3-its");
      while (offset != -FDT_ERR_NOTFOUND) {
          reg_prop = fdt_get_property_w((void *)dt_ptr, offset, "reg", &fdt_err);
          if (NULL == reg_prop || fdt_err < 0) {
            sbsa_print(AVS_PRINT_ERR, L"reg property %x, Error %d \n", offset,fdt_err);
            return;
          }

          memcpy(its_val, reg_prop->data, 4*sizeof(int));
          offset = fdt_node_offset_by_compatible((const void *)dt_ptr, offset, "arm,gic-v3-its");
          ptr->type = gic_type[3];
          ptr->base = ((UINT64)((UINT64)its_val[0]  << 32) | its_val[1]);
          ptr->length = its_val[3];
          GicTable->header.num_its++;
          ptr++;
      }

  }
  dt_dump_gic_table(GicTable);
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
  PCIE_INFO_BLOCK                 *ptr = NULL;
  struct fdt_property *reg_prop = NULL;
  int fdt_err, addr_cell, size_cell;
  int reg_val[4] = {0, 0, 0, 0};
  int bus_range[2] = {0, 0};
  UINT64 dt_ptr = 0;
  int offset;

  if (PcieTable == NULL)
    return;

  dt_ptr = pal_get_dt_ptr();
  if (dt_ptr == 0) {
    sbsa_print(AVS_PRINT_ERR, L"dt_ptr is NULL \n");
    return;
  }

  PcieTable->num_entries = 0;
  ptr = PcieTable->block;

  offset = fdt_node_offset_by_prop_value((const void*) dt_ptr, -1, "device_type", "pci", 4);
  sbsa_print(AVS_PRINT_ERR, L" pci node offset %d \n", offset);

  while (offset != -FDT_ERR_NOTFOUND) {
      size_cell = fdt_size_cells((const void*) dt_ptr, offset);
      sbsa_print(AVS_PRINT_ERR, L" pcie node size cell %d \n", size_cell);
      if (size_cell != 2) {
        sbsa_print(AVS_PRINT_ERR, L"Invalid size cell \n");
        return;
      }

      addr_cell = fdt_address_cells((const void*) dt_ptr, offset);
      sbsa_print(AVS_PRINT_ERR, L" pci node addr cell %d \n", addr_cell);
      if (addr_cell != 3 ){
        sbsa_print(AVS_PRINT_ERR, L"Invalid address cell \n");
        return;
      }

      reg_prop = fdt_get_property_w((void *)dt_ptr, offset, "reg", &fdt_err);
      if (NULL == reg_prop || fdt_err < 0) {
        sbsa_print(AVS_PRINT_ERR, L"reg property %x, Error %d \n", offset,fdt_err);
        return;
      }
      memcpy(reg_val, reg_prop->data, addr_cell*sizeof(int));
      ptr->ecam_base = ((UINT64)((UINT64)reg_val[0]  << 32) | reg_val[1]);
      ptr->segment_num = 0;

      reg_prop = fdt_get_property_w((void *)dt_ptr, offset, "bus-range", &fdt_err);
      if (NULL == reg_prop || fdt_err < 0) {
        sbsa_print(AVS_PRINT_ERR, L"bus range property read err %d \n", fdt_err);
        return;
      }
      memcpy(bus_range, reg_prop->data, size_cell*sizeof(int));
      ptr->start_bus_num = bus_range[0];
      ptr->end_bus_num = bus_range[1];

      offset = fdt_node_offset_by_prop_value((const void*) dt_ptr, offset, "device_type", "pci", 4);
      ptr++;
      PcieTable->num_entries++;
  }

  dt_dump_pcie_table(PcieTable);
}

/**
  @brief  This API fills in the MEMORY_INFO_TABLE with information about memory in the
          system. This is achieved by parsing the UEFI memory map.

  @param  memoryInfoTable  - Address where the memory information needs to be filled.

  @return  None
**/
VOID
pal_memory_create_info_table_dt(MEMORY_INFO_TABLE *memoryInfoTable)
{
  MEM_INFO_BLOCK      *ptr = NULL;
  struct fdt_property *reg_prop = NULL;
  int fdt_err;
  int reg_val[8] = {0};
  UINT64 dt_ptr = 0;
  int offset;

  ptr = memoryInfoTable->info;
  offset = fdt_node_offset_by_prop_value((const void*) dt_ptr, -1, "device_type", "memory", 7);
  sbsa_print(AVS_PRINT_ERR, L" pci node offset %d \n", offset);

  while (offset != -FDT_ERR_NOTFOUND) {
      reg_prop = fdt_get_property_w((void *)dt_ptr, offset, "reg", &fdt_err);
      if (NULL == reg_prop || fdt_err < 0) {
        sbsa_print(AVS_PRINT_ERR, L"reg property %x, Error %d \n", offset,fdt_err);
        return;
      }
      memcpy(reg_val, reg_prop->data, 4*sizeof(int));
      ptr->type = MEMORY_TYPE_NORMAL;
      ptr->phy_addr = ((UINT64)((UINT64)reg_val[0]  << 32) | reg_val[1]);
      ptr->virt_addr = ((UINT64)((UINT64)reg_val[0]  << 32) | reg_val[1]); // virt_addr is same as phy_addr in uefi
      ptr->size = reg_val[3];

      offset = fdt_node_offset_by_prop_value((const void*) dt_ptr, offset, "device_type", "memory", 7);
      ptr++;
  }

  offset = fdt_node_offset_by_compatible((const void *)dt_ptr, offset, "shared-dma-pool");
  while (offset != -FDT_ERR_NOTFOUND) {
      reg_prop = fdt_get_property_w((void *)dt_ptr, offset, "reg", &fdt_err);
      if (NULL == reg_prop || fdt_err < 0) {
        sbsa_print(AVS_PRINT_ERR, L"reg property %x, Error %d \n", offset,fdt_err);
        return;
      }
      memcpy(reg_val, reg_prop->data, 4*sizeof(int));
      ptr->type = MEMORY_TYPE_RESERVED;
      ptr->phy_addr = ((UINT64)((UINT64)reg_val[0]  << 32) | reg_val[1]);
      ptr->virt_addr = ((UINT64)((UINT64)reg_val[0]  << 32) | reg_val[1]);
      ptr->size = reg_val[3];

      offset = fdt_node_offset_by_compatible((const void *)dt_ptr, offset, "shared-dma-pool");
      ptr++;
  }
  dt_dump_memory_table(memoryInfoTable);
}

/**
   @brief  This API fills in the TIMER_INFO Table with information about the timer in the
           system. This is achieved by parsing the DT blob.

   @param  TimerTable  - Address where the timer information needs to be filled.

   @return  None
**/

VOID
pal_timer_create_info_table_dt(TIMER_INFO_TABLE *TimerTable)
{
  //TIMER_INFO_GTBLOCK         *GtEntry = NULL;
  struct fdt_property *reg_prop = NULL;
  int fdt_err;
  int reg_val[8] = {0};
  UINT64 dt_ptr = 0;
  int offset;
  TimerTable->header.num_platform_timer = 0;
 //GtEntry = TimerTable->gt_info;
  offset = fdt_node_offset_by_compatible((const void *)dt_ptr, -1, "arm,armv8-timer");
  sbsa_print(AVS_PRINT_ERR, L" pci node offset %d \n", offset);

  while (offset != -FDT_ERR_NOTFOUND) {
      reg_prop = fdt_get_property_w((void *)dt_ptr, offset, "interrupts", &fdt_err);
      if (NULL == reg_prop || fdt_err < 0) {
        sbsa_print(AVS_PRINT_ERR, L"reg property %x, Error %d \n", offset,fdt_err);
        return;
      }

      memcpy(reg_val, reg_prop->data, PROPERTY_TIMER_REG_SIZE_MAX*sizeof(int));
      TimerTable->header.s_el1_timer_gsiv = reg_val[1];
      TimerTable->header.ns_el1_timer_gsiv = reg_val[4];
      TimerTable->header.virtual_timer_gsiv = reg_val[7];
      TimerTable->header.el2_timer_gsiv    = reg_val[10];

      TimerTable->header.num_platform_timer++;
      offset = fdt_node_offset_by_compatible((const void *)dt_ptr, offset, "arm,armv8-timer");

  }

  offset = fdt_node_offset_by_compatible((const void *)dt_ptr, -1, "arm,armv7-timer-mem");
  sbsa_print(AVS_PRINT_ERR, L" pci node offset %d \n", offset);

  while (offset != -FDT_ERR_NOTFOUND) {
      reg_prop = fdt_get_property_w((void *)dt_ptr, offset, "interrupts", &fdt_err);
      if (NULL == reg_prop || fdt_err < 0) {
        sbsa_print(AVS_PRINT_ERR, L"reg property %x, Error %d \n", offset,fdt_err);
        return;
      }
  }
     sbsa_print(AVS_PRINT_ERR, L"Dummy Timer DT print \n");
}
