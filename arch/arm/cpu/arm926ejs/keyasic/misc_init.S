/*
 * Misc board setup code for KA2000 SoC boards.
 *
 * (C) Copyright 2010
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

.globl cpu_setup
cpu_setup:
	mov     r0, #0
        mcr     p15, 0, r0, c7, c7, 0           /* invalidate I,D caches on v4 */
        mcr     p15, 0, r0, c7, c10, 4          /* drain write buffer on v4 */
        mcr     p15, 0, r0, c8, c7, 0           /* invalidate I,D TLBs on v4, for MMU */
                                                                                                                                                                                   
        /*mov     r0, r0, #(0x03 << 30)         // way3
        orr     r0, r0, #(0x07 << 5)            // set=7
        mcr     p15, 0, r0, c7, c14, 1          // write tag for d cache
	*/
                                                                                                                                                                                   
        mov     r0, #(0x40 << 8)
        orr     r0, r0, #(0xf0 << 16)           /* 0xf0 << 16 */
        orr     r0, r0, #(0x01 << 24)           /* 0x01 << 24 */
        mcr     p15, 0, r0, c2, c0, 0           /* write TTBR */
                                                                                                                                                                                   
        mov     r0, #0x1f
        mcr     p15, 0, r0, c3, c0, 0           /* write domain */
        MOV     pc,lr

.globl test_clean_dcache
test_clean_dcache:
	mrc 	p15, 0, r15, c7, c10, 3
        bne	test_clean_dcache
        mov     pc, lr

.globl goto_offset
goto_offset:
        mov     r0, #0x40000
        mov     pc, r0

