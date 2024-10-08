@ vim: set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab syntax=armasm:
/***********************************************************************
 * Copyright (c) 2014 Wladimir J. van der Laan                         *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/
/*
ARM implementation of field_10x26 inner loops.

Note:

- To avoid unnecessary loads and make use of available registers, two
  'passes' have every time been interleaved, with the odd passes accumulating c' and d' 
  which will be added to c and d respectively in the even passes

*/

	.syntax unified
	@ eabi attributes - see readelf -A
	.eabi_attribute 24, 1 @ Tag_ABI_align_needed = 8-byte
	.eabi_attribute 25, 1 @ Tag_ABI_align_preserved = 8-byte, except leaf SP
	.text

	@ Field constants
	.set field_R0, 0x3d10
	.set field_R1, 0x400
	.set field_not_M, 0xfc000000	@ ~M = ~0x3ffffff

	.align	2
	.global secp256k1_fe_mul_inner
	.type	secp256k1_fe_mul_inner, %function
	.hidden secp256k1_fe_mul_inner
	@ Arguments:
	@  r0  r      Restrict: can overlap with a, not with b
	@  r1  a
	@  r2  b
	@ Stack (total 4+10*4 = 44)
	@  sp + #0        saved 'r' pointer
	@  sp + #4 + 4*X  t0,t1,t2,t3,t4,t5,t6,t7,u8,t9
secp256k1_fe_mul_inner:
	stmfd	sp!, {r4, r5, r6, r7, r8, r9, r10, r11, r14}
	sub	sp, sp, #48			@ frame=44 + alignment
	str     r0, [sp, #0]			@ save result address, we need it only at the end

	/******************************************
	 * Main computation code.
	 ******************************************

	Allocation:
	    r0,r14,r7,r8   scratch
	    r1       a (pointer)
	    r2       b (pointer)
	    r3:r4    c
	    r5:r6    d
	    r11:r12  c'
	    r9:r10   d'

	Note: do not write to r[] here, it may overlap with a[]
	*/

	/* A - interleaved with B */
	ldr	r7, [r1, #0*4]			@ a[0]
	ldr	r8, [r2, #9*4]			@ b[9]
	ldr	r0, [r1, #1*4]			@ a[1]
	umull	r5, r6, r7, r8			@ d = a[0] * b[9]
	ldr	r14, [r2, #8*4]			@ b[8]
	umull	r9, r10, r0, r8			@ d' = a[1] * b[9]
	ldr	r7, [r1, #2*4]			@ a[2]
	umlal	r5, r6, r0, r14			@ d += a[1] * b[8]
	ldr	r8, [r2, #7*4] 			@ b[7]
	umlal	r9, r10, r7, r14		@ d' += a[2] * b[8]
	ldr	r0, [r1, #3*4]   		@ a[3]
	umlal	r5, r6, r7, r8   		@ d += a[2] * b[7]
	ldr	r14, [r2, #6*4]   		@ b[6]
	umlal	r9, r10, r0, r8  		@ d' += a[3] * b[7]
	ldr	r7, [r1, #4*4]   		@ a[4]
	umlal	r5, r6, r0, r14   		@ d += a[3] * b[6]
	ldr	r8, [r2, #5*4]   		@ b[5]
	umlal	r9, r10, r7, r14  		@ d' += a[4] * b[6]
	ldr	r0, [r1, #5*4]   		@ a[5]
	umlal	r5, r6, r7, r8   		@ d += a[4] * b[5]
	ldr	r14, [r2, #4*4]   		@ b[4]
	umlal	r9, r10, r0, r8  		@ d' += a[5] * b[5]
	ldr	r7, [r1, #6*4]   		@ a[6]
	umlal	r5, r6, r0, r14   		@ d += a[5] * b[4]
	ldr	r8, [r2, #3*4]   		@ b[3]
	umlal	r9, r10, r7, r14  		@ d' += a[6] * b[4]
	ldr	r0, [r1, #7*4]   		@ a[7]
	umlal	r5, r6, r7, r8   		@ d += a[6] * b[3]
	ldr	r14, [r2, #2*4]   		@ b[2]
	umlal	r9, r10, r0, r8  		@ d' += a[7] * b[3]
