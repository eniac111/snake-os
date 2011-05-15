/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 * Gary Jennejohn <gj@denx.de>
 * David Mueller <d.mueller@elsoft.ch>
 *
 * Configuation settings for the SAMSUNG SMDK2410 board.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_ARM920T		1	/* This is an ARM920T Core */
#define	CONFIG_STR8100		1	/* in a STR8100 SoC */
#define CONFIG_STAR_EQUULEUS	1	/* on a STAR Equuleus Board */
/*For CONFIG_LOW_FREQ , low down CPU 250MHZ to 200MHZ */
#undef CONFIG_LOW_FREQ

#ifdef CONFIG_LOW_FREQ
#define CONFIG_REBOOTIDX	0 /* Initial value */
#endif /* CONFIG_LOW_FREQ */


/* input clock of PLL */
#if 1
#define CONFIG_SYS_CLK_FREQ	100000000 /* the Equuleus has 100MHz input clock */
#else // on FPGA
#define CONFIG_SYS_CLK_FREQ	13000000 /* the Equuleus has 13MHz input clock */
#endif

//#define CONFIG_CPU_CLOCK	250
//#define CONFIG_CPU_CLOCK	225
//#define CONFIG_CPU_CLOCK	200
#define CONFIG_CPU_CLOCK	175

//#define USE_920T_MMU		1
#define CONFIG_USE_IRQ		1
//#undef CONFIG_USE_IRQ			/* we don't need IRQ/FIQ stuff */

/*
 * Size of malloc() pool
 */
#define CFG_MALLOC_LEN		(CFG_ENV_SIZE + 1024*1024)
#define CFG_GBL_DATA_SIZE	128	/* size in bytes reserved for initial data */

/*
 * Hardware drivers
 */
#define CONFIG_PCI			/* include pci support          */
#define CONFIG_PCI_PNP			/* do (not) pci plug-and-play   */
#define CONFIG_PCI_SCAN_SHOW		/* show pci devices on startup  */

#define CONFIG_STAR_NIC			/* enable Star NIC driver */
#define CONFIG_STAR_NIC_PHY_INTERNAL	/* internal phy support */
//#define CONFIG_STAR_NIC_PHY_VSC8601	/* VSC8601 phy support */
//#define CONFIG_RTL8139
//#define CONFIG_EEPRO100
//#define CONFIG_E1000
//#define CONFIG_MII
//#define CONFIG_NET_MULTI

/*
 * select serial console configuration
 */
#define CONFIG_CONSOLE_UART_PORT	0	/* we use serial port on Equuleus */

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

#define CONFIG_BAUDRATE		38400

/***********************************************************
 * Command definition
 ***********************************************************/
#define CONFIG_BOOTP_MASK	(CONFIG_BOOTP_DEFAULT)

#define CONFIG_COMMANDS (CONFIG_CMD_DFL | CFG_CMD_PCI)

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

#define CONFIG_BOOTDELAY	8
/*#define CONFIG_BOOTARGS    	"root=ramfs devfs=mount console=ttySA0,9600" */
#define CONFIG_ETHADDR		00:aa:bb:cc:dd:10
#define CONFIG_NETMASK          255.255.255.0
#define CONFIG_IPADDR		192.168.0.1
#define CONFIG_SERVERIP		192.168.0.100

//#define CONFIG_BOOTUSBDCOMMAND

#ifdef CONFIG_BOOTUSBDCOMMAND
#define CONFIG_BOOTCOMMAND	"bootusbd;go 0x10040000"
#undef CONFIG_BOOTUSBDCOMMAND
#define CONFIG_BOOTUSBDCOMMAND	"go 0xFE0000"
#else
#define CONFIG_BOOTCOMMAND	"go 0x10040000"
#endif

/*#define CONFIG_BOOTFILE	"elinos-lart" */
/*#define CONFIG_BOOTCOMMAND	"tftp; bootm" */
#define	CONFIG_EXTRA_ENV_SETTINGS					\
	"cpu_clock=250\0"						\
	""

#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	38400		/* speed to run kgdb serial port */
/* what's this ? it's not used anywhere */
#define CONFIG_KGDB_SER_INDEX	1		/* which serial port to use */
#endif

/*
 * Miscellaneous configurable options
 */
#define	CFG_LONGHELP				/* undef to save memory */
#define	CFG_PROMPT		"Star Equuleus # "	/* Monitor Command Prompt */
#define	CFG_CBSIZE		256		/* Console I/O Buffer Size */
#define	CFG_PBSIZE		(CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */
#define	CFG_MAXARGS		16		/* max number of command args */
#define CFG_BARGSIZE		CFG_CBSIZE	/* Boot Argument Buffer Size */

