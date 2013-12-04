/*
 * Copyright (C) 2010
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <common.h>
#include <clk.h>
#include <asm/arch/hardware.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/arch/reg_defs.h>
#define PLL_INIT_FINISH (1 << 28)

//#if 0
#if OSC == 24
#define OSC_MHZ   24
#define HCLK_RATE  CONFIG_HCLK_RATIO
#else
#define OSC_MHZ   12
#define HCLK_RATE  CONFIG_HCLK_RATIO
#endif

void buzzer_off()
{
	word_write (TCON0, word_read(TCON0) & ~1);
}

void buzzer_on(int tone)
{
	word_write(TCFG0, (word_read(TCFG0) & 0xffffff00) | 0x80);	//set Timer0 prescaler=128
	word_write(TCFG1, (word_read(TCFG1) & 0xffeffffc));		//set Timer0 clk input=1/2, interrupt mode

	//set Timer0 duty cycle and count
	if(tone==0) { //low frequency for Fail indication
		word_write(TCMPB0, 0x800);
		word_write(TCNTB0, 0x1000);
	} else {	//high frequency for Pass indication
		word_write(TCMPB0, 0x100);
		word_write(TCNTB0, 0x200);
	}

	word_write(TCON0, (word_read(TCON0) | 0x09));		//set Timer0 auto-reload and start it
}

//------------------------------------------------------------------------------
static void ka2000_set_gpio0(u32 val)
{
	u32 reg_val;

	reg_val = word_read(0xa0005008);
	if (val)
		reg_val |= 1;
	else
		reg_val &= ~1;

	word_write(0xa0005008, reg_val);
	word_write(0xa0005000, 1);   /* set gpio output */
}

void set_clock(int mhz)
{
	volatile ulong val;
	volatile int t = 1000000;
	int div;
	int div_n;
	int div_k;
	int div_m;
#if 0
	printf("\n0xa0000000 %08x\n", word_read(0xa0000000));
	printf("0xa0000004 %08x\n", word_read(0xa0000004));
	printf("0xa0000008 %08x\n", word_read(0xa0000008));
	printf("0xa000000c %08x\n", word_read(0xa000000c));
#endif

	if (mhz == OSC) {
		//word_write(0xa0000000, 0x10c26600);
		printf("clk: OSC %dMHz\n", OSC_MHZ);
		goto end_setclk; //return;
	}

// Normal mode (1)
//     PLL        200MHz
//     ARM        200Mhz
//     HCLK        50MHz
//     PCLK        50MHz
//     SD Card     50MHz
//     SDRAM      100MHz

// mw a000000c 00006003 ; mw a000000c 00006001 ; mw a000000c 00006000 ; mw a0000004 00000100 ; mw a0000008 00040404 ; mw a000400c 83; mw a0004004 00; mw a0004000 a1; mw a000400c 3; mw a0000000 01c33717;

	/*
	mw a000000c 00006707 ; mw a000000c 00006705 ; mw a000000c 00006704 ;
	mw a0000004 00000100 ; mw a0000008 00080808 ;

	mw a000400c 83; mw a0004004 00; mw a0004000 61; mw a000400c 3;
	mw a0000000 01c33715;
	*/
	/* CLK_OUT = FIN * (PLLDIVN [5:0] +1)/ ((PLLDIVM+1) * PLLDIVK [1:0]). */
	div_m = 0;
	div_k = 2;
	div_n = ((mhz / OSC_MHZ) * (1 << div_k)) - 1;
	div = (div_k << 14)  | (div_n << 8) | (div_m << 2);
	if (word_read(0xa000000c) == div) {
		printf("clk: PLL (div=%x)\n", div);
		goto end_setclk; //return;
	}

	val = word_read(0xa0000000);
	printf("div=%x\n", div);

	//printf("Change to %dMHz, div m %d, n %d, k %d\n", mhz, div_m, div_n, div_k);

	word_write(0xa0000000, val & ~(0xff));      //use OSC clk temporarily
	//printf("Changing OSC... \n");
	null_delay(10);

#define PLL_RESET      2
#define PLL_POWER_DOWN 1
#define DIV_N(v) (v << 8)
#define DIV_K(v) (v << 14)

	/* Setup PLL freq change */

	word_write(0xa000000c, 3 | div);
	word_write(0xa000000c, 1 | div);
	//word_write(0xa000000c, (div_k << 14) | (div_n << 8) | PLL_RESET | PLL_POWER_DOWN);  //0x4303
	//word_write(0xa000000c, (div_k << 14) | (div_n << 8) | PLL_POWER_DOWN);                   //0x4301

	null_delay(100);
	/* doing PLL change */
	word_write(0xa000000c, div);
	//word_write(0xa000000c, (div_k << 14) | (div_n << 8));                               //0x4300

	/* wait PLL init finish */
	while((word_read(0xa0000000) & PLL_INIT_FINISH) != PLL_INIT_FINISH) {
		null_delay(1);
		if (t-- <= 0)
			break;
	};

#define SPI_PLL(v) ((v) << 16)
#define SDRAM_PLL(v) ((v) << 8)
#define ARM_PLL(v) ((v) << 0)
	/* PLL Frequency Select Register 1 - 0xa0000004 */
	//word_write(0xa0000004, 0x00100000);
	//word_write(0xa0000004, 0x000000100);
	//word_write(0xa0000004, SPI_PLL(0x10) | SDRAM_PLL(0x1));

	//word_write(0xa0000008, 0x06040004);
	//word_write(0xa0000008, 0x00080808);     //0x00040404

	word_write(0xa000400c, 0x83);
	word_write(0xa0004004, 0x00);
	word_write(0xa0004000, 0xa1);
	word_write(0xa000400c, 0x03);
	//mw a000400c 83; mw a0004004 00; mw a0004000 a1; mw a000400c 3

#define SDHOST_SEL_PLL    (1 << 12)
#define SDRAM_SEL_PLL     (1 << 4)
#define ARM_CLK_RATIO(v)  ((v - 1) << 1)
#define ARM_PLL_SRC       1
	/* setting the new ratio */
	//word_write(0xa0000000, val | 0x11 | ARM_CLK_RATIO(1) | 0x6600);
	val &= ~SDHOST_SEL_PLL;
	val |= 0x11  | 0x600 | ARM_CLK_RATIO(HCLK_RATE);

end_setclk:
	word_write(0xa0000008, 0x00040404);     //0x00040404 //0x00080808
	word_write(0xa0000004, 0x00000000);
	if (mhz == OSC)  //12
		word_write(0xa0000000, 0x10c26600);     //0x10c26600
	else {          //PLL
		word_write(0xa0000000, 0x01c33313);    //0625=0x01c33717, 0x01c33317, 0x01c37317, 0x01c37313, 0x01c33313, 0x01c33311
		//word_write(0xa0000000, 0x01c33317);
	}
	// word_write(0xa0000000, val );
	//word_write(0xa0000000, val | SDRAM_SEL_PLL | ARM_PLL_SRC | ARM_CLK_RATIO(1));
	printf("scu: %08x,%08x,%08x,%08x,%x\n", word_read(0xa0000000), word_read(0xa0000004), word_read(0xa0000008), word_read(0xa000000c), word_read(0xa0000014));

}

