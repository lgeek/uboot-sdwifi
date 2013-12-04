/*
 * Copyright (C) 2010
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>

struct ka_timer {
	u_int32_t	timmercr;
	u_int32_t	wdtcr;
};

static struct ka_timer * const timer =
	(struct ka_timer *)KA_TIMER0_BASE;	//CONFIG_SYS_TIMERBASE;

#define TIMER_LOAD_VAL	(CONFIG_SYS_HZ_CLOCK / CONFIG_SYS_HZ)
#define TIM_CLK_DIV	16

static ulong timestamp;
static ulong lastinc;

int timer_init(void)
{
	// We do not need to initialize timer, dummy function
	//writel(TIMER_LOAD_VAL, &timer->prd34);
	timestamp = 0;
	return(0);
}

void reset_timer(void)
{
	// dummy function
	timestamp = 0;
}

static ulong get_timer_raw(void)
{
    static ulong t;

    if (t == timestamp)
        timestamp++;
    t = timestamp;
	// dummy function
	return timestamp;
}

ulong get_timer(ulong base)
{
	return((get_timer_raw() / (TIMER_LOAD_VAL / TIM_CLK_DIV)) - base);
}

void set_timer(ulong t)
{
	timestamp = t;
}

void __udelay(unsigned long usec)
{
#if 1
	ulong tmo;
	ulong endtime;
	signed long diff;

	tmo = CONFIG_SYS_HZ_CLOCK / 1000;
	tmo *= usec;
	tmo /= (1000 * TIM_CLK_DIV);

	endtime = get_timer_raw() + tmo;

	do {
		ulong now = get_timer_raw();
		diff = endtime - now;
	} while (diff >= 0);
#else
	volatile int i;
	for(i=0; i<(33*usec); i++);
#endif
}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return(get_timer(0));
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk(void)
{
	return CONFIG_SYS_HZ;
}
