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

#ifndef	_STAR_SYSPA_MEMORY_MAP_H_
#define	_STAR_SYSPA_MEMORY_MAP_H_

#if 1
#define	__UBOOT__
#else
#define	__LINUX__
#endif

/*
 * sytem memory	mapping	after reset
 */
#define	SYSPA_ALIASED_FLASH_SRAM_BASE_ADDR	0x00000000
#define	SYSPA_FLASH_SRAM_BANK0_BASE_ADDR	0x10000000
#define	SYSPA_FLASH_SRAM_BANK1_BASE_ADDR	0x11000000
#define	SYSPA_FLASH_SRAM_BANK2_BASE_ADDR	0x12000000
#define	SYSPA_FLASH_SRAM_BANK3_BASE_ADDR	0x13000000
#define	SYSPA_FLASH_SRAM_BANK4_BASE_ADDR	0x14000000
#define	SYSPA_SDRAM_MEMORY_BASE_ADDR		0x20000000
#define	SYSPA_SPI_FLASH_BASE_ADDR		0x30000000
#define	SYSPA_GDMAC_BASE_ADDR			0x60000000
#define	SYSPA_SWITCH_BASE_ADDR			0x70000000
#define	SYSPA_I2C_BASE_ADDR			0x71000000
#define	SYSPA_SPI_BASE_ADDR			0x71000000
#define	SYSPA_PCM_BASE_ADDR			0x71000000
#define	SYSPA_I2S_BASE_ADDR			0x71000000
#define	SYSPA_DDRC_SDRC_BASE_ADDR		0x72000000
#define	SYSPA_SMC_BASE_ADDR			0x73000000
#define	SYSPA_MISC_BASE_ADDR			0x76000000
#define	SYSPA_POWER_MANAGEMENT_BASE_ADDR	0x77000000
#define	SYSPA_UART0_BASE_ADDR			0x78000000
#define	SYSPA_UART1_BASE_ADDR			0x78800000
#define	SYSPA_TIMER_BASE_ADDR			0x79000000
#define	SYSPA_WATCHDOG_TIMER_BASE_ADDR		0x7A000000
#define	SYSPA_RTC_BASE_ADDR			0x7B000000
#define	SYSPA_GPIOA_BASE_ADDR			0x7C000000
#define	SYSPA_GPIOB_BASE_ADDR			0x7C800000
#define	SYSPA_IDP_BASE_ADDR			0x7D000000
#define	SYSPA_RTL_DEBUG_BASE_ADDR		0x7F000000
#define	SYSPA_PCI_DATA_REG_BASE_ADDR		0xA0000000
#define	SYSPA_PCI_ADDR_REG_BASE_ADDR		0xA4000000
#define	SYSPA_PCI_IO_SPACE_BASE_ADDR		0xA8000000
#define	SYSPA_PCI_MEMORY_SPACE_BASE_ADDR	0xB0000000
#define	SYSPA_USB20_CONFIG_BASE_ADDR		0xC0000000
#define	SYSPA_VITESSE_SWITCH_BASE_ADDR		0xE0000000
#define	SYSPA_VIC_BASE_ADDR			0xFFFFF000

#if defined(__LINUX__)

#endif

#endif // end of #ifndef _STAR_SYSPA_MEMORY_MAP_H_

