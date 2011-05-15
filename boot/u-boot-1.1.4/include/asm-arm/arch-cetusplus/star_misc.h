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

#ifndef	_STAR_MISC_H_
#define	_STAR_MISC_H_

#include "star_sys_memory_map.h"

#if defined(__UBOOT__)
#define	MISC_MEM_MAP_VALUE(reg_offset)			(*((u32 volatile *)(SYSPA_MISC_BASE_ADDR + reg_offset)))
#elif defined(__LINUX__)
#define	MISC_MEM_MAP_VALUE(reg_offset)			(*((u32 volatile *)(SYSVA_TIMER_BASE_ADDR + reg_offset)))
#else
#error "NO SYSTEM DEFINED"
#endif

/*
 * define access macros
 */
#define	MISC_MEMORY_REMAP_REG_ADDR			MISC_MEM_MAP_ADDR(0x00)
#define	MISC_CHIP_CONFIG_REG_ADDR			MISC_MEM_MAP_ADDR(0x04)
#define	MISC_DEBUG_PROBE_DATA_REG_ADDR			MISC_MEM_MAP_ADDR(0x08)
#define	MISC_DEBUG_PROBE_SELECTION_REG_ADDR		MISC_MEM_MAP_ADDR(0x0C)
#define	MISC_PCI_CONTROL_BROKEN_MASK_REG_ADDR		MISC_MEM_MAP_ADDR(0x10)
#define	MISC_PCI_BROKEN_STATUS_REG_ADDR			MISC_MEM_MAP_ADDR(0x14)
#define	MISC_PCI_DEVICE_VENDOR_ID_REG_ADDR		MISC_MEM_MAP_ADDR(0x18)
#define	MISC_USB_HOST_PHY_CONTROL_TEST_REG_ADDR		MISC_MEM_MAP_ADDR(0x1C)
#define	MISC_GPIOA_PIN_ENABLE_REG_ADDR			MISC_MEM_MAP_ADDR(0x20)
#define	MISC_GPIOB_PIN_ENABLE_REG_ADDR			MISC_MEM_MAP_ADDR(0x24)
#define	MISC_ETHERNET_PORT_CONFIG_REG_ADDR		MISC_MEM_MAP_ADDR(0x28)

#define	MISC_SOFTWARE_TEST_1_REG_ADDR			MISC_MEM_MAP_ADDR(0x38)
#define	MISC_SOFTWARE_TEST_2_REG_ADDR			MISC_MEM_MAP_ADDR(0x3C)

#define	MISC_E_FUSE_0_REG_ADDR				MISC_MEM_MAP_ADDR(0x60)
#define	MISC_E_FUSE_1_REG_ADDR				MISC_MEM_MAP_ADDR(0x64)


#define	MISC_MEMORY_REMAP_REG				MISC_MEM_MAP_VALUE(0x00)
#define	MISC_CHIP_CONFIG_REG				MISC_MEM_MAP_VALUE(0x04)
#define	MISC_DEBUG_PROBE_DATA_REG			MISC_MEM_MAP_VALUE(0x08)
#define	MISC_DEBUG_PROBE_SELECTION_REG			MISC_MEM_MAP_VALUE(0x0C)
#define	MISC_PCI_CONTROL_BROKEN_MASK_REG		MISC_MEM_MAP_VALUE(0x10)
#define	MISC_PCI_BROKEN_STATUS_REG			MISC_MEM_MAP_VALUE(0x14)
#define	MISC_PCI_DEVICE_VENDOR_ID_REG			MISC_MEM_MAP_VALUE(0x18)
#define	MISC_USB_HOST_PHY_CONTROL_TEST_REG		MISC_MEM_MAP_VALUE(0x1C)
#define	MISC_GPIOA_PIN_ENABLE_REG			MISC_MEM_MAP_VALUE(0x20)
#define	MISC_GPIOB_PIN_ENABLE_REG			MISC_MEM_MAP_VALUE(0x24)
#define	MISC_ETHERNET_PORT_CONFIG_REG			MISC_MEM_MAP_VALUE(0x28)

#define	MISC_SOFTWARE_TEST_1_REG			MISC_MEM_MAP_VALUE(0x38)
#define	MISC_SOFTWARE_TEST_2_REG			MISC_MEM_MAP_VALUE(0x3C)

