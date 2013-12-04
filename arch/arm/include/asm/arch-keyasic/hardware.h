/*
 *  linux/include/asm-arm/arch-keyasic/hardware.h
 *
 *  Copyright (C) 2010 
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
#ifndef __ASM_ARCH_HARDWARE_H
#define __ASM_ARCH_HARDWARE_H

#include <config.h>
#include <asm/sizes.h>

#define	REG(addr)	(*(volatile unsigned int *)(addr))
#define REG_P(addr)	((volatile unsigned int *)(addr))

typedef volatile unsigned int	ka_reg;
typedef volatile unsigned int *	ka_reg_p;

/*
 * Base register addresses
 *
 */
#define KA_REGIF_BASE               	0xA0000000
#define KA_SCU_BASE			KA_REGIF_BASE			/* System Control Unit */
#define KA_SSI_BASE                 	KA_REGIF_BASE + 0x1000
#define KA_PWM_BASE                 	KA_REGIF_BASE + 0x2000
#define KA_WDT_BASE                 	KA_REGIF_BASE + 0x3000
#define KA_UART_BASE                	KA_REGIF_BASE + 0x4000
#define KA_GPIO0_BASE                	KA_REGIF_BASE + 0x5000
#define KA_INTC_BASE                 	KA_REGIF_BASE + 0x6000
#define KA_DMA_BASE			KA_REGIF_BASE + 0x7000
#define KA_SDRAM_CTRL_BASE           	KA_REGIF_BASE + 0x8000
#define KA_SDIO_BASE                	KA_REGIF_BASE + 0x9000
#define KA_SD_SWITCH_BASE           	KA_REGIF_BASE + 0xa000
#define KA_SD_CTRL_BASE                	KA_REGIF_BASE + 0xb000
#define KA_GPIO1_BASE                	KA_REGIF_BASE + 0xc000
//#define KA_I2C_BASE			KA_REGIF_BASE + 0xc000
#define KA_TIMER0_BASE			(0x01c21400)

/* arch/arm/cpu/arm926ejs/keyasic/rw_support.S */
extern void word_write(int addr, int data);
extern int  word_read(int addr);
extern void hword_write(int addr, int data);
extern int  hword_read(int addr);
extern void byte_write(int addr, int data);
extern int  byte_read(int addr);

/* arch/arm/cpu/arm926ejs/keyasic/ka2000.c */
void ka_errata_workarounds(void);

/* Clock IDs */
enum ka_clk_ids {
	KA_SPI0_CLKID = 2,
	KA_UART_CLKID = 2,
	KA_ARM_CLKID = 6,
	KA_PLLM_CLKID = 0xff,
	KA_PLLC_CLKID = 0x100,
	KA_AUXCLK_CLKID = 0x101
};
int clk_get(enum ka_clk_ids id);

/* Interrupt controller */
struct ka_intc_regs {
	ka_reg	revid;
};
#define ka_intc_regs ((struct ka_intc_regs *)KA_INTC_BASE)

/* UART controller*/
struct ka_uart_ctrl_regs {
	ka_reg	revid;
};
#define ka_uart0_ctrl_regs ((struct ka_uart_ctrl_regs *)KA_UART_BASE)

#endif /* __ASM_ARCH_HARDWARE_H */
