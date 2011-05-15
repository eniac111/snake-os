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

#ifndef	_STAR_GPIO_H_
#define	_STAR_GPIO_H_


#include <asm/arch/star_sys_memory_map.h>


#if defined(__UBOOT__)
#define	GPIO_MEM_MAP_VALUE(reg_offset)		(*((u32 volatile *)(SYSPA_GPIO_BASE_ADDR + reg_offset)))
#elif defined(__LINUX__)
#define	GPIO_MEM_MAP_VALUE(reg_offset)		(*((u32 volatile *)(SYSVA_GPIO_BASE_ADDR + reg_offset)))
#else
#error "NO SYSTEM DEFINED"
#endif


#define	GPIO_GPIO_DATA_OUT_REG			GPIO_MEM_MAP_VALUE(0x00)
#define	GPIO_GPIO_DATA_IN_REG			GPIO_MEM_MAP_VALUE(0x04)
#define	GPIO_PIN_DIR_REG			GPIO_MEM_MAP_VALUE(0x08)
#define	GPIO_PIN_BYPASS_REG			GPIO_MEM_MAP_VALUE(0x0C)
#define	GPIO_GPIO_DATA_SET_REG			GPIO_MEM_MAP_VALUE(0x10)
#define	GPIO_GPIO_DATA_CLEAR_REG		GPIO_MEM_MAP_VALUE(0x14)

#define	GPIO_INTERRUPT_ENABLE_REG		GPIO_MEM_MAP_VALUE(0x20)
#define	GPIO_INTERRUPT_RAW_STATE_REG		GPIO_MEM_MAP_VALUE(0x24)
#define	GPIO_INTERRUPT_MASKED_STATE_REG		GPIO_MEM_MAP_VALUE(0x28)
#define	GPIO_INTERRUPT_MASK_REG			GPIO_MEM_MAP_VALUE(0x2C)
#define	GPIO_INTERRUPT_CLEAR_REG		GPIO_MEM_MAP_VALUE(0x30)
#define	GPIO_INTERRUPT_TRIGGER_REG		GPIO_MEM_MAP_VALUE(0x34)
#define	GPIO_INTERRUPT_BOTH_REG			GPIO_MEM_MAP_VALUE(0x38)
#define	GPIO_INTERRUPT_RISE_NEG_REG		GPIO_MEM_MAP_VALUE(0x3C)
#define	GPIO_BOUNCE_ENABLE_REG			GPIO_MEM_MAP_VALUE(0x40)
#define	GPIO_BOUNCE_PRESCALE_REG		GPIO_MEM_MAP_VALUE(0x44)

#define	GPIO_FEATURE_REG			GPIO_MEM_MAP_VALUE(0x78)
#define	GPIO_REVISION_REG			GPIO_MEM_MAP_VALUE(0x7C)


/*
 * define constant macros
 */
#define	GPIO_0_MASK		(1 << 0)
#define	GPIO_1_MASK		(1 << 1)
#define	GPIO_2_MASK		(1 << 2)
#define	GPIO_3_MASK		(1 << 3)
#define	GPIO_4_MASK		(1 << 4)
#define	GPIO_5_MASK		(1 << 5)
#define	GPIO_6_MASK		(1 << 6)
#define	GPIO_7_MASK		(1 << 7)
#define	GPIO_8_MASK		(1 << 8)
#define	GPIO_9_MASK		(1 << 9)
#define	GPIO_10_MASK		(1 << 10)
#define	GPIO_11_MASK		(1 << 11)
#define	GPIO_12_MASK		(1 << 12)
#define	GPIO_13_MASK		(1 << 13)
#define	GPIO_14_MASK		(1 << 14)
#define	GPIO_15_MASK		(1 << 15)
#define	GPIO_16_MASK		(1 << 16)
#define	GPIO_17_MASK		(1 << 17)
#define	GPIO_18_MASK		(1 << 18)
#define	GPIO_19_MASK		(1 << 19)
#define	GPIO_20_MASK		(1 << 20)


/*
 * macro declarations
 */
#define	HAL_GPIO_READ_DATA_OUT_STATE(data_out_state) \
    ((data_out_state) =	(GPIO_DATA_OUT_REG))


#define	HAL_GPIO_READ_DATA_IN_STATE(data_in_state) \
    ((data_in_state) = (GPIO_DATA_IN_REG))


#define	HAL_GPIO_SET_DIRECTION_OUTPUT(gpio_index) \
    ((GPIO_PIN_DIR_REG) |= (gpio_index))


#define	HAL_GPIO_SET_DIRECTION_INPUT(gpio_index) \
    ((GPIO_PIN_DIR_REG) &= ~(gpio_index))


#define	HAL_GPIO_ENABLE_BYPASS_MODE(gpio_index)	\
    ((GPIO_PIN_BYPASS_REG) |= (gpio_index))


#define	HAL_GPIO_DISABLE_BYPASS_MODE(gpio_index) \
    ((GPIO_PIN_BYPASS_REG) &= ~(gpio_index))


