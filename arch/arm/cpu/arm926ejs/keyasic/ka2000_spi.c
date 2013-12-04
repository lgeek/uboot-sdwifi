/*
 * (C) Copyright 2009
 * Frank Bodammer <frank.bodammer@gcd-solutions.de>
 * (C) Copyright 2009 Semihalf, Grzegorz Bernacki
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
#include <common.h>
#include <asm/io.h>
#include <malloc.h>
#include <spi.h>
#include <asm/arch/reg_defs.h>
#include <spi_flash.h>

#define show_str(a, b, c, d);  printf("%d, %d, %s", b, d, a);


volatile unsigned int int_flag;
int mode;
#define SLOW_MODE		0
#define NORMAL_50_MODE		1
#define NORMAL_100_MODE		2
#define NORMAL_200_MODE		3
#define TIMER1_INTR		(0x1 << 22)

/* pwm */
#define PWM_BASE                 KA_PWM_BASE
#define TCFG0                    PWM_BASE + 0x00
#define TCFG1                    PWM_BASE + 0x04
#define TCON0                    PWM_BASE + 0x08
#define TCON1                    PWM_BASE + 0x0c
#define TCNTB0                   PWM_BASE + 0x10
#define TCMPB0                   PWM_BASE + 0x14
#define TCNTO0                   PWM_BASE + 0x18
#define TCNTB1                   PWM_BASE + 0x40
#define TCNTO1                   PWM_BASE + 0x44
#define TCNTB2                   PWM_BASE + 0x48
#define TCNTO2                   PWM_BASE + 0x4c


// SD Controller Registers
#define MULTE 		(0x1 << 14)
#define MIMSK 		(0x1 << 13)
#define TAGD  		(0x1 << 12)
#define SPIMW 		(0x1 << 11)
#define MSTR  		(0x1 << 10)
#define CSO   		(0x1 << 9)
#define ENSCK 		(0x1 << 8)
#define SMOD_PO 	(0x0 << 6)
#define SMOD_INT 	(0x1 << 6)
#define SMOD_DMA 	(0x2 << 6)
#define DRD   		(0x1 << 5)
#define DTD   		(0x1 << 4)
#define CSLV  		(0x1 << 3)
#define KEEP  		(0x1 << 2)
#define CPOL  		(0x1 << 1)
#define CPHA  		(0x1 << 0)
#define TMOD_BYTE  	(0x0 << 4)
#define TMOD_HWORD 	(0x1 << 4)
#define TMOD_WORD  	(0x2 << 4)

// NOR Flash MX25L6406E
#define SERIAL_MODE_0	0x0
#define SERIAL_MODE_3   0x3
#define CODE_WREN       0x06
#define CODE_WRDI       0x04
#define CODE_RDSR       0x05
#define CODE_WRSR       0x01
#define CODE_READ       0x03
#define CODE_PP         0x02    //Page Program
#define CODE_SE         0x20    //sector erase
#define CODE_BE         0xd8    //64KB block erase
#define CODE_CE         0xc7    //chip erase
#define CODE_DP         0xb9    //enter deep power down
#define CODE_RDP        0xab    //release from deep power down
#define CODE_REMS       0x90    //output manufacturer ID & DID
#define CODE_RDSCUR     0x2b    //read security register
#define CODE_WRSCUR     0x2f    //set lock-down bit as 1
#define CODE_RDDMC      0x5a    //read DMC mode
#define WIP             0x01
#define WEL             0x02


void init_timer1()
{
    //set timer 1 prescaler
    word_write(TCON0, word_read(TCON0) & 0xff8fffff);
    word_write(TCFG0, (word_read(TCFG0) & 0xff00ffff) | 0x00800000);
    word_write(TCNTB1, 0x0);

    //set timer 1 to interrupt mode & mux1
    if(mode == NORMAL_200_MODE)		//mux1 = 1/8
	word_write(TCFG1, (word_read(TCFG1) & 0xfefffcff) | 0x200);
    else if (mode == NORMAL_100_MODE) 	//mux1 = 1/4
        word_write(TCFG1, (word_read(TCFG1) & 0xfefffcff) | 0x100);
    else 				//mux1 = 1/2
	word_write(TCFG1, word_read(TCFG1) & 0xfefffcff);

    //Unmask timer1 interrupt
    word_write(INTC_INTMSK1_ADDR, word_read(INTC_INTMSK1_ADDR) & ~TIMER1_INTR);
}
void timer1_onoff(int usec)
{
    int val=0;

    if(usec == 0)	//stop timer
    {
	word_write(TCON0, word_read(TCON0) & 0xff8fffff);
	word_write(TCNTB1, 0x0);
    } else
    {
    	//set timer 1 count buffer value (= (freq * MUX * (msec/1000) / (PRE+1)) - 1)
    	if(mode == SLOW_MODE)
	    word_write(TCNTB1, (int)((47  * usec / 1000) - 1));
    	else
       	    word_write(TCNTB1, (int)((194 * usec / 1000) - 1));
    	//start timer
	val = word_read(TCON0) & 0xff8fffff;
    	word_write(TCON0, val | 0x00600000);	//load value to TCNTB1
    	word_write(TCON0, val | 0x00500000);
    }
}

