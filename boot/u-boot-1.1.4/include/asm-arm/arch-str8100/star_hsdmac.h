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

#ifndef	_STAR_HSDMAC_H_
#define	_STAR_HSDMAC_H_


#include "star_sys_memory_map.h"


#if defined(__UBOOT__)
#define	HSDMAC_MEM_MAP_VALUE(reg_offset)	(*((u32 volatile *)(SYSPA_MISC_BASE_ADDR + reg_offset)))
#elif defined(__LINUX__)
#define	HSDMAC_MEM_MAP_VALUE(reg_offset)	(*((u32 volatile *)(SYSVA_MISC_BASE_ADDR + reg_offset)))
#else
#error "NO SYSTEM DEFINED"
#endif


/*
 * define access macros
 */
#define	HSDMAC_CONTROL_STATUS_REG		HSDMAC_MEM_MAP_VALUE(0x040)

#define	HSDMAC_MASTER0_ADDR_REG			HSDMAC_MEM_MAP_VALUE(0x050)

#define	HSDMAC_MASTER1_ADDR_REG			HSDMAC_MEM_MAP_VALUE(0x054)

#define	HSDMAC_LLP_REG				HSDMAC_MEM_MAP_VALUE(0x058)

#define	HSDMAC_TOT_SIZE_REG			HSDMAC_MEM_MAP_VALUE(0x05C)


#define	HAL_GET_HSDMAC_LLP_COUNTER		((HSDMAC_CONTROL_STATUS_REG >> 8) & 0xF)

#define	HAL_HSDMAC_ENABLE()			((HSDMAC_CONTROL_STATUS_REG) |= (0x1))

#define	HAL_HSDMAC_DISABLE()			((HSDMAC_CONTROL_STATUS_REG) &= ~(0x1))


#define HSDMAC_MASTER0_TO_MASTER1		0
#define HSDMAC_MASTER1_TO_MASTER0		1

#define HSDMAC_RESPONSE_OK			0
#define HSDMAC_RESPONSE_ERR			-1

#define MAX_HSDMA_VEC 				32

#define MAX_HSDMA_XFER_SIZE			(0xFFF << 2)

struct hsdma_xfer;
typedef struct hsdma_xfer hsdma_xfer_t;
typedef void (*hsdma_end_io_t)(hsdma_xfer_t *hsdma_xfer, int err);
typedef struct
{
	u8	data_direction;
	u32	src_addr; // virtual
	u32	dst_addr; // virtual
	u32	size; // bytes
} __attribute__((packed)) hsdma_vec_t;

struct hsdma_xfer
{
	u8			nr_vec;
	hsdma_vec_t		vec[MAX_HSDMA_VEC];
	hsdma_end_io_t		hsdma_end_io;
	void			*private;
};

/*
 * HSDMAC LLP Descriptor object
 */
typedef struct
{
	u32	src_addr; // physical
	u32	dst_addr; // physical
	u32	llp;
	u32	tot_size	: 16;//b15-b0
	u32	reserved0	: 12;//b27-b16
	u32	tc_mask		: 1; //b28
	u32	data_direction	: 1; //b29
	u32	reserved1	: 2; //b31-30
} __attribute__((packed)) hsdma_llp_descr_t;


#endif	// end of #ifndef _STAR_HSDMAC_H_
