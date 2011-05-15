/*
 * (C) Copyright 2000-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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

#include <common.h>

#ifdef CONFIG_HAS_DATAFLASH

#include <dataflash.h>
#include <asm/arch/star_misc.h>
#include <asm/arch/star_spi.h>
#include "spi_flash.h"

static u32 Spi_Flash_Set_Write_Enable(u8 spi_flash_channel);
static u32 Spi_Flash_Is_Flash_Ready(u8 spi_flash_channel);

struct spi_flash_info
{
	u32 sectors;
	u32 sector_size;
	u32 pages;
	u32 page_size;
};

static struct spi_flash_info spi_flash_bank[CFG_MAX_DATAFLASH_BANKS];

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Initialize
 * PURPOSE:
 *
 ******************************************************************************/
static void
Spi_Flash_Initialize(u8 spi_flash_channel)
{
	u32 volatile receive_data;

	// Enable SPI pins
	HAL_MISC_ENABLE_SPI_PINS();

	// Disable SPI serial flash access through 0x30000000 region
	HAL_MISC_DISABLE_SPI_SERIAL_FLASH_BANK_ACCESS();

	/*
	 * Note SPI is NOT enabled after this function is invoked!!
	 */
	SPI_CONFIGURATION_REG =
		(((0x0 & 0x3) << 0) | /* 8bits shift length */
		 (0x0 << 9) | /* general SPI mode */
		 (0x0 << 10) | /* disable FIFO */
		 (0x1 << 11) | /* SPI master mode */
		 (0x0 << 12) | /* disable SPI loopback mode */
		 (0x0 << 13) |
		 (0x0 << 14) |
		 (0x0 << 24) | /* Disable SPI Data Swap */
		 (0x0 << 30) | /* Disable SPI High Speed Read for BootUp */
		 (0x0 << 31)); /* Disable SPI */

	SPI_BIT_RATE_CONTROL_REG = 0x1 & 0x07; // PCLK/8

	// Configure SPI's Tx channel
	SPI_TRANSMIT_CONTROL_REG &= ~(0x03);
	SPI_TRANSMIT_CONTROL_REG |= spi_flash_channel & 0x03;

	// Configure Tx FIFO Threshold
	SPI_FIFO_TRANSMIT_CONFIG_REG &= ~(0x03 << 4);
	SPI_FIFO_TRANSMIT_CONFIG_REG |= ((0x0 & 0x03) << 4);

	// Configure Rx FIFO Threshold
	SPI_FIFO_RECEIVE_CONFIG_REG &= ~(0x03 << 4);
	SPI_FIFO_RECEIVE_CONFIG_REG |= ((0x1 & 0x03) << 4);

	SPI_INTERRUPT_ENABLE_REG = 0;

	// Clear spurious interrupt sources
	SPI_INTERRUPT_STATUS_REG = (0xF << 4);

	receive_data = SPI_RECEIVE_BUFFER_REG;

	// Enable SPI
	SPI_CONFIGURATION_REG |= (0x1 << 31);

	return;
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Is_Bus_Idle
 * PURPOSE:
 *
 ******************************************************************************/
static inline u32
Spi_Flash_Is_Bus_Idle(void)
{
	/*
	 * Return value :
	 *    1 : Bus Idle
	 *    0 : Bus Busy
	 */
	return ((SPI_SERVICE_STATUS_REG & 0x1) ? 0 : 1);
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Is_Tx_Buffer_Empty
 * PURPOSE:
 *
 ******************************************************************************/
static inline u32
Spi_Flash_Is_Tx_Buffer_Empty(void)
{
	/*
	 * Return value :
	 *    1 : SPI Tx Buffer Empty
	 *    0 : SPI Tx Buffer Not Empty
	 */
	return ((SPI_INTERRUPT_STATUS_REG & (0x1 << 3)) ? 1 : 0);
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Is_Rx_Buffer_Full
 * PURPOSE:
 *
 ******************************************************************************/
static inline u32
Spi_Flash_Is_Rx_Buffer_Full(void)
{
	/*
	 * Return value :
	 *    1 : SPI Rx Buffer Full
	 *    0 : SPI Rx Buffer Not Full
	 */
	return ((SPI_INTERRUPT_STATUS_REG & (0x1 << 2)) ? 1 : 0);
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Buffer_Transmit_Receive
 * PURPOSE:
 *
 ******************************************************************************/
static u32
Spi_Flash_Buffer_Transmit_Receive(u32 tx_channel, u32 tx_eof_flag, u32 tx_data, u32 * rx_data)
{
	u32 volatile rx_channel;
	u32 volatile rx_eof_flag;

	/*
	 * 1. Wait until SPI Bus is idle, and Tx Buffer is empty
	 * 2. Configure Tx channel and Back-to-Back transmit EOF setting
	 * 3. Write Tx Data 
	 * 4. Wait until Rx Buffer is full
	 * 5. Get Rx channel and Back-to-Back receive EOF setting
	 * 6. Get Rx Data
	 */
	while (!Spi_Flash_Is_Bus_Idle()) ;

	while (!Spi_Flash_Is_Tx_Buffer_Empty()) ;

	SPI_TRANSMIT_CONTROL_REG &= ~(0x7);
	SPI_TRANSMIT_CONTROL_REG |= (tx_channel & 0x3) | ((tx_eof_flag & 0x1) << 2);

	SPI_TRANSMIT_BUFFER_REG = tx_data;

	while (!Spi_Flash_Is_Rx_Buffer_Full()) ;

	rx_channel = (SPI_RECEIVE_CONTROL_REG & 0x3);

	rx_eof_flag = (SPI_RECEIVE_CONTROL_REG & (0x1 << 2)) ? 1 : 0;

	*rx_data = SPI_RECEIVE_BUFFER_REG;

	if ((tx_channel != rx_channel) || (tx_eof_flag != rx_eof_flag)) {
		return 0;	// Failed!!
	} else {
		return 1;	// OK!!
	}
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Read_Status_Register
 * PURPOSE:
 *
 ******************************************************************************/
static void
Spi_Flash_Read_Status_Register(u8 spi_flash_channel, u8 * status_reg)
{
	u32 rx_data;

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, SPI_FLASH_RDSR_OPCODE, &rx_data);

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 1, 0xFF, &rx_data);

	*status_reg = (u8) (rx_data & 0xFF);
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Write_Status_Register
 * PURPOSE:
 *
 ******************************************************************************/
static u32
Spi_Flash_Write_Status_Register(u8 spi_flash_channel, u8 status_reg)
{
	u32 rx_data;

	/*
	 * First, issue "Write Enable" instruction, and then issue "Write Status
	 * Register" instruction 
	 */
	if (Spi_Flash_Set_Write_Enable(spi_flash_channel)) {
		Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, SPI_FLASH_WRSR_OPCODE, &rx_data);

		Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 1, (u32) status_reg, &rx_data);

		// Wait until this command is complete
		while (!Spi_Flash_Is_Flash_Ready(spi_flash_channel)) ;

		return 1;
	} else {
		return 0;
	}
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Is_Flash_Ready
 * PURPOSE:
 *
 ******************************************************************************/
static u32
Spi_Flash_Is_Flash_Ready(u8 spi_flash_channel)
{
	u8 status_reg;

	/*
	 * Return value :
	 *    1 : SPI Flash is ready
	 *    0 : SPI Flash is busy
	 */
	Spi_Flash_Read_Status_Register(spi_flash_channel, &status_reg);

	return (status_reg & SPI_FLASH_WIP_BIT) ? 0 : 1;
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Set_Write_Enable
 * PURPOSE:
 *
 ******************************************************************************/
static u32
Spi_Flash_Set_Write_Enable(u8 spi_flash_channel)
{
	u32 rx_data;
	u8 status_reg;

	// Wait until Flash is ready
	while (!Spi_Flash_Is_Flash_Ready(spi_flash_channel)) ;

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 1, SPI_FLASH_WREN_OPCODE, &rx_data);

	Spi_Flash_Read_Status_Register(spi_flash_channel, &status_reg);

	return ((status_reg & SPI_FLASH_WEL_BIT) ? 1 : 0);
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Set_Write_Disable
 * PURPOSE:
 *
 ******************************************************************************/
static u32
Spi_Flash_Set_Write_Disable(u8 spi_flash_channel)
{
	u32 rx_data;
	u8 status_reg;

	// Wait until Flash is ready
	while (!Spi_Flash_Is_Flash_Ready(spi_flash_channel)) ;

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 1, SPI_FLASH_WRDI_OPCODE, &rx_data);

	Spi_Flash_Read_Status_Register(spi_flash_channel, &status_reg);

	return ((status_reg & SPI_FLASH_WEL_BIT) ? 0 : 1);
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Read_Identification
 * PURPOSE:
 *
 ******************************************************************************/
static void
Spi_Flash_Read_Identification(u8 spi_flash_channel, u8 * manufacture_id, u16 * device_id)
{
	u32 rx_data1, rx_data2, rx_data3;

	// Wait until Flash is ready
	while (!Spi_Flash_Is_Flash_Ready(spi_flash_channel)) ;

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, SPI_FLASH_RDID_OPCODE, &rx_data1);

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, 0xFF, &rx_data1);

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, 0xFF, &rx_data2);

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 1, 0xFF, &rx_data3);

	*manufacture_id = (u8) (rx_data1 & 0xFF);

	*device_id = (u16) ((rx_data2 & 0xFF) << 8) | (u16) (rx_data3 & 0xFF);
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Read_Data_Bytes
 * PURPOSE:
 *
 ******************************************************************************/
static void
Spi_Flash_Read_Data_Bytes(u8 spi_flash_channel, u32 address, u8 * read_buffer, u32 len)
{
	u32 rx_data;
	u32 ii;

	// Wait until Flash is ready
	while (!Spi_Flash_Is_Flash_Ready(spi_flash_channel)) ;

#if 1
	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, SPI_FLASH_FAST_READ_OPCODE, &rx_data);
#else
	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, SPI_FLASH_READ_OPCODE, &rx_data);
