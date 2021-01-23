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
pal_wd_create_info_table_dt(WD_INFO_TABLE *WdTable);


/*-----------------DEBUG FUNCTION----------------*/

VOID
dt_dump_pe_table(PE_INFO_TABLE *PeTable);

VOID
dt_dump_wd_table(WD_INFO_TABLE *WdTable);

#endif
