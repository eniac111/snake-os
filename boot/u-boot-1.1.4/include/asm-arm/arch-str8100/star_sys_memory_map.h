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

#ifndef	_STAR_SYS_MEMORY_MAP_H_
#define	_STAR_SYS_MEMORY_MAP_H_

#if 1
#define __UBOOT__
#else
#define __LINUX__
#endif

/*
 * sytem memory	mapping	after reset
 */
#define SYSPA_FLASH_SRAM_BANK0_BASE_ADDR	0x10000000
#define SYSPA_FLASH_SRAM_BANK1_BASE_ADDR	0x20000000
#define SYSPA_FLASH_SRAM_BANK2_BASE_ADDR	0x30000000
#define SYSPA_FLASH_SRAM_BANK3_BASE_ADDR	0x40000000
#define	SYSPA_PCMCIA_ATTRIBUTE_MEMORY_BASE_ADDR	0x14000000
#define	SYSPA_PCMCIA_COMMON_MEMORY_BASE_ADDR	0x15000000
#define	SYSPA_PCMCIA_IO_SPACE_BASE_ADDR		0x16000000
#define	SYSPA_IDE_DEVICE_BASE_ADDR		0x18000000
#define	SYSPA_SDRAM_MEMORY_BASE_ADDR		0x20000000
#define	SYSPA_GDMAC_BASE_ADDR			0x60000000
#define	SYSPA_NIC_BASE_ADDR			0x70000000
#define	SYSPA_SPI_BASE_ADDR			0x71000000
#define	SYSPA_PCM_BASE_ADDR			0x71000000
#define	SYSPA_I2C_BASE_ADDR			0x71000000
#define	SYSPA_I2S_BASE_ADDR			0x71000000
#define	SYSPA_DDRC_SDRC_BASE_ADDR		0x72000000
#define	SYSPA_SMC_BASE_ADDR			0x73000000
#define	SYSPA_PCMCIA_CONTROL_BASE_ADDR		0x73000000
#define	SYSPA_IDE_CONTROLLER_BASE_ADDR		0x74000000
#define	SYSPA_MISC_BASE_ADDR			0x76000000
#define	SYSPA_POWER_MANAGEMENT_BASE_ADDR	0x77000000
#define	SYSPA_UART0_BASE_ADDR			0x78000000
#define	SYSPA_UART1_BASE_ADDR			0x78800000
#define	SYSPA_TIMER_BASE_ADDR			0x79000000
#define	SYSPA_WATCHDOG_TIMER_BASE_ADDR		0x7A000000
#define	SYSPA_RTC_BASE_ADDR			0x7B000000
#define	SYSPA_GPIOA_BASE_ADDR			0x7C000000
#define	SYSPA_GPIOB_BASE_ADDR			0x7C800000
#define	SYSPA_PCI_BRIDGE_CONFIG_DATA_BASE_ADDR	0xA0000000
#define	SYSPA_PCI_BRIDGE_CONFIG_ADDR_BASE_ADDR	0xA4000000
#define	SYSPA_PCI_IO_SPACE_BASE_ADDR		0xA8000000
#define	SYSPA_PCI_MEMORY_SPACE_BASE_ADDR	0xB0000000
#define	SYSPA_USB11_CONFIG_BASE_ADDR		0xC0000000
#define	SYSPA_USB11_OPERATION_BASE_ADDR		0xC4000000
#define	SYSPA_USB20_CONFIG_BASE_ADDR		0xC8000000
#define	SYSPA_USB20_OPERATION_BASE_ADDR		0xCC000000
#define	SYSPA_USB20_DEVICE_BASE_ADDR		0xD0000000
#define	SYSPA_VIC_BASE_ADDR			0xFFFFF000

#if defined(__LINUX__)
#define SYSVA_IDE_DEVICE_BASE_ADDR		0xFFF00000
#define SYSVA_GDMAC_BASE_ADDR			0xFFF01000
#define SYSVA_NIC_BASE_ADDR			0xFFF02000
#define SYSVA_SPI_BASE_ADDR			0xFFF03000
#define SYSVA_PCM_BASE_ADDR			0xFFF04000
#define SYSVA_I2C_BASE_ADDR			0xFFF05000
#define SYSVA_I2S_BASE_ADDR			0xFFF06000
#define SYSVA_DDRC_SDRC_BASE_ADDR		0xFFF07000
#define SYSVA_SMC_BASE_ADDR			0xFFF08000
#define SYSVA_PCMCIA_CONTROL_BASE_ADDR		0xFFF08000
#define SYSVA_IDE_CONTROLLER_BASE_ADDR		0xFFF09000
#define SYSVA_MISC_BASE_ADDR			0xFFF0A000
#define SYSVA_POWER_MANAGEMENT_BASE_ADDR	0xFFF0B000
#define SYSVA_UART0_BASE_ADDR			0xFFF0C000
#define SYSVA_UART1_BASE_ADDR			0xFFF0D000
#define SYSVA_TIMER_BASE_ADDR			0xFFF0E000
#define SYSVA_WATCHDOG_TIMER_BASE_ADDR		0xFFF0F000
#define SYSVA_RTC_BASE_ADDR			0xFFF10000
#define SYSVA_GPIOA_BASE_ADDR			0xFFF11000
#define SYSVA_GPIOB_BASE_ADDR			0xFFF12000
#define SYSVA_PCI_BRIDGE_CONFIG_DATA_BASE_ADDR	0xFFF13000
#define SYSVA_PCI_BRIDGE_CONFIG_ADDR_BASE_ADDR	0xFFF14000
#define SYSVA_USB11_CONFIG_BASE_ADDR		0xFFF15000
#define SYSVA_USB11_OPERATION_BASE_ADDR		0xFFF16000
#define SYSVA_USB20_CONFIG_BASE_ADDR		0xFFF17000
#define SYSVA_USB20_OPERATION_BASE_ADDR		0xFFF18000
#define SYSVA_USB20_DEVICE_BASE_ADDR		0xFFF19000
#define SYSVA_VIC_BASE_ADDR			0xFFF1A000
#endif //__LINUX__

#endif // end of #ifndef _STAR_SYS_MEMORY_MAP_H_
