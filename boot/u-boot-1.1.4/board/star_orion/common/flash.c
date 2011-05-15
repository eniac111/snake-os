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
#include <asm/arch/star_smc.h>


flash_info_t flash_info[CFG_MAX_FLASH_BANKS];	/* info for FLASH chips    */

static ulong flash_get_size(vu_long *addr, flash_info_t *info);
static int write_byte(flash_info_t *info, ulong dest, uchar data);
static void flash_get_offsets(ulong base, flash_info_t *info);

unsigned long flash_init(void)
{
	unsigned long size;
	int i;

	/* Init: no FLASHes known */
	for (i = 0; i < CFG_MAX_FLASH_BANKS; ++i)
		flash_info[i].flash_id = FLASH_UNKNOWN;

	size = flash_get_size((vu_long *)CFG_FLASH_BASE, &flash_info[0]);

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
		break;
	case FLASH_MAN_EON:
		printf("EON ");
		break;
	default:
		printf("Unknown Vendor ");
		break;
	}

	switch (info->flash_id & FLASH_TYPEMASK) {
	case FLASH_MXLV640BB:
		printf("MX29LV640BB (64Mbit)\n");
		break;
	case FLASH_MXLV640BT:
		printf("MX29LV640BT (64Mbit)\n");
		break;
	case FLASH_ENLV640H:
		printf("EN29LV640H (64Mbit)\n");
		break;
	default:
		printf("Unknown Chip Type\n");
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
	ushort mid;
	ushort did;
	vu_short *saddr = (vu_short *)addr;
	ulong base = (ulong)addr;

	/* Write auto select command: read Manufacturer ID */
	saddr[0x555] = 0xAA;
	saddr[0x2AA] = 0x55;
	saddr[0x555] = 0x90;

	mid = saddr[0];
	switch (mid) {
	case (MX_MANUFACT & 0xFFFF):
		printf("Flash Manufacturer: MXIC\n");
		info->flash_id = FLASH_MAN_MX;
		break;
	case (EON_MANUFACT & 0xFFFF):
		printf("Flash Manufacturer: EON\n");
		info->flash_id = FLASH_MAN_EON;
		break;
	default:
		printf("Flash Manufacturer: Unknown(0x%lx)\n", mid);
		info->flash_id = FLASH_UNKNOWN;
		info->sector_count = 0;
		info->size = 0;
		return (0); /* no or unknown flash  */
	}

	did = saddr[1];
	switch (did) {
	case (MX_ID_LV640BB & 0xFFFF):
		printf("Flash Device: MXLV640BB(8MB)\n");
		info->flash_id += FLASH_MXLV640BB;
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
	case (MX_ID_LV640BT & 0xFFFF):
		printf("Flash Device: MXLV640BT(8MB)\n");
		info->flash_id += FLASH_MXLV640BT;
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
	case (EON_ID_LV640H & 0xFFFF):
		printf("Flash Device: ENLV640H(8MB)\n");
		info->flash_id += FLASH_ENLV640H;
		info->sector_count = 128;
		info->size = 0x00800000;
		memset(info->protect, 0, 128);
		for (i = 0; i < info->sector_count; i++) {
			info->start[i] = base;
			base += 0x000010000;
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
		saddr = (volatile unsigned short *)(info->start[i]);
		info->protect[i] = saddr[2] & 1;
	}
#endif

	/* reset to read mode */
	saddr[0] = 0xF0; /* reset the bank */
	udelay_masked(10000);

	return (info->size);
}

int flash_erase(flash_info_t *info, int s_first, int s_last)
{
	vu_short *saddr = (vu_short *)(info->start[0]);
	vu_short *saddr_s;
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
			saddr_s = (vu_short *)(info->start[sect]);

			printf("Erasing sector %2d @ %08lX... ", sect, info->start[sect]);

			saddr[0x555] = 0xAA;
			saddr[0x2AA] = 0x55;
			saddr[0x555] = 0x80;
			saddr[0x555] = 0xAA;
			saddr[0x2AA] = 0x55;
			saddr_s[0] = 0x30;

			reset_timer_masked();

			rc = ERR_OK;
			do {
				u16 result;

				if (get_timer_masked() > CFG_FLASH_ERASE_TOUT) {
					rc = ERR_TIMOUT;
					break;
				}

				result = saddr_s[0];

				if (result & 0x8080) {
					break;
				}
				if (result & 0x2020) {
					rc = ERR_PROG_ERROR;
					break;
				}
			} while (!rc);

			saddr[0xAAA] = 0x0F;

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
	saddr = (vu_short *)info->start[0];
	saddr[0] = 0xF0; /* reset bank */
	udelay_masked(10000);

	return (ERR_OK);
}

/*-----------------------------------------------------------------------
 * Copy memory to flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
int write_buff(flash_info_t *info, uchar *src, ulong addr, ulong cnt)
{
	ulong cp, wp, data;
	ulong wb = 0;
	int l;
	int i, rc;

	printf("Writing data ");

	wp = (addr & ~3); /* get lower word aligned address */

	/*
	 * handle unaligned start bytes
	 */
	if ((l = addr - wp) != 0) {
		data = 0;
		for (i = 0, cp = wp; i < l; ++i, ++cp) {
			data = (data >> 8) | (*(uchar *)cp << 24);
		}
		for (; i < 4 && cnt > 0; ++i) {
			data = (data >> 8) | (*src++ << 24);
			--cnt;
			++wb;
			++cp;
		}
		for (; cnt == 0 && i < 4; ++i, ++cp) {
			data = (data >> 8) | (*(uchar *)cp << 24);
		}

		if ((rc = write_word(info, wp, data)) != 0) {
			return (rc);
		}
		wp += 4;
	}

	/*
	 * handle word aligned part
	 */
	while (cnt >= 4) {
		data = *((vu_long *)src);
		if ((rc = write_word(info, wp, data)) != 0) {
			return (rc);
		}
		src += 4;
		wp += 4;
		cnt -= 4;
		wb += 4;
		if ((((cnt +3) & ~(3)) & 0xFFF) == 0) {
			printf("\r0x%08x        ", wb);
		}
	}

	if (cnt == 0) {
		return (ERR_OK);
	}

	/*
	 * handle unaligned tail bytes
	 */
	data = 0;
	for (i = 0, cp = wp; i < 4 && cnt > 0; ++i, ++cp) {
		data = (data >> 8) | (*src++ << 24);
		--cnt;
		++wb;
	}
	for (; i < 4; ++i, ++cp) {
		data = (data >> 8) | (*(uchar *)cp << 24);
	}

	printf("\r0x%08x        ", wb);
	return write_word(info, wp, data);
}