#endif

	/*
	 * Note the address is 24-Bit.
	 * The first byte addressed can be at any location, and the address is automatically
	 * incremented to the next higher address after each byte of the data is shifted-out.
	 * When the highest address is reached, the address counter rolls over to 000000h,
	 * allowing the read sequence to be continued indefinitely.
	 */
	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, (u32) ((address >> 16) & 0xFF), &rx_data);

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, (u32) ((address >> 8) & 0xFF), &rx_data);

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, (u32) ((address >> 0) & 0xFF), &rx_data);

#if 1
	/*
	 * Dummy Byte - 8bit, only on FAST_READ
	 */
	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, (u32) ((address >> 0) & 0xFF), &rx_data);
#endif

	/*
	 * Read "len" data bytes
	 */
	for (ii = 0; ii < len - 1; ii++) {
		Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, 0xFF, &rx_data);

		*read_buffer++ = (u8) (rx_data & 0xFF);
	}

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 1, 0xFF, &rx_data);

	*read_buffer = (u8) (rx_data & 0xFF);
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Sector_Erase
 * PURPOSE:
 *
 ******************************************************************************/
static u32
Spi_Flash_Sector_Erase(u8 spi_flash_channel, u32 sector_addr)
{
	struct spi_flash_info *flash_info = &spi_flash_bank[spi_flash_channel];
	u32 rx_data;

	// The specified address is beyond the maximum address range
	if (sector_addr > (flash_info->sectors * flash_info->sector_size))
		return 0;

	/*
	 * First, issue "Write Enable" instruction, and then issue "Sector Erase" instruction
	 * Note any address inside the Sector is a valid address of the Sector Erase instruction
	 */
	if (Spi_Flash_Set_Write_Enable(spi_flash_channel)) {
		Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, SPI_FLASH_SE_OPCODE, &rx_data);

		/*
		 * Note the sector address is 24-Bit
		 */
		Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, (u32) ((sector_addr >> 16) & 0xFF), &rx_data);

		Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, (u32) ((sector_addr >> 8) & 0xFF), &rx_data);

		Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 1, (u32) ((sector_addr >> 0) & 0xFF), &rx_data);

		return 1;
	} else {
		return 0;
	}
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Bulk_Erase
 * PURPOSE:
 *
 ******************************************************************************/
