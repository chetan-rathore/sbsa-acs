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

#ifndef __PAL_DT_SPEC_H__
#define __PAL_DT_SPEC_H__

/*  PE related */
#define PROPERTY_MASK_PE_REG1  0xFF
#define PROPERTY_MASK_PE_REG0  0xFFFFFF

/*  WD related */
#define PROPERTY_WD_REG_SIZE_MAX 8
#define PROPERTY_WD_INTR_SIZE_MAX 3

/* Interrupt Flags for WD*/
#define INTERRUPT_IS_LEVEL_TRIGGERED 0x0
#define INTERRUPT_IS_EDGE_TRIGGERED  0x1
#define INTERRUPT_IS_ACTIVE_HIGH     0x0
#define INTERRUPT_IS_ACTIVE_LOW      0x1


/* Interrupt Flags from DT*/
#define IRQ_TYPE_NONE             0x00000000         /* Default, unspecified type */
#define IRQ_TYPE_EDGE_RISING      0x00000001         /* Edge rising type */
#define IRQ_TYPE_EDGE_FALLING     0x00000002         /* Edge falling type */
#define IRQ_TYPE_LEVEL_HIGH       0x00000004         /* Level high type */
#define IRQ_TYPE_LEVEL_LOW        0x00000008         /* Level low type */

/* GIC related */
#define PROPERTY_GIC_REG_SIZE_MAX 20

/* Timer related */
#define PROPERTY_TIMER_REG_SIZE_MAX 12

#endif
