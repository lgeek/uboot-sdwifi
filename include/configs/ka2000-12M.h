/*
 * (C) Copyright 2010
 *
 * Configuation settings for the KEYASIC KA2000 board.
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

#ifndef __CONFIG_H
#define __CONFIG_H

/*-----------------------------------------------------------------------
 * CPU, Board, SoC Configuration Options
*/
#define CONFIG_ARM926EJS		1				/*ARM926EJS CPU Core */
#define CONFIG_SOC_KA2000		1				/*KeyAsic KA2000 SoC */
#define CONFIG_KA2000EVM		1				/*Board type*/

#define CONFIG_SYS_CONSOLE_INFO_QUIET 1
/*-----------------------------------------------------------------------
 * Clock, Timer
*/
#if 0
#define CONFIG_SYS_CLK_FREQ		192000000 //96000000			/*ARM clock freq, input clock of PLL */
#define CONFIG_SYS_HZ_CLOCK		24000000 //24000000 //(192000000)			/*Timer input clock freq (timer.c) */
#define CONFIG_HCLK_RATIO       2
#define CONFIG_SYS_UART_CLK		(CONFIG_SYS_CLK_FREQ/4) //(CONFIG_SYS_CLK_FREQ/CONFIG_HCLK_RATIO)

#else     
#define CONFIG_SYS_CLK_FREQ		12000000 //96000000			/*ARM clock freq, input clock of PLL */
#define CONFIG_SYS_HZ_CLOCK		12000000 //96000000 //(192000000)			/*Timer input clock freq (timer.c) */
#define CONFIG_HCLK_RATIO       1
#define CONFIG_SYS_UART_CLK		(CONFIG_SYS_CLK_FREQ) //(CONFIG_SYS_CLK_FREQ/CONFIG_HCLK_RATIO)

//#define CONFIG_SYS_CLK_FREQ		24000000 //96000000			/*ARM clock freq, input clock of PLL */
//#define CONFIG_SYS_HZ_CLOCK		24000000 //96000000 //(192000000)			/*Timer input clock freq (timer.c) */
//#define CONFIG_HCLK_RATIO       2
//#define CONFIG_SYS_UART_CLK		(CONFIG_SYS_CLK_FREQ) //(CONFIG_SYS_CLK_FREQ/CONFIG_HCLK_RATIO)


#endif

#define	CONFIG_SYS_HZ			1000				/*Num of tick/sec (CONFIG_SYS_HZ_CLOCK/CONFIG_SYS_HZ=TIMER_VALUE) */

/*-----------------------------------------------------------------------
 * Hardware drivers
 */


/*-----------------------------------------------------------------------
 * Debug Configuration
 */
#define CONFIG_KA2000_DEBUG 		1

#ifdef CONFIG_KA2000_DEBUG
#undef CONFIG_CMD_CONSOLE
#define CONFIG_CMD_RUN
#define CONFIG_CMD_SPI
#define CONFIG_CMD_SF
#define CONFIG_CMD_EDITENV
#undef CONFIG_CMD_SAVEENV
#define CONFIG_SPI_FLASH
//#define CONFIG_SPI_FLASH_MACRONIX
#define CONFIG_SPI_FLASH_KA2000
#define CONFIG_SYS_SPI_WRITE_TOUT	(5*CONFIG_SYS_HZ)
//#define CONFIG_ENV_OVERWRITE
#define CONFIG_ENV_OFFSET		0x7D000
#undef  CONFIG_ENV_IS_IN_SPI_FLASH
#undef  CONFIG_ENV_IS_IN_FLASH
//#define CONFIG_ENV_ADDR		0x200100
//#define CONFIG_ENV_SIZE		(1*1024)
# define CONFIG_ENV_SPI_BUS	0
# define CONFIG_ENV_SPI_CS	0

#define CONFIG_MMC
#ifdef CONFIG_MMC
#define CONFIG_KA2000_MMC		1
#undef  CONFIG_GENERIC_MMC
#define CONFIG_CMD_MMC  		1
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION		1
#define CONFIG_EXTRA_ENV_SETTINGS       \
	"console_args=console=ttyS0,38400n8\0"			\
	"root_args=root=/dev/ram0 rw\0 initrd=0x800000,4M\0"	\
	"misc_args=mem=30M\0"						\
	"set_bootargs=setenv bootargs ${console_args} ${root_args} ${misc_args}\0" 		\
	"boot_sd=run set_bootargs; mmc init; fatload mmc 1 208000 zImage; go 208000\0"	\
	"bootf=sf probe 0; sf read e00000 3200 19000; go e00000\0" \
	"tu=mmc init; fatload mmc 1 e00000 u-boot.bin; go e00000 \0" \
	"tb=mmc init; fatload mmc 1 0 kaboot.bin; go 0\0" \
	"bu=sf probe 0; sf read e00000 3200 19000; go e00000\0" \
	"program_kernel=sf probe 0; fatload mmc 1 208400 zImage; sf erase 200000 380000; sf write 200000 200000 380000\0" \
	"program_uboot=sf probe 0; sf read e00000 3000 20000; fatload mmc 1 e00200 u-boot.bin;  sf erase 3000 20000; sf write e00000 3000 20000\0" \
	"program_mtd=sf probe 0; fatload mmc 1 500000 mtd_jffs2.bin; sf erase 580000 280000; sf write 500000 580000 280000\0" \
	"bootcmd=run set_bootargs; run bootf\0"