static u32
Spi_Flash_Bulk_Erase(u8 spi_flash_channel)
{
	u32 rx_data;
	u8 status_reg;

	/*
	 * First, issue "Write Enable" instruction, and then issue "Bulk Erase" instruction
	 * Note the Bulk Erase instruction is executed only if all Block Protect (BP2, BP2, 
	 * BP0) bits are 0. The Bulk Erase instruction is ignored if one or more sectors are
	 * protected.
	 */
	if (Spi_Flash_Set_Write_Enable(spi_flash_channel)) {
		Spi_Flash_Read_Status_Register(spi_flash_channel, &status_reg);

		if (status_reg & SPI_FLASH_BP012_BITS) {
			// Failed because one or more sectors are protected!!
			return 0;
		}

		Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 1, SPI_FLASH_BE_OPCODE, &rx_data);

		return 1;
	} else {
		return 0;
	}
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Page_Program_Data_Bytes
 * PURPOSE:
 *
 ******************************************************************************/
static u32
Spi_Flash_Page_Program_Data_Bytes(u8 spi_flash_channel, u32 address, u8 * write_buffer, u32 len)
{
	struct spi_flash_info *flash_info = &spi_flash_bank[spi_flash_channel];
	u32 rx_data;
	u32 ii;

	// This function does not support (len > SPI_FLASH_PAGE_SIZE)
	if (len > flash_info->page_size)
		return 0;

	// The specified address is beyond the maximum address range
	if ((address + len) > (flash_info->pages * flash_info->page_size))
	if ((address + len) > SPI_FLASH_PAGE_BASE_ADDR(SPI_FLASH_MAX_PAGE_NUM))
		return 0;

	// The specified address range will cross the page boundary
	if ((address / flash_info->page_size) != ((address + len - 1) / flash_info->page_size))
		return 0;

	/*
	 * First, issue "Write Enable" instruction, and then issue "Page Program" instruction
	 */
	if (!Spi_Flash_Set_Write_Enable(spi_flash_channel)) {
		return 0;
	}

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, SPI_FLASH_PP_OPCODE, &rx_data);

	/*
	 * Note the address is 24-Bit
	 * If the 8 least significant address bits (A7~A0) are not all zero, all transmitted
	 * data that goes beyond the end of the current page are programmed from the start
	 * address of the same page (from the address whose 8 least significant address bits 
	 * (A7~A0) are all zero.
	 * If more than 256 bytes are sent to the device, previously latched data are discarded
	 * and the last 256 data bytes are guaranteed to be programmed correctly within the
	 * same page.
	 * If less than 256 Data bytes are sent to the device, they are correctly programmed
	 * at the requested addresses without having any effects on the other bytes of the same
	 * page.
	 */
	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, (u32) ((address >> 16) & 0xFF), &rx_data);

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, (u32) ((address >> 8) & 0xFF), &rx_data);

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, (u32) ((address >> 0) & 0xFF), &rx_data);

	/*
	 * Write "len" data bytes
	 */
	for (ii = 0; ii < len - 1; ii++) {
		Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, (u32) * write_buffer++, &rx_data);
	}

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 1, (u32) * write_buffer, &rx_data);

	return 1;
}

