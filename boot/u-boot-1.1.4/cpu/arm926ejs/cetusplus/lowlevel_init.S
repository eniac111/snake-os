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

#if 1	
	// Initialize DDR Controller
	// DDRC: 16-bit mode, SDRAM Module: 256Mb * 16, Little-Endian
	ldr	r0, =0x72000000
	ldr	r1, =0x02000021
	str	r1, [r0]
#endif	

#if 0
	ldr	r0, =0x72000000
	ldr	r1, =0x03000021
	str	r1, [r0]
#endif

	// Configuration of Parameter Configure Register
	ldr	r0, =0x72000004
	ldr	r1, =0x00000021		/* Disable DLL of DDR device */
	str	r1, [r0]

	// Configuration of Timing Parameter 0 Register
	ldr	r0, =0x72000010
	ldr	r1, =0x11148221
	str	r1, [r0]

	// Configuration of Timing Parameter 1 Register
	ldr	r0, =0x72000014
	ldr	r1, =0x240F10C8
	str	r1, [r0]

	// Configuration of Timing Parameter 2 Register
	ldr	r0, =0x72000018
	ldr	r1, =0x00070090
	str	r1, [r0]

	// Configuration of Power-Up Control Register
	ldr	r0, =0x72000008
	ldr	r1, =0x00000001
	str	r1, [r0]

	// Wait for InitCmp bit to become 1
wait:
	ldr	r1, [r0]
	cmp	r1, #0x2
	bne	wait

	// Mask all interrupt sources
	ldr	r0, =0xFFFFF008
	ldr	r1, =0xFFFFFFFF
	str	r1, [r0]

	// return to the caller
	mov	pc, lr