#endif	//CONFIG_MMC
//"program_kernel=sf probe 0; mmc init; fatload mmc 1 100000 uImage;  sf erase 200000 380000; sf write 100000 200000 380000; sf read d00000 200000 100; md d00000 10\0"
//"program_uboot=sf probe 0; mmc init; sf read e00000 3000 20000; fatload mmc 1 e00200 u-boot.bin;  sf erase 3000 20000; sf write e00000 3000 20000; sf read 3200 3200 100; md 3200 10\0"

#else
#undef  CONFIG_CMD_SAVEENV
#define CONFIG_ENV_IS_NOWHERE		1
#endif	//CONFIG_KA2000_DEBUG


/*-----------------------------------------------------------------------
 * Serial Console Configuration
 */
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_COM1		0xa0004000			/*Base address of UART0, =KA_UART_BASE in hardware.h */
#define CONFIG_SYS_NS16550_REG_SIZE	-4
#define CONFIG_SYS_NS16550_CLK		(CONFIG_SYS_UART_CLK)			/*Input clock to NS16550 (eee:12000000,6000000)*/
#define CONFIG_CONS_INDEX		1				/*Use UART0 for console */
#define CONFIG_BAUDRATE			38400				/*Default baud rate, ref by arch/arm/lib/board.c (eee:19200,9600) */
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }	/*Valid baudrates */


/*-----------------------------------------------------------------------
 * Command line configuration
*/
#include <config_cmd_default.h>

#define CONFIG_CMD_CACHE
#undef CONFIG_CMD_NET
//#define CONFIG_CMD_JFFS2						/*Journaling Flash Filesystem Support */
//#define CONFIG_CMD_SPI
//#define CONFIG_CMDLINE_EDITING		1			/*Enable editing & history functions for cmdline input */


/*-----------------------------------------------------------------------
 * Boot Options
*/
#define CONFIG_BOOTDELAY		3				/*Delay (sec) b4 autoboot image, -1 disable autoboot */
#define CONFIG_BOOTARGS			"root=/dev/ram0 rw console=ttyS0,38400n8 mem=30M"
#define CONFIG_BOOTCOMMAND		"run set_bootargs; run bootf"    //bootcmd
//#define CONFIG_BOOTARGS		"root=/dev/ram0 console=ttySA0,19200n8 init=/linuxrc rw initrd=0xc0800000 mem=32M init=/linuxrc" (eee)
//#define CONFIG_BOOTCOMMAND		"bootm 0x400000"
//#define CONFIG_BOOT_RETRY_TIME					/*Enable boot retry (autoboot stopped), specify retry timeout */
//#define CONFIG_BOOT_RETRY_MIN						/*Optional, def to CONFIG_BOOT_RETRY_TIME */
//#define CONFIG_RESET_TO_RETRY
//#define CONFIG_AUTOBOOT_KEYED
//#define CONFIG_AUTOBOOT_PROMPT
//#define CONFIG_AUTOBOOT_DELAY_STR
//#define CONFIG_AUTOBOOT_STOP_STR
//#define CONFIG_ZERO_BOOTDELAY_CHECK
//#define CONFIG_BOOTCOUNT_LIMIT
//#define CONFIG_BOOTFILE		"uImage"			/*Boot file name */


/*-----------------------------------------------------------------------
 * Linux Interfacing
 */
#define	CONFIG_SYS_LOAD_ADDR		0x1ffc00			/*Default Linux kernel load address (eee:0x00000000) 0x33000000*/
#define LINUX_BOOT_PARAM_ADDR   	0x000100 			/*ATAG list offset (eee:PHYS_SDRAM_1 + 0x100) 0x700000*/
#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS



/*-----------------------------------------------------------------------
 * Miscellaneous configurable options
 */