#define	MISC_E_FUSE_0_REG				MISC_MEM_MAP_VALUE(0x60)
#define	MISC_E_FUSE_1_REG				MISC_MEM_MAP_VALUE(0x64)


/*
 * define constants macros
 */
#define	MISC_PARALLEL_FLASH_BOOT			(0)
#define	MISC_SPI_SERIAL_FLASH_BOOT			(1)

#define	MISC_LITTLE_ENDIAN				(0)
#define	MISC_BIG_ENDIAN					(1)

#define	MISC_CPU_CLOCK_166_MHZ				(0)
#define	MISC_CPU_CLOCK_200_MHZ				(1)
#define	MISC_CPU_CLOCK_233_MHZ				(2)
#define	MISC_CPU_CLOCK_266_MHZ				(3)
#define	MISC_CPU_CLOCK_300_MHZ				(4)
#define	MISC_CPU_CLOCK_333_MHZ				(5)
#define	MISC_CPU_CLOCK_366_MHZ				(6)
#define	MISC_CPU_CLOCK_400_MHZ				(7)

/*
 * Macro-defines for shared pins with GPIO_A
 */
#define	MISC_EXT_INT28_PIN				((0x1 << 0))
#define	MISC_EXT_INT29_PIN				((0x1 << 1))
#define	MISC_EXT_INT30_PIN				((0x1 << 2))

#define	MISC_I2C_PINS					((0x1 << 4) | (0x1 << 5))

#define	MISC_I2S_PINS					((0x1 << 6) | (0x1 << 7) | (0x1 << 8))

#define	MISC_SPI_PINS					((0x1 << 9) | (0x1 << 10) | (0x1 << 11) | (0x1 << 12) | (0x1 << 13) | (0x1 << 14) | (0x1 << 15))

#define	MISC_PCM_PINS					((0x1 << 16) | (0x1 << 17) | (0x1 << 18) | (0x1 << 19))

#define	MISC_SMC_CS1_PIN				((0x1 << 20))
#define	MISC_SMC_CS2_PIN				((0x1 << 21))
#define	MISC_SMC_CS3_PIN				((0x1 << 22))

#define	MISC_SMC_WAIT1_PIN				((0x1 << 23))
#define	MISC_SMC_WAIT2_PIN				((0x1 << 24))
#define	MISC_SMC_WAIT3_PIN				((0x1 << 25))

#define	MISC_UART1_TXD_PIN				((0x1 << 26))
#define	MISC_UART1_RXD_PIN				((0x1 << 27))
#define	MISC_UART1_CTS_PIN				((0x1 << 28))
#define	MISC_UART1_RTS_PIN				((0x1 << 29))

#define	MISC_WDTIMER_RESET_PIN				((0x1 << 30))

#define	MISC_CLOCK_OUTPUT_PIN				(((u_int32)0x1 << 31))


/*
 * Macro-defines for shared pins with GPIO_B
 */
#define	MISC_SMC_DATA_0_7_PINS				((0xFF))
#define	MISC_SMC_ADDR_16_23_PINS			((0xFF << 16))


/*
 * Other defines
 */
#define	MISC_GPIOA_PIN_0				(0)
#define	MISC_GPIOA_PIN_1				(1)
#define	MISC_GPIOA_PIN_2				(2)
#define	MISC_GPIOA_PIN_3				(3)
#define	MISC_GPIOA_PIN_4				(4)
#define	MISC_GPIOA_PIN_5				(5)
#define	MISC_GPIOA_PIN_6				(6)
#define	MISC_GPIOA_PIN_7				(7)
#define	MISC_GPIOA_PIN_8				(8)
#define	MISC_GPIOA_PIN_9				(9)
#define	MISC_GPIOA_PIN_10				(10)
#define	MISC_GPIOA_PIN_11				(11)
#define	MISC_GPIOA_PIN_12				(12)
#define	MISC_GPIOA_PIN_13				(13)
#define	MISC_GPIOA_PIN_14				(14)
#define	MISC_GPIOA_PIN_15				(15)

