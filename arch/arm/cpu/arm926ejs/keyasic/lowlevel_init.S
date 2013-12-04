/*
 * Low-level board setup code for KA2000 SoC boards.
 * Board specific setup info
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

#include <config.h>

.globl	lowlevel_init
lowlevel_init:

	/*------------------------------------------------------*
	 * Mask all IRQs 					*
	 *------------------------------------------------------*/
	/*------------------------------------------------------*
	 * PLL Initialization					*
	 *------------------------------------------------------*/
	/*------------------------------------------------------*
	 * Issue Soft Reset 					*
	 *------------------------------------------------------*/
	/*------------------------------------------------------*
	 * Turn Controller Clocks On				*
	 *------------------------------------------------------*/
	/*------------------------------------------------------*
	 * System PLL Initialization				*
	 *------------------------------------------------------*/

	/*nop
	nop */


	/*
	 * Call board-specific lowlevel init.
	 * That MUST be present and THAT returns
	 * back to arch calling code with "mov pc, lr."
	 */
	b	ka_board_init

.ltorg

DUMMY_VAL:
	.word	0xa55aa55a