void AT91F_SpiInit(void)
{
	Spi_Flash_Initialize(0);
}

int AT91F_DataflashProbe(int cs, AT91PS_DataflashDesc pDesc)
{
	u8 manufacturer_id;
	u16 device_id;

	Spi_Flash_Read_Identification(0, &manufacturer_id, &device_id);

	switch ((manufacturer_id << 16) | device_id) {
	case ST_M25P32:
		spi_flash_bank[0].sectors = ST_M25P32_SECTOR_NUM;
		spi_flash_bank[0].sector_size = SPI_FLASH_SECTOR_SIZE;
		spi_flash_bank[0].pages = ST_M25P32_PAGE_NUM;
		spi_flash_bank[0].page_size = SPI_FLASH_PAGE_SIZE;
		break;
	case ST_M25P64:
		spi_flash_bank[0].sectors = ST_M25P64_SECTOR_NUM;
		spi_flash_bank[0].sector_size = SPI_FLASH_SECTOR_SIZE;
		spi_flash_bank[0].pages = ST_M25P64_PAGE_NUM;
		spi_flash_bank[0].page_size = SPI_FLASH_PAGE_SIZE;
		break;
	case MX_25L32:
		spi_flash_bank[0].sectors = MX_25L32_SECTOR_NUM;
		spi_flash_bank[0].sector_size = SPI_FLASH_SECTOR_SIZE;
		spi_flash_bank[0].pages = MX_25L32_PAGE_NUM;
		spi_flash_bank[0].page_size = SPI_FLASH_PAGE_SIZE;
		break;
	case MX_25L64:
		spi_flash_bank[0].sectors = MX_25L64_SECTOR_NUM;
		spi_flash_bank[0].sector_size = SPI_FLASH_SECTOR_SIZE;
		spi_flash_bank[0].pages = MX_25L64_PAGE_NUM;
		spi_flash_bank[0].page_size = SPI_FLASH_PAGE_SIZE;
		break;
	case EN_25P32:
		spi_flash_bank[0].sectors = EN_25P32_SECTOR_NUM;
		spi_flash_bank[0].sector_size = SPI_FLASH_SECTOR_SIZE;
		spi_flash_bank[0].pages = EN_25P32_PAGE_NUM;
		spi_flash_bank[0].page_size = SPI_FLASH_PAGE_SIZE;
		break;
	default:
		break;
	}

	return ((manufacturer_id << 16) | device_id);
}