#define	MISC_GPIOA_RESISTOR_PULL_DOWN			(1)
#define	MISC_GPIOA_RESISTOR_PULL_UP			(1)

/*
 * macro declarations
 */
#define	HAL_MISC_GET_SYSTEM_ENDIAN_MODE(endian_mode) \
{ \
    (endian_mode) = (MISC_CHIP_CONFIG_REG >> 1)	& 0x1; \
}

#define	HAL_MISC_GET_SYSTEM_CPU_CLOCK(cpu_clock) \
{ \
    (cpu_clock)	= (MISC_CHIP_CONFIG_REG	>> 2) &	0x7; \
}

#define	HAL_MISC_ENABLE_SPI_SERIAL_FLASH_BANK_ACCESS() \
{ \
    (MISC_CHIP_CONFIG_REG) |= (0x1 << 16); \
}

#define	HAL_MISC_DISABLE_SPI_SERIAL_FLASH_BANK_ACCESS()	\
{ \
    (MISC_CHIP_CONFIG_REG) &= ~(0x1 << 16); \
}

#define	HAL_MISC_MASK_PCI_DEVICE_BROKEN_INTERRUPTS() \
{ \
    (MISC_PCI_CONTROL_BROKEN_MASK_REG) |= 0x1F;	\
}

#define	HAL_MISC_UNMASK_PCI_DEVICE_BROKEN_INTERRUPTS() \
{ \
    (MISC_PCI_CONTROL_BROKEN_MASK_REG) &= ~0x1F; \
}

#define	HAL_MISC_GET_PCI_DEVICE_BROKEN_STATUS(broken_status) \
{ \
    (broken_status) = (MISC_PCI_BROKEN_STATUS_REG & 0x1F); \
}

#define	HAL_MISC_CLEAR_PCI_DEVICE_BROKEN_STATUS(broken_status) \
{ \
    (MISC_PCI_BROKEN_STATUS_REG) = (broken_status & 0x1F); \
}

#define	HAL_MISC_EXIT_USB_PORT0_SUSPEND_MODE() \
{ \
    (MISC_USB_HOST_PHY_CONTROL_TEST_REG) |= (0x1 << 2);	\
}

#define	HAL_MISC_EXIT_USB_PORT1_SUSPEND_MODE() \
{ \
    (MISC_USB_HOST_PHY_CONTROL_TEST_REG) |= (0x1 << 3);	\
}


#define	HAL_MISC_ENABLE_USB_PORT0_LOOPBACK_TEST_MODE() \
{ \
    (MISC_USB_HOST_PHY_CONTROL_TEST_REG) &= ~(0x1 << 5); \
    (MISC_USB_HOST_PHY_CONTROL_TEST_REG) |= (0x1 << 4);	\
}

#define	HAL_MISC_ENABLE_USB_PORT1_LOOPBACK_TEST_MODE() \
{ \
    (MISC_USB_HOST_PHY_CONTROL_TEST_REG) |= (0x1 << 5);	\
    (MISC_USB_HOST_PHY_CONTROL_TEST_REG) |= (0x1 << 4);	\
}

#define	HAL_MISC_DISABLE_USB_LOOPBACK_TEST_MODE() \
{ \
    (MISC_USB_HOST_PHY_CONTROL_TEST_REG) &= ~(0x1 << 4); \
}

/*
 * Macro defines for GPIOA and GPIOB Pin Enable	Register
 */
#define	HAL_MISC_ENABLE_EXT_INT28_PIN()	\
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	|= (MISC_EXT_INT28_PIN); \
}

#define	HAL_MISC_DISABLE_EXT_INT28_PIN() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	&= ~(MISC_EXT_INT28_PIN); \
}

#define	HAL_MISC_ENABLE_EXT_INT29_PIN()	\
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	|= (MISC_EXT_INT29_PIN); \
}

#define	HAL_MISC_DISABLE_EXT_INT29_PIN() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	&= ~(MISC_EXT_INT29_PIN); \
}

#define	HAL_MISC_ENABLE_EXT_INT30_PIN()	\
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	|= (MISC_EXT_INT30_PIN); \
}

#define	HAL_MISC_DISABLE_EXT_INT30_PIN() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	&= ~(MISC_EXT_INT30_PIN); \
}