void set_wren()
{
    int i=0, done=0;

    while(done == 0)
    {
#ifdef DEBUG
    //	show_str("Send WREN cmd.\n",0,"\n",1);
#endif
    	while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//SSI_CON ready
    	word_write(SSI_CON, TAGD+SPIMW+MSTR+ENSCK+SMOD_PO+KEEP+CSO);	//enabe CS
    	while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//SSI_TDAT ready
    	word_write(SSI_TDAT, CODE_WREN);				//send WREN cmd
	while((word_read(SSI_STA) & 0x1) == 0) {} ;			//cmd sent, data received
	//word_read(SSI_RDAT);						//discard 1st data (will hang)
    	word_write(SSI_CON, TAGD+SPIMW+MSTR+ENSCK+SMOD_PO+KEEP);	//disable CS

#ifdef DEBUG
    //	show_str("Wait Write Enable status\n", 0, "\n", 1);
#endif
    	while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//SSI_CON ready
    	word_write(SSI_CON, TAGD+SPIMW+MSTR+ENSCK+SMOD_PO+KEEP+CSO);	//enable CS
    	while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//SSI_TDAT ready
    	word_write(SSI_TDAT, CODE_RDSR);				//send RDSR cmd
    	while((word_read(SSI_STA) & 0x1) == 0) {} ;			//cmd sent, data received
	word_read(SSI_RDAT);                                            //discard 1st dat
    	/*do {
    	    while((word_read(SSI_STA) & 0x1) == 0) {} ;
    	} while((word_read(SSI_RDAT) & WEL) == 0) ;*/
    	for(i=0, done=0; ((i<8) && !done); i++)
    	{
    	    while((word_read(SSI_STA) & 0x1) == 0) {} ;			//data received
            if((word_read(SSI_RDAT) & 0x3) == WEL)			//check if WEL bit is set
            	done = 1;
    	}
    	while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//SSI_CON ready
    	word_write(SSI_CON, TAGD+SPIMW+MSTR+ENSCK+SMOD_PO+KEEP);	//disable CS
    }
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//wait ready
#ifdef DEBUG
  //  show_str("Exit set_wren().\n", 0, "\n", 1);
#endif
}