#ifdef CONFIG_SOC_KA2000
//ref in include/configs/ka2000.h
int clk_get(enum ka_clk_ids id)
{
	return CONFIG_SYS_HZ_CLOCK;
}
#endif /* CONFIG_SOC_KA2000 */

#ifdef CONFIG_DISPLAY_CPUINFO
static unsigned pll_clk_mhz()
{
	//TODO: placeholder

	return (CONFIG_SYS_CLK_FREQ / 1000);
}

int print_cpuinfo(void)
{
	// printf("Cores: ARM %d MHz\n", 192); 	//pll_clk_mhz());
	// printf("SDRAM: %d MHz\n", 96);	//pll_clk_mhz());

	return 0;
}
#endif /* CONFIG_DISPLAY_CPUINFO */

#ifdef CONFIG_ARCH_CPU_INIT
int arch_cpu_init(void)
{
	/* disable wifi module power by disable gpio 0 */
	ka2000_set_gpio0(0);

	//TODO: placeholder
	if (CONFIG_SET_CLOCK)
		set_clock(CONFIG_SYS_CLK_FREQ / 1000000);

	/*	//change to normal mode
		word_write(0xa0000000,0x00000000);
		word_write(0xa0000004,0x00000100);
		word_write(0xa0000008,0x00101010);
		word_write(0xa000000c,0x00006003);
		word_write(0xa000000c,0x00006001);
		word_write(0xa000000c,0x00006000);
		while((word_read(0xa0000000) & 0x10000000) != 0x10000000) {};
		word_write(0xa0000000,0x00037713);
	*/
	return 0;
}
#endif /* CONFIG_ARCH_CPU_INIT */

