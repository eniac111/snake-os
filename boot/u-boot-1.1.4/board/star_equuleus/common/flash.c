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

flash_info_t flash_info[CFG_MAX_FLASH_BANKS];	/* info for FLASH chips */

static ulong flash_get_size(vu_long *addr, flash_info_t *info);
static int write_byte(flash_info_t *info, ulong dest, uchar data);

unsigned long flash_init(void)
{
	unsigned long size;
	int i;

	/* Init: no FLASHes known */
	for (i = 0; i < CFG_MAX_FLASH_BANKS; ++i)
		flash_info[i].flash_id = FLASH_UNKNOWN;

	size = flash_get_size((vu_long *)CFG_FLASH_BASE, &flash_info[0]);

#if 0
	/* monitor protection ON by default */
	flash_protect(FLAG_PROTECT_SET,
		CFG_FLASH_BASE, CFG_FLASH_BASE + monitor_flash_len - 1,
		&flash_info[0]);

	flash_protect(FLAG_PROTECT_SET,
		CFG_ENV_ADDR,
		CFG_ENV_ADDR + CFG_ENV_SIZE - 1,
		&flash_info[0]);

#ifdef CFG_ENV_ADDR_REDUND
	flash_protect(FLAG_PROTECT_SET,
		CFG_ENV_ADDR_REDUND,
		CFG_ENV_ADDR_REDUND + CFG_ENV_SIZE_REDUND - 1,
		&flash_info[0]);
#endif
#endif

	flash_info[0].size = size;

	return (size);
}

void flash_print_info(flash_info_t * info)
{
	int i;

	if (info->flash_id == FLASH_UNKNOWN) {
		printf("Missing or Unknown FLASH type\n");
		return;
	}

	switch (info->flash_id & FLASH_VENDMASK) {
	case FLASH_MAN_MX:
		printf("MXIC ");
		switch (info->flash_id & FLASH_TYPEMASK) {
		case FLASH_MXLV640BB:
			printf("MX29LV640BB (64Mbit)\n");
			break;
		case FLASH_MXLV640BT:
			printf("MX29LV640BT (64Mbit)\n");
			break;
		default:
			printf("Unknown Chip Type\n");
			break;
		}
		break;
	case FLASH_MAN_EON:
		printf("EON ");
		switch (info->flash_id & FLASH_TYPEMASK) {
		case FLASH_ENLV640B:
			printf("EN29LV640B (64Mbit)\n");
			break;
		case FLASH_ENLV640T:
			printf("EN29LV640T (64Mbit)\n");
			break;
		default:
			printf("Unknown Chip Type\n");
			break;
		}
		break;
	default:
		printf("Unknown Vendor ");
		break;
	}

	printf("  Size: %ld MB in %d Sectors\n", info->size >> 20, info->sector_count);

	printf("  Sector Start Addresses:");
	for (i = 0; i < info->sector_count; ++i) {
		if ((i % 5) == 0)
			printf("\n   ");
		printf(" %08lX%s", info->start[i], info->protect[i] ? " (RO)" : "     ");
	}
	printf("\n");
}