// addr: relative offset to flash base addr
// addr && size are already verified on upper layer
int AT91F_DataFlashRead(
	AT91PS_DataFlash pDataFlash,
	unsigned long addr,
	unsigned long size,
	char *buffer)
{
#if 1
	Spi_Flash_Read_Data_Bytes(0, addr, buffer, size);
#else
        DECLARE_GLOBAL_DATA_PTR;
	struct spi_flash_info *spi_flash_info = &spi_flash_bank[0];
	u32 read_size_left;
	u32 read_size;
	u32 read_size_total;

	read_size_left = size;
	read_size_total = 0;

	while (read_size_left) {
		if (read_size_left >= spi_flash_info->page_size) {
			read_size = spi_flash_info->page_size;
		} else {
			read_size = read_size_left;
		}
		Spi_Flash_Read_Data_Bytes(0, addr + read_size_total, buffer + read_size_total, read_size);
		read_size_left -= read_size;
		read_size_total += read_size;
		if (gd->have_console) {
			printf("\r0x%08x        ", read_size_total);
		}
	}
	if (gd->have_console) {
		printf("\n");
	}
#endif

	return DATAFLASH_OK;
}

// dest: relative offset to flash base addr
// dest && size are already verified on upper layer
AT91S_DataFlashStatus AT91F_DataFlashWrite(
	AT91PS_DataFlash pDataFlash,
	unsigned char *src,
	int dest,
	int size)
{
	struct spi_flash_info *spi_flash_info = &spi_flash_bank[0];
	u32 i;
	u32 start_sector = 0;
	u32 end_sector = 0;
	u32 prog_size_left;
	u32 prog_size;
	u32 prog_size_total;

	for (i = 0; i < spi_flash_info->sectors; i++) {
		if (dest >= (i * spi_flash_info->sector_size))
			continue;
		start_sector = i - 1;
		break;
	}
	for (i = start_sector; i < spi_flash_info->sectors; i++) {
		if ((dest + size) > (i * spi_flash_info->sector_size))
			continue;
		end_sector = i - 1;
		break;
	}

	printf("\n");
	for (i = start_sector; i <= end_sector; i++) {
		if (!Spi_Flash_Sector_Erase(0, i * spi_flash_info->sector_size)) {
			return DATAFLASH_ERROR;
		}
		printf("Serial Flash Sector %d Erase OK!\n", i);
	}

	prog_size_left = size;
	prog_size_total = 0;

	while (prog_size_left) {
		if (prog_size_left >= spi_flash_info->page_size) {
			prog_size = spi_flash_info->page_size;
		} else {
			prog_size = prog_size_left;
		}
		if (!Spi_Flash_Page_Program_Data_Bytes(0, dest + prog_size_total, src + prog_size_total, prog_size)) {
			return ERR_PROG_ERROR;
		}
		prog_size_left -= prog_size;
		prog_size_total += prog_size;
		printf("\r0x%08x        ", prog_size_total);
	}
	printf("\n");

	return DATAFLASH_OK;
}