#define	CONFIG_SYS_LONGHELP		1				/*Undef to save memory */
#define	CONFIG_SYS_PROMPT		"KA2000#"			/*Monitor Command Prompt */
#define	CONFIG_SYS_CBSIZE		256				/*Console I/O Buffer Size */
#define	CONFIG_SYS_PBSIZE 		(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16) /*Print Buffer Size */
#define	CONFIG_SYS_MAXARGS		16				/*Max number of command args */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE		/*Boot Argument Buffer Size */
#define CONFIG_VERSION_VARIABLE						/*Set "ver" env with u-boot version string */
//#define CONFIG_DISPLAY_CPUINFO					/*Display cpuinfo with print_cpuinfo(), not implemented */
//#define CONFIG_DISPLAY_BOARDINFO					/*Display boardinfo with checkboard(), not implemented */
//#define CONFIG_HARD_I2C


/*-----------------------------------------------------------------------
 * Memory Info and organization
 */
/* SDRAM */
#define CONFIG_NR_DRAM_BANKS		1	   			/*1 bank of DRAM (arch/arm/lib/board.c) */
#define PHYS_SDRAM_1			0x01000000 			/*SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE		0x01000000 			/*Size 16 MB */
#define CONFIG_SYS_MEMTEST_START	0x01000000
#define CONFIG_SYS_MEMTEST_END		0x011F0000

/* Flash */
#define CONFIG_SYS_FLASH_BASE           0x10000000                      /*Physical start address of Flash memory */
#define CONFIG_SYS_FLASH_SIZE           0x00800000                      /*Flash memory size (8MB) */
#define CONFIG_SYS_MAX_FLASH_BANKS	1				/*Max number of memory banks */
#define CONFIG_SYS_MAX_FLASH_SECT	16384				/*Max number of sectors on 1 chip (include/flash.h) */

/* Environment */
#define CONFIG_ENV_ADDR			(CONFIG_SYS_MONITOR_BASE-CONFIG_ENV_SIZE) /*Addr of ENV (env_flash.c) */
#define CONFIG_ENV_SIZE			(64*1024)			/*Total size of ENV */
#define CONFIG_ENV_SECT_SIZE		512
#define CONFIG_ENV_IS_NOWHERE		1
//#define CONFIG_ENV_OVERWRITE						/*Allow to overwrite serial and ethaddr */

/* Mapping */
#define CONFIG_SYS_MONITOR_BASE		TEXT_BASE			/*Physical start address of boot monitor code */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (128*1024)) 	/*Size of SDRAM reserved for  malloc() & relocated env */
#define CONFIG_SYS_GBL_DATA_SIZE	128				/*Space in bytes reserved for initial data */
#define CONFIG_STACKSIZE		(128*1024)			/*Regular stack (stack sizes are set up in start.S) */

#undef CONFIG_USE_IRQ
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ		(4*1024)			/*IRQ stack size (arch/arm/lib/interrupt.c) */
#define CONFIG_STACKSIZE_FIQ		(4*1024)			/*FIQ stack size (arch/arm/lib/interrupt.c) */
#endif


/*-----------------------------------------------------------------------
 * Init options (start.S, board.c)
 */
#define CONFIG_ARCH_CPU_INIT         					/*placeholder for arch_cpu_init() in cpu.c */
#define CONFIG_SKIP_LOWLEVEL_INIT					/*No low level (cpu) init required */
//#define CONFIG_SKIP_RELOCATE_UBOOT					/*no need to relocate u-boot */
#define CONFIG_SYS_DISABLE_DCACHE
//#define CONFIG_SYS_CACHE_TEST

#define CONFIG_IDENT_STRING     	" on KeyAsic KA2000"		/*(arch/arm/lib/board.c) */
#define CONFIG_ARCH_MISC_INIT						/*Do arch dependent init with arch_misc_init() */
#define CONFIG_MISC_INIT_R						/*Do platform dependent init with misc_init_r() */
#define BOARD_LATE_INIT					   	/*Do board init with board_late_init() */


/*-----------------------------------------------------------------------
 * Flash Support
 */
/* enable */
//#define CONFIG_FLASH_CFI_DRIVER
//#define CONFIG_SYS_FLASH_CFI
//#define CONFIG_ENV_IS_IN_SPI_FLASH
//#define CONFIG_SPI_FLASH_MACRONIX	1
//#define CONFIG_SYS_SPI_WRITE_TOUT	(5*CONFIG_SYS_HZ)
//#define CONFIG_SYS_FLASH_SECT_SZ
//#define CONFIG_SYS_FLASH_ERASE_TOUT	(5*CONFIG_SYS_HZ) 		/*Timeout for Flash Erase (ticks) */
//#define CONFIG_SYS_FLASH_WRITE_TOUT	(5*CONFIG_SYS_HZ) 		/*Timeout for Flash Write (ticks) */

/* disable */
#define CONFIG_SYS_NO_FLASH						/*No need to init flash */
#undef CONFIG_CMD_IMLS
#undef CONFIG_CMD_FLASH
//#undef CONFIG_CMD_SAVEENV

#undef CONFIG_CMD_I2C

#endif	/* __CONFIG_H */