int ssi_write(unsigned int start_addr,unsigned int *write_data, int len)
{
    int i=0, j;
    unsigned int readSta = 0;
	unsigned int v1, v2;
	unsigned char *p;

    //printf("SF Write: %x\n", start_addr);

	if (start_addr < 0x3000)
	{
		printf("can't programe sf before 0x2000 at addr %x\n", start_addr);
		return 0;
	}


    //Set Write Enable & check Write Enable Latch bit
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//wait ready
    word_write(SSI_STA, TMOD_BYTE);
    while((word_read(SSI_STA) & 0x1) == 0) {} ;
    word_read(SSI_RDAT);
    set_wren();

    //change to WORD mode
    word_write(SSI_STA, TMOD_WORD);
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//wait ready
    word_read(SSI_RDAT);

#ifdef DEBUG
    //show_str("Send SSI PAGE_PROGRAM command\n", start_addr, "\n", 1);
#endif
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//SSI_CON ready
    word_write(SSI_CON, TAGD+SPIMW+MSTR+ENSCK+SMOD_PO+KEEP+CSO);	//enable CS
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//SSI_TDAT ready
    word_write(SSI_TDAT, (CODE_PP << 24) + start_addr);			//issue PAGE_PROGRAM cmd
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//cmd sent, data received

    //write data 256 byte to spi flash
    p = &v1;
    for(j=0; (j<(len/4)); j++)
    {
        v1 = write_data[j];					//read received data
        v2 = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
//printf("v1 %x v2 %x\n", v1, v2);
        word_write(SSI_TDAT, v2);				//transmit write data
        while((word_read(SSI_STA) & 0x1) == 0) { } ; 			//data sent

#ifdef DEBUG1
       // show_str("Write:\n", write_data[j], "\n", 1);
#endif
    }
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//SSI_CON ready
    word_write(SSI_CON, TAGD+SPIMW+MSTR+SMOD_PO+KEEP);			//disable CS
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//wait ready

    //change to BYTE mode
    word_write(SSI_STA, TMOD_BYTE);
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//wait Transfer ready (must have!)
    word_read(SSI_RDAT);						//to reset counter

#ifdef DEBUG
   // show_str("Check program start (WIP set)\n", 0, "\n", 1);
#endif
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//SSI_CON ready
    word_write(SSI_CON, TAGD+SPIMW+MSTR+ENSCK+SMOD_PO+KEEP+CSO);	//enable CS
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//SSI_TDAT ready
    word_write(SSI_TDAT, CODE_RDSR);					//issue RDSR cmd
    while((word_read(SSI_STA) & 0x1) == 0) {} ;				//cmd sent, data received
    word_read(SSI_RDAT);                                            	//discard 1st dat
    //while(1) {
    //    while((word_read(SSI_STA) & 0x1) == 0) {} ;			//data received
    //    readSta = word_read(SSI_RDAT);					//read received data
    //    if( (readSta & WIP) == 0x0) break;					//break if WIP set
    //};
    //for(i=0; i<120; i++);						//delay for programming

#ifdef DEBUG
    //show_str("Check Program Done (WIP reset)\n", 0, "\n", 1);
#endif
    timer1_onoff(5000);	//5ms
    do { while((word_read(SSI_STA) & 0x1) == 0) {} ;			//data received
    } while(((word_read(SSI_RDAT) & WIP) == 1) && !int_flag);		//read received data & check if WIP set
    timer1_onoff(0);
    if(int_flag) { //timeout
#ifdef DEBUG0
    	show_str("SSI write timeout.\n",int_flag, "\n", 1);
#endif
	int_flag = 0;
	return 1;
    }

    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//SSI_CON ready
    word_write(SSI_CON, TAGD+SPIMW+MSTR+SMOD_PO+KEEP);
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//wait ready

    //disable Write Enable Latch (Flash reset WEL automatically)
    //send_command(CODE_WRDI, 1, WEL, 1);
    return 0;
}


void ssi_read(unsigned int start_addr, int len, unsigned int* read_buf)
{
    int i, j;
    unsigned int tmpBuf = 0;
    unsigned int pi;
    unsigned char *p;
    //change to WORD mode
    word_write(SSI_STA, TMOD_WORD);
    while((word_read(SSI_STA) & 0x1) == 0) {} ;     			//data ready to read
    word_read(SSI_RDAT); 						//reset counter


    //printf("SF READ %d, ", len);
    //printf(" addr %x\n", start_addr);

    for(i=0; i<20; i++);
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//SSI_CON ready
    word_write(SSI_CON, TAGD+SPIMW+MSTR+ENSCK+SMOD_PO+KEEP+CSO);	//enable CS
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//SSI_TDAT ready
    word_write(SSI_TDAT, (CODE_READ << 24) + start_addr);		//issue READ cmd
    while((word_read(SSI_STA) & 0x1) == 0) {} ;            		//cmd sent, data received

 	word_read(SSI_RDAT);					//read received data

 	p = (unsigned char *)(&pi);
    for(i = 0, j=0; i< len; i+=4, j++)
	{
        pi = word_read(SSI_RDAT);					//read received data
        read_buf[j] = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3] ;
		//if(j > 0) read_buf[j-1] = tmpBuf & 0xffffffff;			//discard 1st data (dummy)
        while((word_read(SSI_STA) & 0x1) == 0) {} 			//next data received

		//printf("%08x, ", read_buf[j]);
		//if ((i % 8)==7) printf("\n");
    }

    //show_str("SSI Read done.\n", 0, "\n", 1);
	//printf("\n");

    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//SSI_CON ready
    word_write(SSI_CON, TAGD+SPIMW+MSTR+ENSCK+SMOD_PO+KEEP);		//disable CS
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//wait ready
}


