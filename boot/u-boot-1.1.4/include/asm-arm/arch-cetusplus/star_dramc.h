/*******************************************************************************
 *
 *  Copyright(c) 2006 Star Semiconductor Corporation, All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along with
 *  this program; if not, write to the Free Software Foundation, Inc., 59
 *  Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *  The full GNU General Public License is included in this distribution in the
 *  file called LICENSE.
 *
 *  Contact Information:
 *  Technology Support <tech@starsemi.com>
 *  Star Semiconductor 4F, No.1, Chin-Shan 8th St, Hsin-Chu,300 Taiwan, R.O.C
 *
 ******************************************************************************/

#ifndef _STAR_DRAMC_H_
#define _STAR_DRAMC_H_

#include "star_sys_memory_map.h"

#if defined(__UBOOT__)
#define DRAMC_MEM_MAP_VALUE(reg_offset)			(*((u32  volatile *)(SYSPA_DRAMC_BASE_ADDR + reg_offset)))
#elif defined(__LINUX__)
#define DRAMC_MEM_MAP_VALUE(reg_offset)			(*((u32 volatile *)(SYSVA_DRAMC_BASE_ADDR + reg_offset)))
#else
#error "NO SYSTEM DEFINED"
#endif

/*
 * define access macros
 */
#define DRAMC_MEMORY_INTERFACE_CONFIG_REG		DRAMC_MEM_MAP_VALUE(0x00)
#define DRAMC_PARAMETER_CONFIG_REG			DRAMC_MEM_MAP_VALUE(0x04)
#define DRAMC_POWER_UP_CONTROL_REG			DRAMC_MEM_MAP_VALUE(0x08)
#define DRAMC_TIMING_PARAMETER0_REG			DRAMC_MEM_MAP_VALUE(0x10)
#define DRAMC_TIMING_PARAMETER1_REG			DRAMC_MEM_MAP_VALUE(0x14)
#define DRAMC_TIMING_PARAMETER2_REG			DRAMC_MEM_MAP_VALUE(0x18)
#define DRAMC_PREREAD_TIMEOUT_DISABLE_REG		DRAMC_MEM_MAP_VALUE(0x1C)
#define DRAMC_PREREAD_ENABLE_REG			DRAMC_MEM_MAP_VALUE(0x20)
#define DRAMC_PREREAD_TIMEOUT0_REG			DRAMC_MEM_MAP_VALUE(0x24)
#define DRAMC_PREREAD_TIMEOUT1_REG			DRAMC_MEM_MAP_VALUE(0x28)
#define DRAMC_DDQ_OUTPUT_DELAY_CONTROL_REG		DRAMC_MEM_MAP_VALUE(0x30)
#define DRAMC_DQS_INPUT_DELAY_CONTROL_REG		DRAMC_MEM_MAP_VALUE(0x34)
#define DRAMC_HCLK_DELAY_CONTROL_REG			DRAMC_MEM_MAP_VALUE(0x38)
#define DRAMC_PAD_POWER_DOWN_REG			DRAMC_MEM_MAP_VALUE(0x3C)
#define DRAMC_IDLE_COUNTER_REG				DRAMC_MEM_MAP_VALUE(0x40)
#define DRAMC_PERFORMANCE_CONTROL_REG			DRAMC_MEM_MAP_VALUE(0x44)
#define DRAMC_WRITE_CACHELINE_LOW_COUNTER_REG		DRAMC_MEM_MAP_VALUE(0x48)
#define DRAMC_WRITE_CACHELINE_HIGH_COUNTER_REG		DRAMC_MEM_MAP_VALUE(0x4C)
#define DRAMC_READ_CACHELINE_HIGH_COUNTER_REG		DRAMC_MEM_MAP_VALUE(0x4C)
#define DRAMC_READ_CACHELINE_LOW_COUNTER_REG		DRAMC_MEM_MAP_VALUE(0x50)
#define DRAMC_EMBEDDED_SRAM_CONFIG_REG			DRAMC_MEM_MAP_VALUE(0x54)

/*
 * define constants macros
 */
#define DRAMC_PREREAD_DISABLE				(0)
#define DRAMC_PREREAD_ENABLE				(1)

#define DRAMC_PREREAD_TIMEOUT_DISABLE			(0)
#define DRAMC_PREREAD_TIMEOUT_ENABLE			(1)

#define DRAMC_CHANNEL_0					(0)
#define DRAMC_CHANNEL_1					(1)
#define DRAMC_CHANNEL_2					(2)
#define DRAMC_CHANNEL_3					(3)
#define DRAMC_CHANNEL_4					(4)
#define DRAMC_CHANNEL_5					(5)
#define DRAMC_CHANNEL_6					(6)
#define DRAMC_CHANNEL_7					(7)

/*
 * macro declarations
 */
#define HAL_DRAMC_RESET_PERFORMANCE_COUNTER() \
{ \
    ((DRAMC_PERFORMANCE_CONTROL_REG) = (1 << 16)); \
}

#define HAL_DRAMC_STOP_PERFORMANCE_COUNTER() \
{ \
    ((DRAMC_PERFORMANCE_CONTROL_REG) = 0); \
}

#define HAL_DRAMC_RUN_PERFORMANCE_COUNTER() \
{ \
    ((DRAMC_PERFORMANCE_CONTROL_REG) = (1 << 17)); \
}

#endif  // end of #ifndef _STAR_DRAMC_H_