//=============================================================================
flash_info_t flash_info[CFG_MAX_FLASH_BANKS];	/* info for FLASH chips */

static ulong flash_get_size(vu_long *addr, flash_info_t *info);

unsigned long flash_init(void)
{
	unsigned long size;
	int i;

	/* Init: no FLASHes known */
	for (i = 0; i < CFG_MAX_FLASH_BANKS; ++i)
		flash_info[i].flash_id = FLASH_UNKNOWN;

	size = flash_get_size((vu_long *)CFG_SPI_FLASH_BASE, &flash_info[0]);

	flash_info[0].size = size;

	return (size);
}

void flash_print_info(flash_info_t * info)
{
	printf("\n");
	return;
}

static ulong flash_get_size(vu_long *addr, flash_info_t *info)
{
	u8 manufacturer_id;
	u16 device_id;
	int i;
	ulong base = CFG_SPI_FLASH_BASE;

	Spi_Flash_Read_Identification(0, &manufacturer_id, &device_id);

	switch ((manufacturer_id << 16) | device_id) {
	case ST_M25P32:
		printf("Flash Manufacturer: ST\n");
		spi_flash_bank[0].sectors = ST_M25P32_SECTOR_NUM;
		spi_flash_bank[0].sector_size = SPI_FLASH_SECTOR_SIZE;
		spi_flash_bank[0].pages = ST_M25P32_PAGE_NUM;
		spi_flash_bank[0].page_size = SPI_FLASH_PAGE_SIZE;
		info->flash_id = FLASH_MAN_STM + ST_M25P32_DEVICE_ID;
		info->sector_count = 64;
		info->size = 0x400000;
		memset(info->protect, 0, 64);
		for (i = 0; i < info->sector_count; i++) {
			info->start[i] = base;
			base += 0x10000;
		}
		break;
	case ST_M25P64:
		printf("Flash Manufacturer: ST\n");
		spi_flash_bank[0].sectors = ST_M25P64_SECTOR_NUM;
		spi_flash_bank[0].sector_size = SPI_FLASH_SECTOR_SIZE;
		spi_flash_bank[0].pages = ST_M25P64_PAGE_NUM;
		spi_flash_bank[0].page_size = SPI_FLASH_PAGE_SIZE;
		info->flash_id = FLASH_MAN_STM + ST_M25P64_DEVICE_ID;
		info->sector_count = 128;
		info->size = 0x800000;
		memset(info->protect, 0, 128);
		for (i = 0; i < info->sector_count; i++) {
			info->start[i] = base;
			base += 0x10000;
		}
		break;
	case MX_25L32:
		printf("Flash Manufacturer: MX\n");
		spi_flash_bank[0].sectors = MX_25L32_SECTOR_NUM;
		spi_flash_bank[0].sector_size = SPI_FLASH_SECTOR_SIZE;
		spi_flash_bank[0].pages = MX_25L32_PAGE_NUM;
		spi_flash_bank[0].page_size = SPI_FLASH_PAGE_SIZE;
		info->flash_id = FLASH_MAN_MX + MX_25L32_DEVICE_ID;
		info->sector_count = 64;
		info->size = 0x400000;
		memset(info->protect, 0, 64);
		for (i = 0; i < info->sector_count; i++) {
			info->start[i] = base;
			base += 0x10000;
		}
		break;
	case MX_25L64:
		printf("Flash Manufacturer: MX\n");
		spi_flash_bank[0].sectors = MX_25L64_SECTOR_NUM;
		spi_flash_bank[0].sector_size = SPI_FLASH_SECTOR_SIZE;
		spi_flash_bank[0].pages = MX_25L64_PAGE_NUM;
		spi_flash_bank[0].page_size = SPI_FLASH_PAGE_SIZE;
		info->flash_id = FLASH_MAN_MX + MX_25L64_DEVICE_ID;
		info->sector_count = 128;
		info->size = 0x800000;
		memset(info->protect, 0, 128);
		for (i = 0; i < info->sector_count; i++) {
			info->start[i] = base;
			base += 0x10000;
		}
		break;
	case EN_25P32:
		printf("Flash Manufacturer: EON\n");
		spi_flash_bank[0].sectors = EN_25P32_SECTOR_NUM;
		spi_flash_bank[0].sector_size = SPI_FLASH_SECTOR_SIZE;
		spi_flash_bank[0].pages = EN_25P32_PAGE_NUM;
		spi_flash_bank[0].page_size = SPI_FLASH_PAGE_SIZE;
		info->flash_id = FLASH_MAN_EON + EN_25P32_DEVICE_ID;
		info->sector_count = 64;
		info->size = 0x400000;
		memset(info->protect, 0, 64);
		for (i = 0; i < info->sector_count; i++) {
			info->start[i] = base;
			base += 0x10000;
		}
		break;
	default:
		printf("Flash Manufacturer: Unknown(0x%lx)\n", manufacturer_id);
		info->flash_id = FLASH_UNKNOWN;
		info->sector_count = 0;
		info->size = 0;
		break;
	}

	return (info->size);
}