#define CFG_MEMTEST_START	0x00100000	/* memtest works on */
#define CFG_MEMTEST_END		PHYS_SDRAM_1_SIZE

#undef  CFG_CLKS_IN_HZ		/* everything, incl board info, in Hz */

#define	CFG_LOAD_ADDR		0x00800000	/* default load address	*/

#define	CFG_HZ			1000

/* valid baudrates */
#define CFG_BAUDRATE_TABLE	{ 9600, 14400, 19200, 28800, 38400, 57600, 115200 }

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	(128*1024)	/* regular stack */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	(4*1024)	/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ	(4*1024)	/* FIQ stack */
#endif

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */

//#define CONFIG_USE_SDRAM			/* use sdram */
#define CONFIG_USE_DDR				/* use ddr ram */


#define RAM_SIZE_16MBIT		(0)
#define RAM_SIZE_64MBIT		(1)
#define RAM_SIZE_128MBIT	(2)
#define RAM_SIZE_256MBIT	(3)
#define RAM_SIZE_512MBIT	(4)


#define RAM_SIZE		RAM_SIZE_256MBIT
#define CONFIG_NR_DRAM_BANKS	1		/* we have 1 bank of DRAM */
#define PHYS_SDRAM_1		0x00000000	/* SDRAM Bank #1 */
#if (RAM_SIZE == RAM_SIZE_64MBIT)
#define PHYS_SDRAM_1_SIZE	0x00800000	/* 08 MB */
#elif (RAM_SIZE == RAM_SIZE_128MBIT)
#define PHYS_SDRAM_1_SIZE	0x01000000	/* 16 MB */
#elif (RAM_SIZE == RAM_SIZE_256MBIT)
#define PHYS_SDRAM_1_SIZE	0x02000000	/* 32 MB */
#elif (RAM_SIZE == RAM_SIZE_512MBIT)
#define PHYS_SDRAM_1_SIZE	0x04000000	/* 64 MB */
#endif

#define PHYS_FLASH_1		0x10000000	/* Flash Bank #1 */
#define CFG_FLASH_BASE		PHYS_FLASH_1

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CFG_MAX_FLASH_BANKS	1		/* max number of memory banks */
#define PHYS_FLASH_SIZE		0x00800000	/* 8MB */
#define CFG_MAX_FLASH_SECT	(512)		/* max number of sectors on one chip */

/* timeout values are in ticks */
#define CFG_FLASH_ERASE_TOUT	(5*CFG_HZ)	/* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT	(5*CFG_HZ)	/* Timeout for Flash Write */

/* SPI Flash Support */
#define CONFIG_SPI_FLASH_BOOT_SUPPORT		/* support boot from spi flash */
#define CONFIG_SPI_FLASH_SUPPORT

#ifdef CONFIG_SPI_FLASH_SUPPORT
#define CFG_SPI_FLASH_CHANNEL		0
#define CFG_SPI_FLASH_BASE		0x30000000

#define CONFIG_HAS_DATAFLASH		1
#define CFG_MAX_DATAFLASH_BANKS		1
#define CFG_DATAFLASH_LOGIC_ADDR_CS0	CFG_SPI_FLASH_BASE /* Logical adress for CS0 */ 
#endif

#ifdef CONFIG_SPI_FLASH_BOOT_SUPPORT
#undef CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND		"go 0x1000000"
#ifdef CONFIG_BOOTUSBDCOMMAND
#undef CONFIG_BOOTCOMMAND
#undef CONFIG_BOOTUSBDCOMMAND
#define CONFIG_BOOTCOMMAND		"bootusbd;go 0x1000000"
#define CONFIG_BOOTUSBDCOMMAND		"go 0xFE0000"
#endif
#define CFG_ENV_IS_IN_DATAFLASH		1
#define CFG_ENV_OFFSET			0x20000		/* environment offset on dataflash */
#define CFG_ENV_ADDR			(CFG_DATAFLASH_LOGIC_ADDR_CS0 + CFG_ENV_OFFSET) /* addr of environment */
#define CFG_ENV_SIZE			0x10000		/* Total Size of Environment Sector */
#define CFG_KERNEL_OFFSET		0x40000		/* linux kernel image offset on dataflash */
#define CFG_KERNEL_END			0x7FFFFF	/* linux kernel image end on dataflash */
#define CFG_USBD_OFFSET			0x30000
#define CFG_USBD_END			0x3FFFF
#else
#define	CFG_ENV_IS_IN_FLASH		1
#define CFG_ENV_OFFSET			0x20000		/* environment offset on flash */
#define CFG_ENV_ADDR			(CFG_FLASH_BASE + CFG_ENV_OFFSET) /* addr of environment */
#define CFG_ENV_SIZE			0x10000		/* Total Size of Environment Sector */
#endif


#endif	/* __CONFIG_H */