int sector_erase(unsigned int start_addr)
{
    int i=0, done=0;


    //printf("Erase sector %d\n",start_addr);
    //printf(".");


    //Set Write Enable & check Write Enable Latch bit
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//wait ready
    word_write(SSI_STA, TMOD_BYTE);					//change to BYTE mode
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//receive data ready
    word_read(SSI_RDAT);						//to reset counter
    //while((word_read(SSI_STA) & 0x1) == 0) {} ;
    set_wren();								//set WREN

    //change to WORD mode
    word_write(SSI_STA, TMOD_WORD);					//change to WORD mode
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//receive data ready
    word_read(SSI_RDAT);						//to reset counter

#ifdef DEBUG
    //show_str("Send Sector Erase Cmd.\n",0,"\n",1);
#endif
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//SSI_CON ready
    word_write(SSI_CON, TAGD+SPIMW+MSTR+ENSCK+SMOD_PO+KEEP+CSO);	//enable CS
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//SSI_TDAT ready
    word_write(SSI_TDAT, (CODE_SE << 24) + start_addr);			//issue SE cmd
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//cmd sent, data received
    word_write(SSI_CON, TAGD+SPIMW+MSTR+ENSCK+SMOD_PO+KEEP);		//disable CS
    while((word_read(SSI_STA) & 0x1) == 0) { } ; 			//wait Transfer ready

#ifdef DEBUG
    //show_str("Check in progress status (wait WIP set).\n",0,"\n",1);
#endif
    //check in progress status
    word_write(SSI_STA, TMOD_BYTE);					//change to BYTE mode
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//wait data ready
    word_read(SSI_RDAT);						//to reset counter

    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//SSI_CON ready
    word_write(SSI_CON, TAGD+SPIMW+MSTR+ENSCK+SMOD_PO+KEEP+CSO);	//enable CS
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//SSI_STA ready
    word_write(SSI_TDAT, CODE_RDSR);					//send RDSR cmd
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//cmd sent, data received
    word_read(SSI_RDAT);                                		//discard 1st data

    timer1_onoff(10000); //10ms
    do { while((word_read(SSI_STA) & 0x1) == 0) {} ;			//next data received
    } while(((word_read(SSI_RDAT) & WIP) == 0) && !int_flag);		//read received data & check if WIP set
    timer1_onoff(0);
    if(int_flag) { //timeout
#ifdef DEBUG0
    	show_str("SSI erase timeout(80).\n",int_flag, "\n", 1);
#endif
	int_flag = 0;
	return 1;
    }

    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//SSI_CON ready
    word_write(SSI_CON, TAGD+SPIMW+MSTR+ENSCK+SMOD_PO+KEEP);		//disabe CS

    for(i=0; i<400; i++);
    //check program ok (wait WIP clear)
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//SSI_CON ready
    word_write(SSI_CON, TAGD+SPIMW+MSTR+ENSCK+SMOD_PO+KEEP+CSO);	//enable CS
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//SSI_TDAT ready
    word_write(SSI_TDAT, CODE_RDSR);					//issue RDSR cmd
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//cmd sent, data received
    word_read(SSI_RDAT);                                		//discard 1st data

    timer1_onoff(300000); //300ms
    do { while((word_read(SSI_STA) & 0x1) == 0) {} ;			//next data received
    } while(((word_read(SSI_RDAT) & WIP) == 1) && !int_flag);		//read received data & check if WIP clear
    timer1_onoff(0);
    if(int_flag) { //timeout
#ifdef DEBUG0
    	show_str("SSI erase timeout(300).\n",int_flag, "\n", 1);
#endif
	int_flag = 0;
	return 1;
    }

    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//SSI_CON ready
    word_write(SSI_CON, TAGD+SPIMW+MSTR+ENSCK+SMOD_PO+KEEP);		//disable CS
    while((word_read(SSI_STA) & 0x1) == 0) {} ;         		//wait ready

#ifdef DEBUG
  // show_str("Erase sector done.\n",start_addr,"\n",1);
#endif
   return 0;
}


void ssi_init(void)
{
    init_timer1();	//setup timer 1 for timeout

    //Initialize SSI Controller (NOR Flash)
    while((word_read(SSI_STA) & 0x1) == 0) {}; //Transfer Ready
    word_write(SSI_PRE, 0x0); //baud rate = PCLK/2/(SSI_PRE+1)

}

struct soft_spi_slave {
	struct spi_slave slave;
	unsigned int mode;
};

static inline struct soft_spi_slave *to_soft_spi(struct spi_slave *slave)
{
	return container_of(slave, struct soft_spi_slave, slave);
}


void spi_init (void)
{
    //printf("ka2000 spi_init\n");
	ssi_init();
}

int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return bus == 0 && cs == 0;
}


struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
		unsigned int max_hz, unsigned int mode)
{
	struct soft_spi_slave *ss;

	if (!spi_cs_is_valid(bus, cs))
		return NULL;

	ss = malloc(sizeof(struct soft_spi_slave));
	if (!ss)
		return NULL;

	ss->slave.bus = bus;
	ss->slave.cs = cs;
	ss->mode = mode;

	/* TODO: Use max_hz to limit the SCK rate */

	return &ss->slave;
}