#define	HAL_MISC_ENABLE_I2C_PINS() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	|= (MISC_I2C_PINS); \
}

#define	HAL_MISC_DISABLE_I2C_PINS() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	&= ~(MISC_I2C_PINS); \
}

#define	HAL_MISC_ENABLE_I2S_PINS() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	|= (MISC_I2S_PINS); \
}

#define	HAL_MISC_DISABLE_I2S_PINS() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	&= ~(MISC_I2S_PINS); \
}

#define	HAL_MISC_ENABLE_SPI_PINS() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	|= (MISC_SPI_PINS); \
}

#define	HAL_MISC_DISABLE_SPI_PINS() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	&= ~(MISC_SPI_PINS); \
}

#define	HAL_MISC_ENABLE_PCM_PINS() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	|= (MISC_PCM_PINS); \
}

#define	HAL_MISC_DISABLE_PCM_PINS() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	&= ~(MISC_PCM_PINS); \
}

#define	HAL_MISC_ENABLE_SMC_CS1_PIN() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	|= (MISC_SMC_CS1_PIN); \
}

#define	HAL_MISC_DISABLE_SMC_CS1_PIN() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	&= ~(MISC_SMC_CS1_PIN);	\
}

#define	HAL_MISC_ENABLE_SMC_CS2_PIN() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	|= (MISC_SMC_CS2_PIN); \
}

#define	HAL_MISC_DISABLE_SMC_CS2_PIN() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	&= ~(MISC_SMC_CS2_PIN);	\
}

#define	HAL_MISC_ENABLE_SMC_CS3_PIN() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	|= (MISC_SMC_CS3_PIN); \
}

#define	HAL_MISC_DISABLE_SMC_CS3_PIN() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	&= ~(MISC_SMC_CS3_PIN);	\
}

#define	HAL_MISC_ENABLE_SMC_WAIT1_PIN()	\
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	|= (MISC_SMC_WAIT1_PIN); \
}

#define	HAL_MISC_DISABLE_SMC_WATI1_PIN() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	&= ~(MISC_SMC_WAIT1_PIN); \
}

#define	HAL_MISC_ENABLE_SMC_WAIT2_PIN()	\
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	|= (MISC_SMC_WAIT2_PIN); \
}

#define	HAL_MISC_DISABLE_SMC_WATI2_PIN() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	&= ~(MISC_SMC_WAIT2_PIN); \
}

#define	HAL_MISC_ENABLE_SMC_WAIT3_PIN()	\
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	|= (MISC_SMC_WAIT3_PIN); \
}

#define	HAL_MISC_DISABLE_SMC_WATI3_PIN() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	&= ~(MISC_SMC_WAIT3_PIN); \
}


#define	HAL_MISC_ENABLE_UART1_TXD_PIN()	\
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	|= (MISC_UART1_TXD_PIN); \
}

#define	HAL_MISC_DISABLE_UART1_TXD_PIN() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	&= ~(MISC_UART1_TXD_PIN); \
}

#define	HAL_MISC_ENABLE_UART1_RXD_PIN()	\
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	|= (MISC_UART1_RXD_PIN); \
}

#define	HAL_MISC_DISABLE_UART1_RXD_PIN() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	&= ~(MISC_UART1_RXD_PIN); \
}

#define	HAL_MISC_ENABLE_UART1_CTS_PIN()	\
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	|= (MISC_UART1_CTS_PIN); \
}

#define	HAL_MISC_DISABLE_UART1_CTS_PIN() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	&= ~(MISC_UART1_CTS_PIN); \
}

#define	HAL_MISC_ENABLE_UART1_RTS_PIN()	\
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	|= (MISC_UART1_RTS_PIN); \
}

#define	HAL_MISC_DISABLE_UART1_RTS_PIN() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	&= ~(MISC_UART1_RTS_PIN); \
}

#define	HAL_MISC_ENABLE_WDTIMER_RESET_PIN() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	|= (MISC_WDTIMER_RESET_PIN); \
}

#define	HAL_MISC_DISABLE_WDTIMER_RESET_PIN() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	&= ~(MISC_WDTIMER_RESET_PIN); \
}

