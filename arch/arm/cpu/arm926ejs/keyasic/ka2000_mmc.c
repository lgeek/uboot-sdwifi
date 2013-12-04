
#include <config.h>
#include <common.h>
#include <command.h>
#include <mmc.h>
#include <part.h>
#include <malloc.h>
#include <mmc.h>
#include <asm/errno.h>
#include <asm/io.h>

#include <asm/arch/reg_defs.h>
//#include "ka2000-define.h"
#include "sdctrl.h"


#define DRIVER_NAME "ka2000-mmc"

static block_dev_desc_t mmc_blk_dev;

void test_switch_register()
{
	u32 v0[64];
	u32 v1[64];
	u32 r;
	u32 val;
	int i;
	int n = 24;
	r = SDSW_BASE;

	while (1)
	{
		for (i = 0; i < 64; i++)
		{
			v0[i]=v1[i];
			v1[i] = word_read(r+i*4);
		}

		for (i = 0; i < 64; i++)
		{
			if (v1[i] != v0[i])
				printf("i %d v0 %08x, v1 %08x", i, v0[i], v1[i]);
		}
	}

}

block_dev_desc_t *mmc_get_dev(int dev)
{
	return (block_dev_desc_t *) &mmc_blk_dev;
}

uint32_t mmc_bread(int dev_num, uint32_t blknr, lbaint_t blkcnt, void *dst)
{
	int i = 0;
	int n = blkcnt;
	int m = 512;
	
	SelectM2();

	for (i = 0; i + m < blkcnt; i += m)
	{
		CardRead_single_pin((unsigned int)dst, blknr, m);
		dst += m * 512;
		blknr += m;
		n -= m;
	}

	if (n > 0)
		CardRead_single_pin((unsigned int)dst, blknr, n);
		
	DeselectM2();

	return 1;
}


/* find first device whose first partition is a DOS filesystem */
int find_fat_partition (block_dev_desc_t *dev_desc)
{
	int i, j;
	unsigned char *part_table;
	unsigned char buffer[512];
	unsigned int part_offset = 0;

	if (!dev_desc) {
		debug ("couldn't get ide device!\n");
		return (-1);
	}
	if (dev_desc->part_type == PART_TYPE_DOS) {
		if (dev_desc->
			block_read (dev_desc->dev, 0, 1, (ulong *) buffer) != 1) {
			debug ("can't perform block_read!\n");
			return (-1);
		}
		part_table = &buffer[0x1be];	/* start with partition #4 */
		for (j = 0; j < 4; j++) {
			if ((part_table[4] == 1 ||	/* 12-bit FAT */
			     part_table[4] == 4 ||	/* 16-bit FAT */
			     part_table[4] == 6) &&	/* > 32Meg part */
			    part_table[0] == 0x80) {	/* bootable? */
				//curr_dev = i;
				part_offset = part_table[11];
				part_offset <<= 8;
				part_offset |= part_table[10];
				part_offset <<= 8;
				part_offset |= part_table[9];
				part_offset <<= 8;
				part_offset |= part_table[8];
				debug ("found partition start at %ld\n", part_offset);
				return (0);
			}
			part_table += 16;
		}
	}

	//debug ("no valid devices found!\n");
	return (-1);
}

int detect_test_bin(void)
{
    int ret, size, count;
    u32 *buf;
    buf = (u32 *)0xe00000;

    fat_register_device(&mmc_blk_dev,1);
    //buzzer_off();

    size = file_fat_read ("utest.bin", buf, 128 * 1024);
    if (size > 1 && (buf[0] == 0xea000012 || buf[0] == 0x120000ea))
    {
        buzzer_on(0);
        run_command("go e00000", 0);
        buzzer_off();
    }
    //buzzer_off();
    return 0;
}


int detect_program_bin(void)
{
    int ret, size, count;
    u32 *buf;
    buf = (u32 *)0xe00000;

    fat_register_device(&mmc_blk_dev,1);
    //buzzer_off();

    size = file_fat_read ("program.bin", buf, 128 * 1024);
    if (size > 1 && (buf[0] == 0xea000012 || buf[0] == 0x120000ea))
    {
        buzzer_on(0);
        run_command("go e00000", 0);
        buzzer_off();
    }
    //buzzer_off();
    return 0;
}


int fat_register_device(block_dev_desc_t *dev_desc, int part_no);

int mmc_inited = 0;

int mmc_legacy_init(int verbose)
{
	//test_switch_register();
	printf("MMC Init Card ...\n");
	InitCardReader();
	ReInitCard();

	mmc_blk_dev.if_type = IF_TYPE_MMC;
	mmc_blk_dev.part_type = PART_TYPE_DOS;
	mmc_blk_dev.dev = 0;
	mmc_blk_dev.lun = 0;
	mmc_blk_dev.type = 0;

	/* FIXME fill in the correct size (is set to 32MByte) */
	mmc_blk_dev.blksz = 512;
	mmc_blk_dev.lba = 0x100000;
	mmc_blk_dev.removable = 0;
	mmc_blk_dev.block_read = mmc_bread;

	if (fat_register_device(&mmc_blk_dev, 1))
	{
		printf("Could not register MMC fat device\n");
		init_part(&mmc_blk_dev);
	}

    if (mmc_inited == 0)
    {
        mmc_inited = 1;
        //detect_program_bin();
        //detect_test_bin();
    }

    mmc_inited = 1;

	return 0;
}

