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

#ifndef	_STAR_WATCHDOG_TIMER_H_
#define	_STAR_WATCHDOG_TIMER_H_


#include <asm/arch/star_sys_memory_map.h>


#if defined(__UBOOT__)
#define	WDTIMER_MEM_MAP_VALUE(reg_offset)	(*((u32	volatile *)(SYSPA_WATCHDOG_TIMER_BASE_ADDR + reg_offset)))
#elif defined(__LINUX__)
#define	WDTIMER_MEM_MAP_VALUE(reg_offset)	(*((u32	volatile *)(SYSVA_WATCHDOG_TIMER_BASE_ADDR + reg_offset)))
#else
#error "NO SYSTEM DEFINED"
#endif


/*
 * define access macros
 */
#define	WDTIMER_COUNTER_REG			WDTIMER_MEM_MAP_VALUE(0x00)
#define	WDTIMER_AUTO_RELOAD_REG			WDTIMER_MEM_MAP_VALUE(0x04)
#define	WDTIMER_COUNTER_RESTART_REG		WDTIMER_MEM_MAP_VALUE(0x08)
#define	WDTIMER_CONTROL_REG			WDTIMER_MEM_MAP_VALUE(0x0C)
#define	WDTIMER_STATUS_REG			WDTIMER_MEM_MAP_VALUE(0x10)
#define	WDTIMER_CLEAR_REG			WDTIMER_MEM_MAP_VALUE(0x14)
#define	WDTIMER_INTERRUPT_LENGTH_REG		WDTIMER_MEM_MAP_VALUE(0x18)


/*
 * define constants macros
 */
#define	WDTIMER_ENABLE_BIT			(1 << 0)
#define	WDTIMER_SYSTEM_RESET_ENABLE_BIT		(1 << 1)
#define	WDTIMER_SYSTEM_INTERRUPT_ENABLE_BIT	(1 << 2)
#define	WDTIMER_EXTERNAL_SIGNAL_ENABLE_BIT	(1 << 3)
#define	WDTIMER_EXTERNAL_CLOCK_ENABLE_BIT	(1 << 4)


#define	WDTIMER_MAGIC_RESTART_VALUE		(0x5AB9)


/*
 * macros declarations
 */
#define	HAL_WDTIMER_READ_COUNTER(counter) \
{ \
    ((counter) = (WDTIMER_COUNTER_REG)); \
}


#define	HAL_WDTIMER_WRITE_AUTO_RELOAD_COUNTER(counter) \
{ \
    ((WDTIMER_AUTO_RELOAD_REG) = (counter)); \
}


#define	HAL_WDTIMER_READ_AUTO_RELOAD_COUNTER(counter) \
{ \
    ((counter) = (WDTIMER_AUTO_RELOAD_REG)); \
}


#define	HAL_WDTIMER_ENABLE_RESTART_RELOAD() \
{ \
    ((WDTIMER_COUNTER_RESTART_REG) = (WDTIMER_MAGIC_RESTART_VALUE)); \
}


#define	HAL_WDTIMER_ENABLE() \
{ \
    ((WDTIMER_CONTROL_REG) |= (WDTIMER_ENABLE_BIT)); \
}


#define	HAL_WDTIMER_DISABLE() \
{ \
    ((WDTIMER_CONTROL_REG) &= ~(WDTIMER_ENABLE_BIT)); \
}


#define	HAL_WDTIMER_ENABLE_SYSTEM_RESET() \
{ \
    ((WDTIMER_CONTROL_REG) |= (WDTIMER_SYSTEM_RESET_ENABLE_BIT)); \
}


#define	HAL_WDTIMER_DISABLE_SYSTEM_RESET() \
{ \
    ((WDTIMER_CONTROL_REG) &= ~(WDTIMER_SYSTEM_RESET_ENABLE_BIT)); \
}


#define	HAL_WDTIMER_ENABLE_SYSTEM_INTERRUPT() \
{ \
    ((WDTIMER_CONTROL_REG) |= (WDTIMER_SYSTEM_INTERRUPT_ENABLE_BIT)); \
}


#define	HAL_WDTIMER_DISABLE_SYSTEM_INTERRUPT() \
{ \
    ((WDTIMER_CONTROL_REG) &= ~(WDTIMER_SYSTEM_INTERRUPT_ENABLE_BIT)); \
}


#define	HAL_WDTIMER_ENABLE_EXTERNAL_SIGNAL() \
{ \
    ((WDTIMER_CONTROL_REG) |= (WDTIMER_EXTERNAL_SIGNAL_ENABLE_BIT)); \
}


#define	HAL_WDTIMER_DISABLE_EXTERNAL_SIGNAL() \
{ \
    ((WDTIMER_CONTROL_REG) &= ~(WDTIMER_EXTERNAL_SIGNAL_ENABLE_BIT)); \
}


#define	HAL_WDTIMER_CLOCK_SOURCE_PCLK()	\
{ \
    ((WDTIMER_CONTROL_REG) &= ~(WDTIMER_EXTERNAL_CLOCK_ENABLE_BIT)); \
}


#define	HAL_WDTIMER_CLOCK_SOURCE_EXTCLK() \
{ \
    ((WDTIMER_CONTROL_REG) |= (WDTIMER_EXTERNAL_CLOCK_ENABLE_BIT)); \
}


#define	HAL_WDTIMER_READ_STATUS(status)	\
{ \
    ((status) =	(WDTIMER_STATUS_REG) & 0x00000001); \
}


#define	HAL_WDTIMER_CLEAR_STATUS() \
{ \
    ((WDTIMER_CLEAR_REG) = (1)); \
}


#define	HAL_WDTIMER_WRITE_INTERRUPT_LENGTH(length) \
{ \
    ((WDTIMER_INTERRUPT_LENGTH_REG) = (length) & 0x000000FF); \
}


#endif	// end of #ifndef _STAR_WATCHDOG_TIMER_H_