/*-----------------------------------------------------------------------
 * Write a word to Flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
static int write_word(flash_info_t *info, ulong dest, ulong data)
{
	vu_short *saddr = (vu_short *)(info->start[0]);
	vu_short sdata;
	int flag;

	/* Check if Flash is (sufficiently) erased */
	if ((*((vu_long *)dest) & data) != data) {
		return (ERR_NOT_ERASED);
	}

	/* First write low 16 bits */
	sdata = (short)(data & 0xFFFF);

	/* Disable interrupts which might cause a timeout here */
	flag = disable_interrupts();

	saddr[0x555] = 0xAA;
	saddr[0x2AA] = 0x55;
	saddr[0x555] = 0xA0;

	*((vu_short *)dest) = sdata;

	/* re-enable interrupts if necessary */
	if (flag)
		enable_interrupts();

	/* data polling for D7 */
	reset_timer_masked();
	while ((*((vu_short *)dest) & 0x8080) != (sdata & 0x8080)) {
		if (get_timer_masked() > CFG_FLASH_WRITE_TOUT) {
			return (ERR_TIMOUT);
		}
	}

	/* Now write high 16 bits */
	sdata = (short)(data >> 16);

	/* Disable interrupts which might cause a timeout here */
	flag = disable_interrupts();

	saddr[0x555] = 0xAA;
	saddr[0x2AA] = 0x55;
	saddr[0x555] = 0xA0;

	*((vu_short *)dest + 1) = sdata;

	/* re-enable interrupts if necessary */
	if (flag)
		enable_interrupts();

	/* data polling for D7 */
	reset_timer_masked();
	while ((*((vu_short *)dest + 1) & 0x8080) != (sdata & 0x8080)) {
		if (get_timer_masked() > CFG_FLASH_WRITE_TOUT) {
			return (ERR_TIMOUT);
		}
	}

	return (ERR_OK);
}