#define	HAL_GPIO_SET_DATA_OUT_HIGH(gpio_index) \
    ((GPIO_DATA_SET_REG) |= (gpio_index))


#define	HAL_GPIO_SET_DATA_OUT_LOW(gpio_index) \
    ((GPIO_DATA_CLEAR_REG) |= (gpio_index))


#define	HAL_GPIO_ENABLE_INTERRUPT(gpio_index) \
    ((GPIO_INTERRUPT_ENABLE_REG) |= (gpio_index))


#define	HAL_GPIO_DISABLE_INTERRUPT(gpio_index) \
    ((GPIO_INTERRUPT_ENABLE_REG) &= ~(gpio_index))


#define	HAL_GPIO_READ_INTERRUPT_RAW_STATE(raw_state) \
    ((raw_state) = (GPIO_INTERRUPT_RAW_STATE_REG))


#define	HAL_GPIO_READ_INTERRUPT_MASKED_STATE(masked_raw_state) \
    ((masked_raw_state)	= (GPIO_INTERRUPT_MASKED_STATE_REG))


#define	HAL_GPIO_DISABLE_INTERRUPT_MASK(gpio_index) \
    ((GPIO_INTERRUPT_MASK_REG) &= ~(gpio_index))


#define	HAL_GPIO_ENABLE_INTERRUPT_MASK(gpio_index) \
    ((GPIO_INTERRUPT_MASK_REG) |= (gpio_index))


#define	HAL_GPIO_CLEAR_INTERRUPT(gpio_index) \
    ((GPIO_INTERRUPT_CLEAR_REG) = (gpio_index))


#define	HAL_GPIO_SET_INTERRUPT_EDGE_TRIGGER_MODE(gpio_index) \
    ((GPIO_INTERRUPT_TRIGGER_REG) &= ~(gpio_index))


#define	HAL_GPIO_SET_INTERRUPT_LEVEL_TRIGGER_MODE(gpio_index) \
    ((GPIO_INTERRUPT_TRIGGER_REG) |= (gpio_index))


#define	HAL_GPIO_SET_INTERRUPT_SINGLE_EDGE_TRIGGER_MODE(gpio_index) \
{ \
    ((GPIO_INTERRUPT_TRIGGER_REG) &= ~(gpio_index)); \
    ((GPIO_INTERRUPT_BOTH_REG) &= ~(gpio_index)); \
}


#define	HAL_GPIO_SET_INTERRUPT_BOTH_EDGE_TRIGGER_MODE(gpio_index) \
{ \
    ((GPIO_INTERRUPT_TRIGGER_REG) &= ~(gpio_index)); \
    ((GPIO_INTERRUPT_BOTH_REG) |= (gpio_index));	\
}


#define	HAL_GPIO_SET_INTERRUPT_SINGLE_RISING_EDGE_TRIGGER_MODE(gpio_index) \
{ \
    ((GPIO_INTERRUPT_TRIGGER_REG) &= ~(gpio_index)); \
    ((GPIO_INTERRUPT_BOTH_REG) &= ~(gpio_index)); \
    ((GPIO_INTERRUPT_RISE_NEG_REG) &= ~(gpio_index)); \
}


#define	HAL_GPIO_SET_INTERRUPT_SINGLE_FALLING_EDGE_TRIGGER_MODE(gpio_index) \
{ \
    ((GPIO_INTERRUPT_TRIGGER_REG) &= ~(gpio_index)); \
    ((GPIO_INTERRUPT_BOTH_REG) &= ~(gpio_index)); \
    ((GPIO_INTERRUPT_RISE_NEG_REG) |= (gpio_index)); \
}


#define	HAL_GPIO_SET_INTERRUPT_HIGH_LEVEL_TRIGGER_MODE(gpio_index) \
{ \
    ((GPIO_INTERRUPT_TRIGGER_REG) |= (gpio_index)); \
    ((GPIO_INTERRUPT_RISE_NEG_REG) &= ~(gpio_index)); \
}


#define	HAL_GPIO_SET_INTERRUPT_LOW_LEVEL_TRIGGER_MODE(gpio_index) \
{ \
    ((GPIO_INTERRUPT_TRIGGER_REG) |= (gpio_index)); \
    ((GPIO_INTERRUPT_RISE_NEG_REG) |= (gpio_index)); \
}


#define	HAL_GPIO_ENABLE_BOUNCE(gpio_index) \
    ((GPIO_BOUNCE_ENABLE_REG) |= (gpio_index))


#define	HAL_GPIO_DISABLE_BOUNCE(gpio_index) \
    ((GPIO_BOUNCE_ENABLE_REG) &= ~(gpio_index))


#define	HAL_GPIO_READ_BOUNCE_PRESCALE_RATIO(prescale_ratio) \
    ((prescale_ratio) =	((GPIO_BOUNCE_PRESCALE_REG) & 0x0000FFFF))


#define	HAL_GPIO_WRITE_BOUNCE_PRESCALE_RATIO(prescale_ratio) \
    ((GPIO_BOUNCE_PRESCALE_REG) = (prescale_ratio & 0x0000FFFF))


#endif	// end of _STAR_GPIO_H_

