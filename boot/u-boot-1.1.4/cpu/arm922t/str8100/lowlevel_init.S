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
	ldr	r0, =0x7A00000C
	mov	r1, #0x0
	str	r1, [r0]

#ifdef CONFIG_SPI_FLASH_SUPPORT
	// enable SPI high speed read for booting
	ldr	r0, =0x71000040
	ldr	r1, [r0]
	mov	r2, #0x40000000
	orr	r1, r1, r2
	str	r1, [r0]

	// change SPI clock rate
	ldr	r0, =0x71000048
	mov	r1, #0x1
	str	r1, [r0]
#endif

	// set SMC bank0 setting
	ldr	r0, =0x73000000
	// write protect off, bank enable, bus width 8
	ldr	r1, =0x00000002
	str	r1, [r0]

	// set SMC bank0 timing
	ldr	r0, =0x73000004
	ldr	r1, =0x330F0F0F
	str	r1, [r0]

	// disable PLL power-down, i.e., power-on all PLL
	ldr	r0, =0x77000010
	mov	r1, #0x0
	str	r1, [r0]

	// disable clock gating
	ldr	r0, =0x77000000
	mov	r1, #0xFFFFFFFF
	str	r1, [r0]

	ldr	r0, =0x77000004
	mov	r1, #0xFFFFFFFF
	str	r1, [r0]

	// De-assert all of RESETn, i.e., exit reset state
	ldr	r0, =0x77000008
	ldr	r1, =0xFFFFFFFF
	str	r1, [r0]

#ifdef CONFIG_USE_DDR
	// for Str8181 not using both SDR and MII interface simultaneously
	// Set pd_25 = 0, and sel_sdr = 0
	ldr	r0, =0x77000024
	ldr	r1, =0x00130723
	str	r1, [r0]

	// Configuration of Memory Interface Configure Register 
	ldr	r0, =0x72000000
#if (RAM_SIZE == RAM_SIZE_512MBIT)
	// DDRC: 16-bit mode, SDRAM Module: 512Mb (64MB), Little-Endian
	ldr	r1, =0x02000022
#elif (RAM_SIZE == RAM_SIZE_256MBIT)
	// DDRC: 16-bit mode, SDRAM Module: 256Mb (32MB), Little-Endian
	ldr	r1, =0x02000021
#elif (RAM_SIZE == RAM_SIZE_128MBIT)
	// DDRC: 16-bit mode, SDRAM Module: 128Mb (16MB), Little-Endian
	ldr	r1, =0x02000020

#endif
	str	r1, [r0]

	// Configuration of Parameter Configure Register
	ldr	r0, =0x72000004
#ifdef CONFIG_USE_DDR_ON_FPGA
	ldr	r1, =0x00000020
#else
	ldr	r1, =0x00000020
#endif
	str	r1, [r0]

	// Configuration of Timing Parameter 0 Register
	ldr	r0, =0x72000010
	ldr	r1, =0x32292A62
	str	r1, [r0]

	// Configuration of Timing Parameter 1 Register
	ldr	r0, =0x72000014
	ldr	r1, =0x140F10C8
	str	r1, [r0]

	// Configuration of Timing Parameter 2 Register
	ldr	r0, =0x72000018
#ifdef CONFIG_USE_DDR_ON_FPGA
	ldr	r1, =0x00080078
#else
	ldr	r1, =0x00070029
#endif
	str	r1, [r0]

	// Configuration of Power-Up Control Register
	ldr	r0, =0x72000008
	ldr	r1, =0x00000001
	str	r1, [r0]

	// wait for InitCmp bit to become 1
wait:
	ldr	r1, [r0]
	cmp	r1, #0x2
	bne	wait

	// Configuration of Preread Timeout Disable Register	
	ldr	r0, =0x7200001C
	ldr	r1, =0x00000080
	str	r1, [r0]

	// Configuration of Preread Enable Register
	ldr	r0, =0x72000020
	ldr	r1, =0x000000FF
	str	r1, [r0]

	// Configuration of DQS Input Delay Control Register
	ldr	r0, =0x72000034
	ldr	r1, =0x00330033
	str	r1, [r0]

#if 0
	// Configuration of DDQ Output Delay Control Register
	ldr	r0, =0x72000030
	ldr	r1, =0x00000044
	str	r1, [r0]

	// Configuration of DQS Input Delay Control Register
	ldr	r0, =0x72000034
	ldr	r1, =0x04040404
	str	r1, [r0]

	// Configuration of HCLK Delay Control Register
	ldr	r0, =0x72000038
	ldr	r1, =0x07
	str	r1, [r0]
#endif
#else
	// for Str8133 using SDR and MII interface simultaneously
	// Set pd_25 = 1, and sel_sdr = 1
	ldr	r0, =0x77000024
	ldr	r1, =0x00133723
	str	r1, [r0]

	// Configuration of Memory Interface Configure Register 
	// SDRC: 16-bit mode, SDRAM Module: 256Mb * 16, Little-Endian
	ldr	r0, =0x72000000

#if (RAM_SIZE == RAM_SIZE_512MBIT)
	ldr	r1, =0x00000022
#elif (RAM_SIZE == RAM_SIZE_256MBIT)
	ldr	r1, =0x00000021
#elif (RAM_SIZE == RAM_SIZE_128MBIT)
	ldr	r1, =0x00000020
#elif (RAM_SIZE == RAM_SIZE_64MBIT)
	ldr	r1, =0x00000026
#elif (RAM_SIZE == RAM_SIZE_16MBIT)
	ldr	r1, =0x00000024
#endif

	str	r1, [r0]

	// Configuration of Parameter Configure Register
	ldr	r0, =0x72000004
	ldr	r1, =0x00000020
	str	r1, [r0]

	// Configuration of Timing Parameter 0 Register
	ldr	r0, =0x72000010
	ldr	r1, =0x32292A62
	str	r1, [r0]

	// Configuration of Timing Parameter 1 Register
	ldr	r0, =0x72000014
	ldr	r1, =0x140F09C8
	str	r1, [r0]

	// Configuration of Timing Parameter 2 Register
#ifdef CONFIG_USE_SDR_ON_FPGA
	ldr	r0, =0x72000018
	ldr	r1, =0x00080078
	str	r1, [r0]
#else
	ldr	r0, =0x72000018
	ldr	r1, =0x00070029
	str	r1, [r0]
#endif

	// Configuration of Power-Up Control Register
	ldr	r0, =0x72000008
	ldr	r1, =0x00000001
	str	r1, [r0]

	// wait for InitCmp bit to become 1
wait:
	ldr	r1, [r0]
	cmp	r1, #0x2
	bne	wait

	// Configuration of Preread Timeout Disable Register	
	ldr	r0, =0x7200001C
	ldr	r1, =0x00000080
	str	r1, [r0]

	// Configuration of Preread Enable Register
	ldr	r0, =0x72000020
	ldr	r1, =0x000000FF
	str	r1, [r0]
#endif

	// return to the caller
	mov	pc, lr