int flash_erase(flash_info_t *info, int s_first, int s_last)
{
	struct spi_flash_info *spi_flash_info = &spi_flash_bank[0];
	int flag, prot, sect;

	if (info->flash_id == FLASH_UNKNOWN) {
		printf("Can't erase unknown flash type %08lx - aborted\n", info->flash_id);
		return (ERR_UNKNOWN_FLASH_TYPE);
	}

	if ((s_first < 0) || (s_first > s_last)) {
		printf("- No sectors to erase\n");
		return (ERR_INVAL);
	}

	prot = 0;
	for (sect = s_first; sect <= s_last; ++sect) {
		if (info->protect[sect]) {
			prot++;
		}
	}

	if (prot) {
		printf("- Warning: %d protected sectors will not be erased\n", prot);
	} else {
		printf("\n");
	}

	/* Disable interrupts which might cause a timeout here */
	flag = disable_interrupts();

	/* Start erase on unprotected sectors */
	for (sect = s_first; sect <= s_last; sect++) {
		if (!Spi_Flash_Sector_Erase(0, sect * spi_flash_info->sector_size)) {
			return ERR_PROG_ERROR;
		} else {
			printf("Serial Flash Sector %d Erase OK!\n", sect);
		}
	}

	/* re-enable interrupts if necessary */
	if (flag)
		enable_interrupts();

	return (ERR_OK);
}

/*-----------------------------------------------------------------------
 * Copy memory to flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
int write_buff(flash_info_t * info, uchar * src, ulong addr, ulong cnt)
{
	struct spi_flash_info *spi_flash_info = &spi_flash_bank[0];
	u32 prog_size_left;
	u32 prog_size;
	u32 prog_size_total;
	u32 dest = addr - CFG_SPI_FLASH_BASE;

	prog_size_left = cnt;
	prog_size_total = 0;

	while (prog_size_left) {
		if (prog_size_left >= spi_flash_info->page_size) {
			prog_size = spi_flash_info->page_size;
		} else {
			prog_size = prog_size_left;
		}
		if (!Spi_Flash_Page_Program_Data_Bytes(0, dest + prog_size_total, src + prog_size_total, prog_size)) {
			return ERR_PROG_ERROR;
		}
		prog_size_left -= prog_size;
		prog_size_total += prog_size;
		printf("\r0x%08x        ", prog_size_total);
	}
	printf("\n");

	return (ERR_OK);
}
//=============================================================================

#endif // CONFIG_HAS_DATAFLASH