static ulong flash_get_size(vu_long *addr, flash_info_t *info)
{
	short i;
	uchar mid;
	uchar did;
	vu_char *caddr = (vu_char *)addr;
	ulong base = (ulong)addr;

	/* Write auto select command: read Manufacturer ID */
	caddr[0xAAA] = 0xAA;
	caddr[0x555] = 0x55;
	caddr[0xAAA] = 0x90;

	mid = caddr[0];
	switch (mid) {
	case (MX_MANUFACT & 0xFF):
		printf("Flash Manufacturer: MXIC\n");
		info->flash_id = FLASH_MAN_MX;
		break;
	case (EON_MANUFACT & 0xFF):
		printf("Flash Manufacturer: EON\n");
		info->flash_id = FLASH_MAN_EON;
		break;
	default:
		printf("Flash Manufacturer: Unknown(0x%lx)\n", mid);
		info->flash_id = FLASH_UNKNOWN;
		info->sector_count = 0;
		info->size = 0;
		return (0);				/* no or unknown flash  */
	}

	did = caddr[2];					/* device ID */
	switch (did) {
	case (MX_ID_LV640BB & 0xFF): /*EON_ID_LV640B and MX_ID_LV640BB are 0x22CB22CB*/
		if (FLASH_MAN_MX == info->flash_id) {
			printf("Flash Device: MXLV640BB(8MB)\n");
			info->flash_id += FLASH_MXLV640BB;
		} else if (FLASH_MAN_EON == info->flash_id) { 
			printf("Flash Device: EN29LV640B(8MB)\n");
			info->flash_id += FLASH_ENLV640B;
		}
		info->sector_count = 135;
		info->size = 0x00800000;
		memset(info->protect, 0, 135);
		for (i = 0; i < 8; i++) {
			info->start[i] = base;
			base += 0x00002000;
		}
		for (; i < info->sector_count; i++) {
			info->start[i] = base;
			base += 0x00010000;
		}
		break;

	case (MX_ID_LV640BT & 0xFF): /*EON_ID_LV640T and MX_ID_LV640BT are 0x22C922C9 */
		if (FLASH_MAN_MX == info->flash_id) {
			printf("Flash Device: MXLV640BT(8MB)\n");
			info->flash_id += FLASH_MXLV640BT;
		} else if (FLASH_MAN_EON == info->flash_id) { 
			printf("Flash Device: EN29LV640T(8MB)\n");
			info->flash_id += FLASH_ENLV640T;
		}
		info->sector_count = 135;
		info->size = 0x00800000;
		memset(info->protect, 0, 135);
		for (i = 0; i < (info->sector_count - 8); i++) {
			info->start[i] = base;
			base += 0x00010000;
		}
		for (;i < (info->sector_count); i++) {
			info->start[i] = base;
			base += 0x00002000;
		}
		break;

	default:
		printf("Flash Device: Unknown(0x%lx)\n", did);
		info->flash_id = FLASH_UNKNOWN;
		return (0);				/* => no or unknown flash */
		break;
	}

#if 0
	/* check for protected sectors */
	for (i = 0; i < info->sector_count; i++) {
		/* read sector protection: D0 = 1 if protected */
		caddr = (volatile unsigned char *)(info->start[i]);
		info->protect[i] = caddr[2] & 1;
	}
#endif

	/* reset to read mode */
	caddr[0] = 0xF0; /* reset bank */
	udelay_masked(10000);

	return (info->size);
}

int flash_erase(flash_info_t *info, int s_first, int s_last)
{
	vu_char *caddr = (vu_char *)(info->start[0]);
	vu_char *caddr_s;
	int flag, prot, sect;
	int rc;

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
		if (info->protect[sect] == 0) {	/* not protected */
			caddr_s = (vu_char *)(info->start[sect]);

			printf("Erasing sector %2d @ %08lX... ", sect, info->start[sect]);

			caddr[0xAAA] = 0xAA;
			caddr[0x555] = 0x55;
			caddr[0xAAA] = 0x80;
			caddr[0xAAA] = 0xAA;
			caddr[0x555] = 0x55;
			caddr_s[0] = 0x30;

			reset_timer_masked();

			rc = ERR_OK;
			do {
				u8 result;

				if (get_timer_masked() > CFG_FLASH_ERASE_TOUT) {
					rc = ERR_TIMOUT;
					break;
				}

				result = caddr_s[0];

				if (result & 0x80) {
					break;
				}
				if (result & 0x20) {
					rc = ERR_PROG_ERROR;
					break;
				}
			} while (!rc);

			caddr[0xAAA] = 0x0F;

			if (rc == ERR_OK) {
				printf("OK\n");
			} else {
				printf("Failed");
				return (rc);
			}
		}
	}

	/* re-enable interrupts if necessary */
	if (flag)
		enable_interrupts();

	/* reset to read mode */
	caddr = (vu_char *)info->start[0];
	caddr[0] = 0xF0; /* reset bank */
	udelay_masked(10000);

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
	int rc;
	unsigned long wb = 0;

	printf("Writing data ");
	while (cnt > 0) {
		if ((rc = write_byte(info, addr++, *src++)) != ERR_OK) {
			return (rc);
		}
		--cnt;
		++wb;
		if ((cnt & 0x1fff) == 0) {
			printf("\r0x%08x        ", wb);
		}
	}

	return (ERR_OK);
}

/*-----------------------------------------------------------------------
 * Write a byte to Flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
static int write_byte(flash_info_t *info, ulong dest, uchar data)
{
	vu_char *caddr = (vu_char *)(info->start[0]);
	int flag;

	/* Check if Flash is (sufficiently) erased */
	if ((*((vu_char *) dest) & data) != data) {
		return (ERR_NOT_ERASED);
	}

	/* Disable interrupts which might cause a timeout here */
	flag = disable_interrupts();

	caddr[0xAAA] = 0xAA;
	caddr[0x555] = 0x55;
	caddr[0xAAA] = 0xA0;

	*((vu_char *)dest) = data;

	/* re-enable interrupts if necessary */
	if (flag)
		enable_interrupts();

	/* data polling for D7 */
	reset_timer_masked();
	while ((*((vu_char *)dest) & 0x80) != (data & 0x80)) {
		if (get_timer_masked() > CFG_FLASH_WRITE_TOUT) {
			return (ERR_TIMOUT);
		}
	}

	return (ERR_OK);
}