#if defined(CONFIG_ARCH_MISC_INIT)
void pagetbl_setup(unsigned int phy_addr, unsigned int cache_enable, unsigned int write_buf)
{
	unsigned int base_addr = phy_addr >> 20;
	word_write(0x01f04000 + (base_addr * 4), 0);
	word_write(0x01f04000 + (base_addr * 4), (base_addr<<20) | (0x03<<10) | (0x00<<5) | (1<<4) | (cache_enable<<3) | (write_buf<<2) | 0x02);
}
void pagetbl_init()
{
	pagetbl_setup(0x01f00000, 0x00, 0x00);	//setup page table addr, non-cacheable, non-buffered, size 1MB
	//pagetbl_setup(0x00000000, 0x00, 0x00);	//setup page table addr, non-cacheable, non-buffered, size 1MB
	pagetbl_setup(0xa0000000, 0x00, 0x00);	//setup peripheral addr, non-cacheable, non-buffered, size 1MB
}

int arch_misc_init(void)
{
	int result = 1, i;
	uint32_t reg;

	cpu_setup();

	buzzer_on(1);

	pagetbl_init();
	for(i = 0; i < 31; i++) //setup a special mem location, cacheable, buffered, write-back mode, size 1MB
		pagetbl_setup((i<<20), 0x01, 0x01);
	//printf("Page Table setup completed.\n");

#if 1//def CONFIG_SKIP_LOWLEVEL_INIT
	//printf("ICACHE disabled.\n");
#else
	icache_enable();
	//printf("ICACHE enabled.\n");
#endif /* CONFIG_SKIP_LOWLEVEL_INIT */

#ifdef CONFIG_SYS_DISABLE_DCACHE
	//printf("DCACHE & MMU disabled.\n");
#else
	//enable MMU & dcache
	//dcache_enable();
	reg = get_cr();
	set_cr(reg | CR_M | CR_S | CR_R | CR_C);	//set MMU, system MMU protect, ROM MMU protect, dcache
	nop();
	nop();
	//printf("DCACHE & MMU enabled. CR=0x%x\n", reg);
#ifdef CONFIG_SYS_CACHE_TEST
	for(i=0; i<0x100; i++)
		word_write(0x00200180 + (i*4),i);

	for(i=0; i<0x100; i++) {
		if(word_read(0x00200180 + (i*4)) != i)
			printf("Cache test Failed. Exp: 0x%x, Act: 0x%x\n", i, word_read(0x00200180 + (i*4)));
	}
	word_write(0xc1800000, 0x01);
	printf("Cache test completed.\n");
#endif /* CONFIG_SYS_CACHE_TEST */

#endif /* CONFIG_SYS_DISABLE_DCACHE */

	return 0;
}

int program_mode()
{
	//TODO: placeholder
	//Assume: SSI & SD Host clk are enabled, SD card initialized
	//Init SD Controller
	//Init SSI Controller
	//Allocate buffer for write data

	//Read data from SD Card into buffer (block-512B)
	//Switch SSI control to NOR Flash (def was Autoload)
	//Erase NOR Flash sector (sector-4kB)
	//Write data to NOR Flash (page-256B)
	//Read & Compare NOR Flash data	(page-256B)

	//Free buffer

	return 0;
}

#endif /* CONFIG_ARCH_MISC_INIT */

#ifdef CONFIG_CMD_CLK
#define ARM_RATIO(v,s)  ((v - 1) << s)
#define PLL_RATIO(v,s)  ((v == 2) ? (1 << s) : (1 << (int)(v / 2)) << s)
static clk_speed_t clk_dev;

int pll_ratio(int input, int type)
{
	//type: 1=div to reg (set), 0=reg to div (check)
	int result = 0;
	switch(input) {
	case 0:
		result = type ? 0 : 1;
		break;
	case 1:
		result = type ? 0 : 2;
		break;
	case 2:
		result = type ? 1 : 3;
		break;
	case 3:
		result = type ? 2 : 1;
		break;
	case 4:
		result = 4;
		break;
	case 6:
		result = type ? 8 : 1;
		break;
	case 8:
		result = type ? 8 : 6;
		break;
	case 10:
		result = type ? 0 : 8;
		break;
	default:
		result = type ? 0 : 1;
		break;
	}
	return result;
}