void spi_free_slave(struct spi_slave *slave)
{
	struct soft_spi_slave *ss = to_soft_spi(slave);

	free(ss);
}

int spi_claim_bus(struct spi_slave *slave)
{
	struct soft_spi_slave *ss = to_soft_spi(slave);



	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
	/* Nothing to do */
}



struct ka2000_spi_flash_params {
	u16 idcode;
	u16 page_size;
	u16 pages_per_sector;
	u16 sectors_per_block;
	u16 nr_blocks;
	const char *name;
};

struct ka2000_spi_flash {
	struct spi_flash flash;
	const struct ka2000_spi_flash_params *params;
};

int  spi_xfer(struct spi_slave *slave, unsigned int bitlen,
		const void *dout, void *din, unsigned long flags)
{
	return 0;
}

static inline struct ka2000_spi_flash *to_ka2000_spi_flash(struct spi_flash
							       *flash)
{
	return container_of(flash, struct ka2000_spi_flash, flash);
}


static int ka2000_wait_ready(struct spi_flash *flash, unsigned long timeout)
{

	return 0;

}

int ka2000_sf_read(struct spi_flash *flash, u32 offset, size_t len, void *buf)
{
	int i, n, s;
#define BLK_SZ (4096*4) //512
	n = BLK_SZ;
	s = len;
	for (i = 0; i < len; i += BLK_SZ)
	{
		if (s < BLK_SZ)
			n = s;
        else
            n = BLK_SZ;
        if (i & 1) printf(".");
		ssi_read(offset+i, n, buf+i);
		s -= BLK_SZ;
	}
	//printf("\n---->sf read done\n");
	return 0;
}


int ka2000_sf_write(struct spi_flash *flash,
			  u32 offset, size_t len, const void *buf)
{
	int i, n, s;

	if (offset < 0x3000)
		return 0;
#define W_BLK_SZ 256
	n = BLK_SZ;
	s = len;
	//printf("len=%d\n", len);
	for (i = 0; i < len; i += W_BLK_SZ)
	{
		if (s < W_BLK_SZ)
			n = s;
        else
            n = W_BLK_SZ;

		if((i % 1000) == 0) printf(".");
		//_ka2000_erase(offset+i, BLK_SZ);

		ssi_write(offset+i, buf+i, n);
		s -= W_BLK_SZ;
	}

	//printf("\n---->sf write done\n");
	return 0;
}


int ka2000_sf_erase(struct spi_flash *flash, u32 offset, size_t len)
{

	int i, block;
	int n = len / 4096;

	if (offset < 0x3000 || ((offset % 4096) != 0))
	{
	    printf("offset invalid %d\n", offset);
	    return 0;
	}

	if (len < 4096 || ((len % 4096) != 0))
	{
	    printf("len invalid %d\n", len);
	    return 0;
	}

	if (len % 4096)
		n++;

	for (i = 0; i < n; i++)
	{
		block = offset / 4096;
		sector_erase(block * 4096);
		if((i % 100) == 0) printf(".");
		offset += 4096;
	}

    //printf("\n---->sf erase done\n");
	return 0;
}

struct ka2000_spi_flash ka2000_sf;
struct ka2000_spi_flash_params ka2000_sf_params =
	{
		.idcode = 0x2017,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 128,
		.name = "MX25L6405D",
	};

struct spi_flash *spi_flash_probe_ka2000(struct spi_slave *spi, u8 *idcode)
{
	struct ka2000_spi_flash *sf;
	struct ka2000_spi_flash_params *params;
	unsigned int i;
	u16 id = idcode[2] | idcode[1] << 8;

	//printf("spi_flash_probe_ka2000 %d\n", id);

	sf = &ka2000_sf; //malloc(sizeof(*sf));
	//if (!sf) {
	//	printf("SF: Failed to allocate memory\n");
	//	return NULL;
	//}

    params = &ka2000_sf_params;
	sf->params = params;
	sf->flash.spi = spi;
	sf->flash.name = params->name;

	sf->flash.write = ka2000_sf_write;
	sf->flash.erase = ka2000_sf_erase;
	sf->flash.read = ka2000_sf_read;
	sf->flash.size = params->page_size * params->pages_per_sector
	    * params->sectors_per_block * params->nr_blocks;

	//printf("SF: Detected %s with page size %u, total %u bytes\n",
	//      params->name, params->page_size, sf->flash.size);

	return &sf->flash;
}