#define	HAL_MISC_ENABLE_CLOCK_OUTPUT_PIN() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	|= (MISC_CLOCK_OUTPUT_PIN); \
}

#define	HAL_MISC_DISABLE_CLOCK_OUTPUT_PIN() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	&= ~(MISC_CLOCK_OUTPUT_PIN); \
}

/*
 * Macro-defines for GPIO_B
 */
#define	HAL_MISC_ENABLE_SMC_DATA_0_7_PINS() \
{ \
    (MISC_GPIOB_PIN_ENABLE_REG)	|= (MISC_SMC_DATA_0_7_PINS); \
}

#define	HAL_MISC_DISABLE_SMC_DATA_0_7_PINS() \
{ \
    (MISC_GPIOB_PIN_ENABLE_REG)	&= ~(MISC_SMC_DATA_0_7_PINS); \
}

#define	HAL_MISC_ENABLE_SMC_ADDR_16_23_PINS() \
{ \
    (MISC_GPIOB_PIN_ENABLE_REG)	|= (MISC_SMC_ADDR_16_23_PINS); \
}

#define	HAL_MISC_DISABLE_SMC_ADDR_16_23_PINS() \
{ \
    (MISC_GPIOB_PIN_ENABLE_REG)	&= ~(MISC_SMC_ADDR_16_23_PINS);	\
}

#define	HAL_MISC_ENABLE_ALL_SHARED_GPIO_PINS() \
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	= (0x0); \
    (MISC_GPIOB_PIN_ENABLE_REG)	= (0x0); \
}

#define	HAL_MISC_DISABLE_ALL_SHARED_GPIO_PINS()	\
{ \
    (MISC_GPIOA_PIN_ENABLE_REG)	= (0xFFFFFFFF);	\
    (MISC_GPIOB_PIN_ENABLE_REG)	= (0xFFFFFFFF);	\
}

#define	HAL_MISC_CONNECT_VTSS_MAC5_TO_GMII0() \
{ \
    (MISC_ETHERNET_PORT_CONFIG_REG) &= ~(0x1 <<	0); \
}

#define	HAL_MISC_CONNECT_STAR_MAC1_TO_GMII0() \
{ \
    (MISC_ETHERNET_PORT_CONFIG_REG) |= (0x1 << 0); \
}

#define	HAL_MISC_CONNECT_VTSS_MAC6_TO_RGMII1() \
{ \
    (MISC_ETHERNET_PORT_CONFIG_REG) &= ~(0x3 <<	1); \
}

#define	HAL_MISC_CONNECT_VTSS_MAC6_TO_STAR_MAC1() \
{ \
    (MISC_ETHERNET_PORT_CONFIG_REG) &= ~(0x3 <<	1); \
    (MISC_ETHERNET_PORT_CONFIG_REG) |= (0x1 << 1); \
}

#define	HAL_MISC_CONNECT_STAR_MAC1_TO_RGMII1() \
{ \
    (MISC_ETHERNET_PORT_CONFIG_REG) &= ~(0x3 <<	1); \
    (MISC_ETHERNET_PORT_CONFIG_REG) |= (0x2 << 1); \
}

#define	HAL_MISC_ENABLE_VTSS_MDC_MDIO_PINS() \
{ \
    (MISC_ETHERNET_PORT_CONFIG_REG) &= ~(0x1 <<	3); \
}

#define	HAL_MISC_ENABLE_STAR_MDC_MDIO_PINS() \
{ \
    (MISC_ETHERNET_PORT_CONFIG_REG) |= (0x1 << 3); \
}

#define	HAL_MISC_TRIGGER_INTERRUPT_TO_VTSS_8051() \
{ \
    (MISC_ETHERNET_PORT_CONFIG_REG) |= (0x1 << 4); \
    (MISC_ETHERNET_PORT_CONFIG_REG) &= ~(0x1 <<	4); \
    (MISC_ETHERNET_PORT_CONFIG_REG) &= ~(0x1 <<	4); \
    (MISC_ETHERNET_PORT_CONFIG_REG) &= ~(0x1 <<	4); \
    (MISC_ETHERNET_PORT_CONFIG_REG) |= (0x1 << 4); \
}

#endif	// end of #ifndef _STAR_MISC_H_