uint32_t set_clock_f(int pllclk, int armclk, int hclk, int sdclk)
{
	volatile ulong val;
	volatile int t = 1000000;
	int div;
	int div_n;
	int div_k = 2;
	int div_m = 0;
	unsigned int uartval;
	int sdratio = 0;
	int hclkratio = 0;
	int armratio = 0;

	printf("\npre: %08x,%08x,%08x,%08x,%x", word_read(0xa0000000), word_read(0xa0000004), word_read(0xa0000008), word_read(0xa000000c), word_read(0xa0000014));
	//set pll clock
	if (armclk == clk_dev.osc_clk) {
		pllclk = clk_dev.osc_clk;
		printf("OSC %dMHz\n", OSC_MHZ);
		goto set_ratio;
	}

	div_n = ((pllclk / clk_dev.osc_clk) * (1 << div_k)) - 1;
	div = (div_k << 14)  | (div_n << 8) | (div_m << 2);
	printf(" (PLL %dMHz, div=%x)", pllclk, div);

	if (word_read(0xa000000c) == div)
		goto set_ratio;

	val = word_read(0xa0000000);
	word_write(0xa0000000, val & ~(0xff));      //use OSC clk temporarily
	null_delay(10);

	word_write(0xa000000c, 3 | div);
	word_write(0xa000000c, 1 | div);
	null_delay(100);
	word_write(0xa000000c, div);
	/* wait PLL init finish */
	while((word_read(0xa0000000) & PLL_INIT_FINISH) != PLL_INIT_FINISH) {
		null_delay(1);
		if (t-- <= 0) {
			armclk = hclk = pllclk = sdclk = clk_dev.osc_clk;
			printf("\nFailed to change PLL, use OSC.");
			break;
		}
	};

set_ratio:
	//set uart
	uartval = (unsigned int)((hclk * 1000000 / 16 / clk_dev.baudrate) - 1);
	word_write(0xa000400c, 0x83);
	word_write(0xa0004004, 0x00);
	word_write(0xa0004000, uartval/*0xa1*/);
	word_write(0xa000400c, 0x03);
	null_delay(10);

	if (armclk == clk_dev.osc_clk) {
		word_write(0xa0000000, 0x10c26600);     //0x10c26600
		word_write(0xa000000c, 0x3);
		goto done;
	}

	/* setting the new ratio */
	sdratio = (int)(pllclk / sdclk);
	hclkratio = (int)(armclk / hclk);
	armratio = (int)(pllclk / armclk);

	if(sdratio == 1)
		word_write(0xa0000008, 0x00000000);
	else
		word_write(0xa0000008, PLL_RATIO(sdratio,16) | PLL_RATIO(sdratio,8) | PLL_RATIO(sdratio,0));

	if(armratio == 1)
		word_write(0xa0000004, 0x00000000);
	else
		word_write(0xa0000004, 0x00000000 | PLL_RATIO(armratio,0));

	word_write(0xa0000000, 0x01c33301 | ARM_RATIO(hclkratio,1));    //0x01c33313, 0x01c37313

done:
	printf("\npost:%08x,%08x,%08x,%08x,%x", word_read(0xa0000000), word_read(0xa0000004), word_read(0xa0000008), word_read(0xa000000c), word_read(0xa0000014));
	clk_dev.pllclk = pllclk;
	clk_dev.armclk = armclk;
	clk_dev.hclk = hclk;
	clk_dev.sdclk = clk_dev.sdioclk = sdclk;
	return 0;
}

uint32_t get_clock_f(void)
{
	unsigned int val = 0;
	unsigned int sdfreq = 0;
	int div_n = 0;
	int div_k = 2;
	int div_m = 0;
	int armratio = 0;

	printf("\nscu: %08x,%08x,%08x,%08x,%x", word_read(0xa0000000), word_read(0xa0000004), word_read(0xa0000008), word_read(0xa000000c), word_read(0xa0000014));
	val = word_read(0xa0000000);
	if((val & 0x1) == 0) {
		clk_dev.pllclk = clk_dev.armclk = clk_dev.hclk = clk_dev.sdclk = clk_dev.sdioclk = clk_dev.osc_clk;
		return 0;
	}

	//PLL clock
	div_n = (word_read(0xa000000c) >> 8) & 0x3f;
	clk_dev.pllclk = (int)(clk_dev.osc_clk * (div_n + 1) / (1 << div_k) / (div_m + 1));

	//ARM clock
	armratio = word_read(0xa0000004) & 0x1f;
	clk_dev.armclk = clk_dev.pllclk / pll_ratio(armratio,0);

	//HCLK
	clk_dev.hclk = clk_dev.armclk / (((val & 0xe) >> 1) + 1);

	//SD, SDIO clock
	sdfreq = word_read(0xa0000008);
	clk_dev.sdclk = clk_dev.pllclk / pll_ratio((sdfreq & 0x1f),0);
	clk_dev.sdioclk = clk_dev.pllclk / pll_ratio(((sdfreq >> 8) & 0x1f),0);

	return 0;
}

clk_speed_t *clk_get_dev(void)
{
	clk_dev.set_clock_speed = set_clock_f;
	clk_dev.get_clock_speed = get_clock_f;
	return (clk_speed_t *) &clk_dev;
}


#endif
