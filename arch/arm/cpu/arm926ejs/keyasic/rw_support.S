/************************************************
 * This confidential and proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 *   (C) COPYRIGHT 2001 ARM Limited
 *       ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 ************************************************/

/*
 * (C) Copyright 2010
 *
 * Read / Write support functions
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

/*   AREA rw_support, CODE, READONLY ;READONLY is default for CODE */

/*----------------------------------------------------------------
 word_write(int addr, int data)
 writes a word to an address
 r0 contains addr
 r1 contains data
 -----------------------------------------------------------------*/
.globl word_write
word_write:
    stmfd SP!,{lr}  /* save return address to stack */
    str r1,[r0,#0]  /* store data */
    ldmfd SP!,{pc}  /* return */

/*-----------------------------------------------------------------
 word_read(int addr)
 reads a word from an address
 r0 contains addr
 -----------------------------------------------------------------*/
.globl word_read
word_read:
    STMFD SP!,{r1,lr}  /* save return address and r1 to stack */
    LDR r1,[r0,#0]     /* load temp with data */
    MOV r0, r1         /* copy temp to r0 for return value */
    LDMFD SP!,{r1,pc}  /* return */
 
/*-----------------------------------------------------------------
 hword_write(int addr, int data)
 writes a half word to an address
 r0 contains addr
 r1 contains data
 -----------------------------------------------------------------*/
.globl hword_write
hword_write:
    STMFD SP!,{lr}  /* save return address to stack */
    STRH r1,[r0,#0] /* store data */
    LDMFD SP!,{pc}  /* return */

/*-----------------------------------------------------------------
 hword_read(int addr)
 reads a half word from an address
 r0 contains addr
 -----------------------------------------------------------------*/
.globl hword_read
hword_read:
    STMFD SP!,{r1,lr}  /* save return address and r1 to stack */
    LDRH r1,[r0,#0]    /* load temp with data */
    MOV r0, r1         /* copy temp to r0 for return value */
    LDMFD SP!,{r1,pc}  /* return */
 
/*-----------------------------------------------------------------
 byte_write(int addr, int data)
 writes a byte to an address
 r0 contains addr
 r1 contains data
 -----------------------------------------------------------------*/
.globl byte_write
byte_write:
    STMFD SP!,{lr}  /* save return address to stack */
    STRB r1,[r0,#0] /* store data */
    LDMFD SP!,{pc}  /* return */
 
/*-----------------------------------------------------------------
 byte_read(int addr)
 reads a byte from an address
 r0 contains addr
-----------------------------------------------------------------*/
.globl byte_read
byte_read:
    STMFD SP!,{r1,lr}  /* save return address and r1 to stack */
    LDRB r1,[r0,#0]    /* load temp with data */
    MOV r0, r1         /* copy temp to r0 for return value */
    LDMFD SP!,{r1,pc}  /* return */
 
