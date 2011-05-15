/*
 * Memory Setup stuff - taken from blob memsetup.S
 *
 * Copyright (C) 1999 2000 2001 Erik Mouw (J.A.K.Mouw@its.tudelft.nl) and
 *                     Jan-Derk Bakker (J.D.Bakker@its.tudelft.nl)
 *
 * Modified for the Samsung SMDK2410 by
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
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


#include <config.h>
#include <version.h>

	.globl lowlevel_init
lowlevel_init:
	//mkl070509: shut down the watchdog, in case the HW reset is enabled.
	ldr     r0, =0x7A00000C
	mov     r1, #0x0
	str     r1, [r0]

	// disable PLL power-down, i.e., power-on all PLL
	ldr	r0, =0x77000010
	mov	r1, #0x0
	str	r1, [r0]

	// disable AHB and APB clock mask
	ldr	r0, =0x77000000
	mov	r1, #0x0
	str	r1, [r0]

	// exit power on reset state
	ldr	r0, =0x77000004
	mov	r1, #0xFFFFFFFF
	str	r1, [r0]

        // mask all IRQs by setting interrupt controller
	ldr	r0, =0x7D000004
	mov	r1, #0xFFFFFFFF
	str	r1, [r0]

	/*
	 * set SMC config register as follows:
	 * bank enable, bank size = 8MByte, bank data width = 16Bit, disable write protect
	 */
	ldr	r0, =0x30000000
	ldr	r1, =0x10000031
	str	r1, [r0]

#if 0
	// Initialize UART, 38400, 8N1
	// UART PSR
	ldr	r0, =0x7800000C
	mov	r1, #0x80
	str	r1, [r0]
	ldr	r0, =0x78000008
	mov	r1, #0x2
	str	r1, [r0]
	ldr	r0, =0x7800000C
	mov	r1, #0x0
	str	r1, [r0]

	// UART DLM
	ldr	r0, =0x7800000C
	mov	r1, #0x80
	str	r1, [r0]
	ldr	r0, =0x78000004
	mov	r1, #0x0
	str	r1, [r0]
	ldr	r0, =0x7800000C
	mov	r1, #0x0
	str	r1, [r0]

	// UART DLL
	ldr	r0, =0x7800000C
	mov	r1, #0x80
	str	r1, [r0]
	ldr	r0, =0x78000000
	mov	r1, #0x18
	str	r1, [r0]
	ldr	r0, =0x7800000C
	mov	r1, #0x0
	str	r1, [r0]

	// UART LCR
	ldr	r0, =0x7800000C
	mov	r1, #0x3
	str	r1, [r0]

	// UART FCR
	ldr	r0, =0x78000008 
	mov	r1, #0x81
	str	r1, [r0]
#endif

	/* Disable DDRC bank, and configure DDRC's bank configuration */
	ldr	r0, =0x4000000C
#if (DDR_RAM_DATA_BUS_WIDTH == DDR_RAM_DATA_BUS_WIDTH_32)
  #if (DDR_RAM_SIZE == DDR_RAM_SIZE_512MBIT)
	ldr	r1, =0x000000B1		  /* DDRC: 32-bit, DDR SDRAM: 512Mb, x16 device */
  #elif (DDR_RAM_SIZE == DDR_RAM_SIZE_256MBIT)
    #if 1
	ldr	r1, =0x000000A1		  /* DDRC: 32-bit, DDR SDRAM: 256Mb, x16 device */
    #else /* for testing only */
	ldr	r1, =0x00000061		  /* DDRC: 32-bit, DDR SDRAM: 256Mb, x8 device */
    #endif
  #elif (DDR_RAM_SIZE == DDR_RAM_SIZE_128MBIT)
	ldr	r1, =0x00000091		  /* DDRC: 32-bit, DDR SDRAM: 128Mb, x16 device */
  #else
	ldr	r1, =0x00000081		  /* DDRC: 32-bit, DDR SDRAM: 64Mb, x16 device */
  #endif
#else	/* for #if (DDRC_DATA_BUS_WIDTH == DDRC_DATA_BUS_WIDTH_16) */
  #if (DDR_RAM_SIZE == DDR_RAM_SIZE_512MBIT)
	ldr	r1, =0x000000B0		  /* DDRC: 16-bit, DDR SDRAM: 512Mb, x16 device */
  #elif (DDR_RAM_SIZE == DDR_RAM_SIZE_256MBIT)
    #if 1
	ldr	r1, =0x000000A0		  /* DDRC: 16-bit, DDR SDRAM: 256Mb, x16 device */
    #else /* for testing only */
	ldr	r1, =0x00000060		  /* DDRC: 16-bit, DDR SDRAM: 256Mb, x8 device */
    #endif
  #elif (DDR_RAM_SIZE == DDR_RAM_SIZE_128MBIT)
	ldr	r1, =0x00000090		  /* DDRC: 16-bit, DDR SDRAM: 128Mb, x16 device */
  #else
	ldr	r1, =0x00000080		  /* DDRC: 16-bit, DDR SDRAM: 64Mb, x16 device */
  #endif
#endif	/* end of #if (DDRC_DATA_BUS_WIDTH == DDRC_DATA_BUS_WIDTH_32) */
	str	r1, [r0]

	/* configure DDRC timing parameter 0 register */
	ldr	r0, =0x40000000
	ldr	r1, =0x009EEA88
	str	r1, [r0]

	/* configure DDRC timing parameter 1 register */
	ldr	r0, =0x40000004
#if (DDR_RAM_SIZE == DDR_RAM_SIZE_512MBIT)
	ldr	r1, =0x000081E2
#elif (DDR_RAM_SIZE == DDR_RAM_SIZE_256MBIT)
	ldr	r1, =0x000083C5
#elif (DDR_RAM_SIZE == DDR_RAM_SIZE_128MBIT)
	ldr	r1, =0x0000878A
#elif (DDR_RAM_SIZE == DDR_RAM_SIZE_64MBIT)
	ldr	r1, =0x00008F14
#endif
	str	r1, [r0]

	/* configure DDRC configuration register;Init Seq */
	ldr	r0, =0x40000008
	ldr	r1, =0x00000001
	str	r1, [r0]

	/* Waiting for Init done */        
wait:     
	ldr	r0, =0x40000008
	ldr	r1, [r0]
	bic	r1, r1, #0xfffffffd
	cmp	r1, #0x00000002	/* icmp bit */
	bne	wait

	/* configure DDRC DQs output delay */
	ldr	r0, =0x40000014
	ldr	r1, =0x00074444
	str	r1, [r0]

	/* enable DDRC bank configuration */
	ldr	r0, =0x4000000C
	ldr	r1, [r0]
	orr	r1, r1, #0x00000100
	str	r1, [r0]

	// return to the caller
	mov	pc, lr

