/*
 * (C) Copyright 2011
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

/*
 * Chanage clock speed support
 */
#include <common.h>
#include <command.h>
#include <clk.h>



int do_clk (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	unsigned int i;
	clk_speed_t *clkspeed;

	if (argc < 2) {
		cmd_usage(cmdtp);
		return 1;
	}

	if (strcmp(argv[1], "set") == 0) {
	    int pllclk = 192;
	    int armclk = 192;
	    int hclk = 96;
	    int sdclk = 48;

	    clkspeed = clk_get_dev();

        clkspeed->baudrate = CONFIG_BAUDRATE;
	    clkspeed->osc_clk = (int)simple_strtoul(argv[2], NULL, 10);
	    if (argc >= 3)
	        pllclk = (int)simple_strtoul(argv[3], NULL, 10);
        if (argc >= 4)
            armclk = (int)simple_strtoul(argv[4], NULL, 10);
        if (argc >= 5)
            hclk = (int)simple_strtoul(argv[5], NULL, 10);
        if (argc >= 6)
            sdclk = (int)simple_strtoul(argv[6], NULL, 10);

	    u32 n = clkspeed->set_clock_speed(pllclk, armclk, hclk, sdclk);

		printf("\nSet clock speed to:\nPLL:\t%dMHz\nARM:\t%dMHz\nHCLK:\t%dMHz\nSD:\t%dMHz\nSDIO:\t%dMHz ... ",
			clkspeed->pllclk, clkspeed->armclk, clkspeed->hclk, clkspeed->sdclk, clkspeed->sdioclk);

		printf("%s\n", n ? "ERROR" : "OK");
		return n;
	} else if (strcmp(argv[1], "get") == 0) {
	    clkspeed = clk_get_dev();
	    clkspeed->osc_clk = (int)simple_strtoul(argv[2], NULL, 10);
	    u32 n = clkspeed->get_clock_speed();

		printf("\nGet current clock speed:\nPLL:\t%dMHz\nARM:\t%dMHz\nHCLK:\t%dMHz\nSD:\t%dMHz\nSDIO:\t%dMHz ... ",
			clkspeed->pllclk, clkspeed->armclk, clkspeed->hclk, clkspeed->sdclk, clkspeed->sdioclk);

		printf("%s\n", n ? "ERROR" : "OK");
		return n;
	} else {
		cmd_usage(cmdtp);
		return 1;
	}

	return 0;
}
/***************************************************/

U_BOOT_CMD(
	clk,	CONFIG_SYS_MAXARGS,	1,	do_clk,
	"Clock Speed",
	"set <osc> <pll> <arm> <hclk> <sd/sdio>\n"
	"get <osc>\n"
);
