/** @file
 * Copyright (c) 2021, Arm Limited or its affiliates. All rights reserved.
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

#ifndef __PAL_DT_H__
#define __PAL_DT_H__

VOID
pal_pe_create_info_table_dt(PE_INFO_TABLE *PeTable);

VOID
pal_gic_create_info_table_dt(GIC_INFO_TABLE *GicTable);

VOID
pal_pcie_create_info_table_dt(PCIE_INFO_TABLE *PcieTable);

VOID
pal_timer_create_info_table_dt(TIMER_INFO_TABLE *TimerTable);

VOID
pal_wd_create_info_table_dt(WD_INFO_TABLE *WdTable);

VOID
pal_memory_create_info_table_dt(MEMORY_INFO_TABLE *memoryInfoTable);

/*-----------------DEBUG FUNCTION----------------*/

VOID
dt_dump_pe_table(PE_INFO_TABLE *PeTable);

VOID
dt_dump_gic_table(GIC_INFO_TABLE *GicTable);

VOID
dt_dump_pcie_table(PCIE_INFO_TABLE *PcieTable);

VOID
dt_dump_timer_table(TIMER_INFO_TABLE *TimerTable);

VOID
dt_dump_wd_table(WD_INFO_TABLE *WdTable);

VOID
dt_dump_memory_table(MEMORY_INFO_TABLE *memoryInfoTable);

int fdt_interrupt_cells(const void *fdt, int nodeoffset);

#endif
