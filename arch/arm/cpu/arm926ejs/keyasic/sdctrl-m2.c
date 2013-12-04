/* SD Controller
 *
 * SD card must be written to 6 blks to prevent file system for message lost.
 *
 */
#include <config.h>
#include <common.h>
#include <mmc.h>
#include <asm/errno.h>
#include <asm/arch/hardware.h>
#include <part.h>

#include <asm/arch/reg_defs.h>

#include "sdctrl.h"

char dma_buf[4*1024*1024];

#define DMA_WADDR 	(dma_buf)
#define DMA_RADDR 	(dma_buf)
#define EN_INTR	  	(0x1  << 24)
#define BLK_CNT		0x01
#define XFER_COUNT	0x200
#define BUF_BLK_CNT	    (BLK_CNT << 16)
#define BUF_XFER_START	(0x1  << 2)
#define BUF_WRITE	    (0x1  << 1)
#define FIFO_XFER_DONE	0x1
#define SD_XFER_DONE	0x2
#define SD_CMD_DONE	0x4
#define SD_DATA_BOUND	0x8
#define DMA_CH0_INTR	0x1
#define DMA_CH1_INTR	0x2

int HighCap;
int RCA0 = 0, RCA1 = 0;
unsigned int OCR1 = 0, OCR2 = 0, RSP1;


#ifdef show_str_en
	#define DBG printf
	#define show_str(a, b, c, d);  printf("%d, %d, %s", b, d, a );
#else
	#define DBG(...); {}
	void show_str(char *start,unsigned int input,char *end,int end_len){}
#endif


//------------------------------------------------------------------------------
static int null_delay(int t)
{
    volatile int i = 0;
    int j = 0;
    for (i = 0; i < t; i++)
    {
        j++;
    }
    return j;
}
//------------------------------------------------------------------------------
static void dump_regs()
{

}
//------------------------------------------------------------------------------
static void dump_buffer(unsigned char *buf, int count)
{
#ifdef show_str_en
    int i;
    unsigned char *p = (unsigned char*)(buf);

    if (count <= 8)
    {
    }
    else
    {
        for(i = 0; i < count; i+=16)
        {
        }
    }

#endif
}

//------------------------------------------------------------------------------
void SoftReset(void)
{
    word_write(SDR_CTRL_REG, word_read(SDR_CTRL_REG) | 0x40);
    udelay(10);
}

//------------------------------------------------------------------------------
int CardErase_single(unsigned int blk_no)
{
    unsigned int LBA0, LBA1, LBA2;


    WaitBusReady(0x0c);

    LBA0 = ( blk_no & 0x000000ff);
    LBA1 = ( blk_no & 0x0000ff00) >> 8;
    LBA2 = ( blk_no & 0x00ff0000) >> 16;

    word_write(SDR_Card_BLOCK_SET_REG,(0x00000100|BLK_CNT));

    if(HCmdNoData(0x00, LBA2, LBA1, LBA0, 0x20, 0x38, 0x11))
        goto fail;

    if(HCmdNoData(0x00, LBA2, LBA1, LBA0, 0x21, 0x38, 0x11))
        goto fail;

    if(HCmdNoData(0x00, 0x00, 0x00, 0x00, 0x26, 0x38, 0x11))
        goto fail;

    if(WaitResp(SD_XFER_DONE, 0))
        goto fail;

    word_write(SDR_DMA_TRAN_RESP_REG, SD_XFER_DONE);



    return 0;
fail:

    return 1;
}

//------------------------------------------------------------------------------
int WaitResp(unsigned int expResp, int delay)
{
    volatile unsigned int i, result = 0;
    volatile int time_out = 100000;


    for(i=0; i<delay; i++);

    while( (word_read(SDR_DMA_TRAN_RESP_REG) & expResp) != expResp )
    {
        if (time_out-- <= 0)
        {
            break;
        }
    }
	if (time_out <= 0)

	word_write(SDR_DMA_TRAN_RESP_REG, expResp);

    result = word_read(SDR_STATUS_REG);
    if (result & 0x200000)
    {
	}

    if( (result & 0x4f0000))
    {
        result = 1;
    }
    else
        result = 0;

    return result;
}
//------------------------------------------------------------------------------
void WaitDMAIntr(unsigned int channel)
{
    volatile int time_out = 100000;
    while((word_read(SDR_DMA_INTS_REG) & channel) != channel) //wait DMA transfer done
    {
        if (time_out-- <= 0)
        {
            break;
        }
    }
    word_write(SDR_DMA_INTS_REG, word_read(SDR_DMA_INTS_REG));
}
//------------------------------------------------------------------------------
int WaitCardReady()
{
    if (HCmdNoData(RCA1, RCA0, 0x00, 0x00, 0x0D, 0x38, 0x11))
        goto fail;
    else
        return ~( (word_read(SDR_RESPONSE1_REG) & 0x00000001));
fail:
    return 1;
}
//------------------------------------------------------------------------------
void WaitBusReady(unsigned int expStatus)
{
    unsigned int CR_BUS_STS;

    do
    {
        CR_BUS_STS = word_read(SDR_STATUS_REG);
    }
    while(  ((CR_BUS_STS >> 8) & expStatus ) != expStatus);

}
//------------------------------------------------------------------------------
int InCaIsSDVer(int Ver, int CCap)
{
    volatile int i;

    for(i=0; i<50000; i++);

    if(Ver==1)
    {

        HighCap=0;
        OCR1=0;
        OCR2=0;

        if(ACMD41tillReady(0x00) == 1)
            goto fail;
    }
    else
    {

        if(HCmdNoData(RCA1, RCA0, 0x00, 0x00, 0x37, 0x38, 0x11))
            goto fail;

        if(HCmdNoData(0, 0, 0, 0x00, 0x29, 0x38, 0x11))
            goto fail;

        OCR1 = (word_read(SDR_RESPONSE1_REG) & 0x0000ff00);
        OCR2 = (word_read(SDR_RESPONSE1_REG) & 0x00ff0000) >> 16;


        if(ACMD41tillReady(0x40) == 1)
            goto fail;
    }

    return 0;
fail:

    return 1;
}

//------------------------------------------------------------------------------
int ACMD41tillReady(int CCap)
{
	int retry = 100;


    while(1)
    {
        if(HCmdNoData(RCA1, RCA0, 0x00, 0x00, 0x37, 0x38, 0x11))
            goto fail;

        if(HCmdNoData(CCap, OCR2, OCR1, 0x00, 0x29, 0x38, 0x11))
        {
        	if (retry-- > 0)
        		continue;
        	else
        		goto fail;
		}


        HighCap = word_read(SDR_RESPONSE1_REG);

        if( (HighCap & 0x80000000) == 0x80000000) 
            goto pass;
    }
pass:
    return 0;

fail:

    return 1;
}

//------------------------------------------------------------------------------
void write_512_buffer(unsigned int dest_addr, unsigned int DMA_RADDR_start)
{
    unsigned int data_temp;
    int j;

    for(j=0; j<128; j++)
    {
        data_temp = word_read( DMA_RADDR_start + (j*4));
        word_write(dest_addr + (j*4), data_temp);
    }
}

//------------------------------------------------------------------------------
int CardRead_dma(unsigned int buf_dest_addr, unsigned int blk_no, int blk_order)
{
    int DMA_RADDR_start;

    DMA_RADDR_start = (unsigned int)DMA_RADDR + (512 * (blk_order));
    word_write(SDR_DMA_DACH1_REG, DMA_RADDR_start);
    word_write(SDR_DMA_CTRCH1_REG,0x3F);
    word_write(SDR_DMA_CTRCH1_REG,0x33);
    word_write(SDR_BUF_TRAN_CTRL_REG, (BUF_BLK_CNT | BUF_XFER_START));

    if(WaitResp(FIFO_XFER_DONE, 0))		
       goto fail;
    WaitDMAIntr(DMA_CH1_INTR);		

    memcpy((void *)buf_dest_addr, (void *)DMA_RADDR, (blk_order + 1) * 512);

    return 0;

fail:
    return 1;
}

//------------------------------------------------------------------------------
void command_read_pre_clear_status(void)
{
    word_write(SDR_Card_BLOCK_SET_REG,(0x00000100|BLK_CNT));

    word_write(SDR_DMA_TRAN_RESP_REG, (FIFO_XFER_DONE|SD_XFER_DONE|SD_CMD_DONE));
    word_write(SDR_DMA_INTS_REG, 0x3);						
}

//------------------------------------------------------------------------------
int command_read_single_block(unsigned int blk_flag)
{
	unsigned int LBA0, LBA1, LBA2;

    LBA0 = (( blk_flag) & 0x000000ff);
    LBA1 = (( blk_flag) & 0x0000ff00) >> 8;
    LBA2 = (( blk_flag) & 0x00ff0000) >> 16;

    if(HCmdNoData(0x00, LBA2, LBA1, LBA0, 0x11, 0x79, 0x11))
        return 1;
    return 0;
}
#if 0
//------------------------------------------------------------------------------
int CardRead_single_pin(unsigned int buf_dest_addr, unsigned int blk_no, int blk_num)
{
    unsigned int i;
    int blk_flag=0;
    unsigned int DMA_RADDR_start;



    for(i = 0; i<blk_num ; i++)
    {
        WaitBusReady(0x0c);

        blk_flag = blk_no + i;


		command_read_pre_clear_status();
        command_read_single_block(blk_flag);

        if(i > 0)
        {
            DMA_RADDR_start = (unsigned long)DMA_RADDR + (512 * (i-1));

            word_write(SDR_Card_BLOCK_SET_REG,(0x00000100|BLK_CNT));
            word_write(SDR_DMA_TCCH1_REG, XFER_COUNT);
            word_write(SDR_DMA_DACH1_REG, DMA_RADDR_start);
            word_write(SDR_DMA_CTRCH1_REG, 0x3F);
            word_write(SDR_DMA_CTRCH1_REG, 0x33);
            word_write(SDR_BUF_TRAN_CTRL_REG, BUF_BLK_CNT | BUF_XFER_START);

            if(WaitResp(FIFO_XFER_DONE|SD_XFER_DONE, 0))
                goto fail;

            WaitDMAIntr(DMA_CH1_INTR);	

        }
        else if(i==0)
        {
            if(WaitResp(SD_XFER_DONE, 0))	
                goto fail;
        }

    }

    if((i-1)==(blk_num-1))
    {
        CardRead_dma(buf_dest_addr, blk_flag, (i-1));
    }

    return 0;

fail:
    return 1;
}
#else

int ReadDma(unsigned int blk_no, unsigned int blk_cnt, unsigned int startAddr)
{
    int DMA_RADDR_start;

    DMA_RADDR_start = startAddr + (512 * blk_no);	
    word_write(SDR_DMA_TCCH1_REG, (512 * blk_cnt));
    word_write(SDR_DMA_DACH1_REG, DMA_RADDR_start);

    if (blk_no < blk_cnt || blk_cnt == 1)
    {
        word_write(SDR_BUF_TRAN_CTRL_REG, ((blk_cnt << 16) | BUF_XFER_START));
        null_delay(1);
    }

    word_write(SDR_DMA_CTRCH1_REG,0x3F);
    word_write(SDR_DMA_CTRCH1_REG,0x33);

    return 0;
}

int CardRead_single_pin(unsigned int buf_dest_addr, unsigned int cmd_arg, int blk_num)
{
    unsigned int i, k;
    int cur_blk=0;
    int single = 1;

    if (blk_num > 1)
        single = 0;
    for(i=0; i<blk_num; i++)
    {
        WaitBusReady(0x0c);
        cur_blk = cmd_arg + i;

        if(single)
            word_write(SDR_Card_BLOCK_SET_REG,(0x00000100|0x1));
        else
            word_write(SDR_Card_BLOCK_SET_REG,(0x00000100|blk_num));

        word_write(SDR_DMA_TRAN_RESP_REG, 0x1f);
        word_write(SDR_DMA_INTS_REG, 0x3);				

        if(single)
        {
            if(HCmdNoData(((cur_blk>>24)&0xff), ((cur_blk>>16)&0xff), ((cur_blk>>8)&0xff), (cur_blk&0xff), 0x11, 0x79, 0x11))
                goto fail;

            if(WaitResp(SD_XFER_DONE, 1000))		
                goto fail;

            ReadDma(i, 0x1, buf_dest_addr);				
            WaitDMAIntr(DMA_CH1_INTR);			
        }
        else
        {
            if(HCmdNoData(((cmd_arg>>24)&0xff), ((cmd_arg>>16)&0xff), ((cmd_arg>>8)&0xff), (cmd_arg&0xff), 0x12, 0x7B, 0x11))
                goto fail;


            for(k=0; k<blk_num-1; k++)
            {
                if(WaitResp(SD_DATA_BOUND, 500))			
                    goto fail;

                word_write(SDR_SPECIAL_CTRL_REG, 0x1);		

                ReadDma(k, 1, buf_dest_addr);	

                if(WaitResp(FIFO_XFER_DONE, 100))			
                    goto fail;

                WaitDMAIntr(2);			
            }

            if(WaitResp(SD_XFER_DONE, 100))			
                goto fail;

            if(WaitResp(SD_CMD_DONE, 100))  			
                goto fail;

            ReadDma(k, 0x1, buf_dest_addr);


            WaitDMAIntr(2);                          	

            goto strdata;
        }

    } 

strdata:

    word_write(SDR_DMA_TRAN_RESP_REG,(FIFO_XFER_DONE|SD_XFER_DONE));
    word_write(SDR_DMA_INTS_REG, word_read(SDR_DMA_INTS_REG));
    return 0;
fail:

    word_write(SDR_DMA_TRAN_RESP_REG,(FIFO_XFER_DONE|SD_XFER_DONE));
    word_write(SDR_DMA_INTS_REG, word_read(SDR_DMA_INTS_REG));
    return 1;
}
#endif
//------------------------------------------------------------------------------
int HCmdReadData(unsigned int CMD_ARG3,unsigned int CMD_ARG2, unsigned int CMD_ARG1, unsigned int CMD_ARG0, unsigned int CMD, unsigned int CTRL, unsigned int reader_set,
                 unsigned int buf, int blk_cnt, int blk_size)
{
    unsigned int flag=0;
    unsigned int CMD_ARG;

    memset((void *)buf, 0xff, blk_cnt * blk_size);

    CMD_ARG = (CMD_ARG3 << 24) |  (CMD_ARG2 << 16) | (CMD_ARG1 << 8) | CMD_ARG0;
    word_write(SDR_CMD_ARGUMENT_REG, CMD_ARG);

    flag = flag | (CTRL << 8) | (CMD << 16) | reader_set | EN_INTR;   
    word_write(SDR_CTRL_REG, flag);


    if(WaitResp(SD_CMD_DONE, 10))
        goto fail;
    else if((CTRL & 0xC) == 0)		
        goto pass;

    RSP1 = word_read(SDR_RESPONSE1_REG);

    WaitBusReady(0x0c);

    word_write(SDR_DMA_DACH1_REG,  buf);
    word_write(SDR_DMA_TCCH1_REG,  blk_size);
    word_write(SDR_DMA_CTRCH1_REG, 0x3F);
    word_write(SDR_DMA_CTRCH1_REG, 0x33);
    word_write(SDR_BUF_TRAN_CTRL_REG, ((blk_cnt << 16)| BUF_XFER_START));

    if(WaitResp(FIFO_XFER_DONE, 0))		
        goto fail;

    WaitDMAIntr(DMA_CH1_INTR);			

    dump_buffer((unsigned char *)buf, blk_size);

pass:
    return 0;

fail:
    return 1;
}
//------------------------------------------------------------------------------
int HCmdNoData(unsigned int CMD_ARG3,unsigned int CMD_ARG2, unsigned int CMD_ARG1, unsigned int CMD_ARG0, unsigned int CMD, unsigned int CTRL, unsigned int reader_set)
{
    unsigned int flag=0;
    unsigned int CMD_ARG;

    word_write(SDR_DMA_TRAN_RESP_REG, word_read(SDR_DMA_TRAN_RESP_REG));

    CMD_ARG = (CMD_ARG3 << 24) |  (CMD_ARG2 << 16) | (CMD_ARG1 << 8) | CMD_ARG0;
    word_write(SDR_CMD_ARGUMENT_REG, CMD_ARG);

    flag = flag | (CTRL << 8) | (CMD << 16) | reader_set | EN_INTR; 
    word_write(SDR_CTRL_REG, flag);



    if(WaitResp(SD_CMD_DONE, 10))
    {
    	if (CMD == 8)
    	{
    		word_write(SDSW_SW_CTRL0, 1<<2);
    		goto fail;
		}
		else
    		goto fail;
	}
    else if((CTRL & 0xC) == 0)	
        goto pass;

#if 1
    RSP1 = word_read(SDR_RESPONSE1_REG);
#endif

pass:
    if((CMD==6) && (CMD_ARG==2))
		word_write(0xa000a0c8, 0x2);
    return 0;

fail:
    return 1;
}

//------------------------------------------------------------------------------

int init_m2(void)
{
	volatile int timeout = 100000;

	word_write(0xa000a01c, 0x8);
    while ((word_read(0xa000a000) & 0x1) == 0x1)
    {
    	word_write(0xa000a01c, 0x8);
        word_write(0xa000a000, 0x0);

        if (timeout-- <= 0)
        {
        	return -1;
		}
    }

    word_write(0xa000a004, 0x1);
    return 0;
}

//------------------------------------------------------------------------------
int command_55_get_scr_0()
{
    word_write(SDR_Card_BLOCK_SET_REG, 0x00080001);          
    word_write(SDR_Error_Enable_REG, 0x6f);                  
    if(HCmdNoData(RCA1, RCA0, 0x00, 0x00, 0x37, 0x38, 0x11))
         return -1;

    if(HCmdNoData(0x00, 0x00, 0x00, 0x00, 0x33, 0x79, 0x11))  
         return -1;

    if(WaitResp(SD_XFER_DONE, 0))                           
         return -1;

    word_write(SDR_DMA_TCCH1_REG, 0x8);
    word_write(SDR_DMA_DACH1_REG, DMA_RADDR);
    word_write(SDR_DMA_CTRCH1_REG, 0x3F);
    word_write(SDR_DMA_CTRCH1_REG, 0x33);
    word_write(SDR_BUF_TRAN_CTRL_REG, 0x00010000 | BUF_XFER_START);

    if(WaitResp(FIFO_XFER_DONE, 0))                        
       return -1;


	return 0;
}
//------------------------------------------------------------------------------
int command_55_get_scr()
{
    word_write(SDR_Card_BLOCK_SET_REG, 0x00080001);          
    word_write(SDR_Error_Enable_REG, 0x6f);                  
    if(HCmdNoData(RCA1, RCA0, 0x00, 0x00, 0x37, 0x38, 0x11))
         return -1;

    if(HCmdNoData(0x00, 0x00, 0x00, 0x00, 0x33, 0x79, 0x11)) 
         return -1;

    if(WaitResp(SD_XFER_DONE, 0))                            
         return -1;

    word_write(SDR_DMA_TCCH1_REG, 0x8);
    word_write(SDR_DMA_DACH1_REG, DMA_RADDR);
    word_write(SDR_DMA_CTRCH1_REG, 0x3F);

    word_write(SDR_DMA_CTRCH1_REG, 0x33);
    word_write(SDR_BUF_TRAN_CTRL_REG, 0x00010000 | BUF_XFER_START);

    if(WaitResp(FIFO_XFER_DONE, 0))                        
       return -1;


    word_write(SDR_DMA_CTRCH1_REG, 0x33);
	return 0;
}


//------------------------------------------------------------------------------
int ReInitCard()
{

    init_m2();

    if(HCmdNoData(0x00, 0x00, 0x00, 0x00, 0x00, 0x30,0x11))


    RCA0 = 0;
    RCA1 = 0;

    if(HCmdNoData(0x00, 0x00, 0x01, 0xaa, 0x08, 0x38, 0x11))
    {
		if(HCmdNoData(0x00, 0x00, 0x01, 0xaa, 0x08, 0x38, 0x11))
    	{
        	if(InCaIsSDVer(1,0))
            goto fail;
        }
	    else
	    {
	        if(InCaIsSDVer(2, 0x40))
	            goto fail;
	    }
    }
    else
    {
        if(InCaIsSDVer(2, 0x40))
            goto fail;
    }

    if(HCmdNoData(0x00, 0x00, 0x00, 0x00, 0x02, 0x34, 0x11))
        goto fail;

    if(HCmdNoData(0x00, 0x00, 0x00, 0x00, 0x03, 0x38, 0x11))
        goto fail;

    RCA0 = (word_read(SDR_RESPONSE1_REG) & 0x00ff0000) >> 16;
    RCA1 = (word_read(SDR_RESPONSE1_REG) & 0xff000000) >> 24;


    if(HCmdNoData(RCA1, RCA0, 0x00, 0x00, 0x07, 0x3C, 0x11))
        goto fail;
    else
        WaitBusReady(0x8c);

    if(HCmdNoData(RCA1, RCA0, 0x00, 0x00, 0x37, 0x38, 0x11))
		goto fail;

    if(HCmdNoData(0x00, 0x00, 0x00, 0x02, 0x06, 0x38, 0x11))
		goto fail;

    return 0;
fail:
    return 1;
}

int test()
{

    return 1;
}
//------------------------------------------------------------------------------
int InitCard()
{

    if (init_m2() == -1)
        return 0;

    if(HCmdNoData(0x00, 0x00, 0x00, 0x00, 40, 0x30,0x11))

    RCA0 = 0;
    RCA1 = 0;
    if(HCmdNoData(0x00, 0x00, 0x01, 0xaa, 0x08, 0x38, 0x11))
    {
		if(HCmdNoData(0x00, 0x00, 0x01, 0xaa, 0x08, 0x38, 0x11))
    	{
        	if(InCaIsSDVer(1,0))
            goto fail;
        }
	    else
	    {
	        if(InCaIsSDVer(2, 0x40))
	            goto fail;
	    }
    }
    else
    {
        if(InCaIsSDVer(2, 0x40))
            goto fail;
    }

    if(HCmdNoData(0x00, 0x00, 0x00, 0x00, 0x02, 0x34, 0x11))
        goto fail;

    if(HCmdNoData(0x00, 0x00, 0x00, 0x00, 0x03, 0x38, 0x11))
        goto fail;

    RCA0 = (word_read(SDR_RESPONSE1_REG) & 0x00ff0000) >> 16;
    RCA1 = (word_read(SDR_RESPONSE1_REG) & 0xff000000) >> 24;

    if(HCmdNoData(RCA1, RCA0, 0x00, 0x00, 0x07, 0x3C, 0x11))
        goto fail;
    else
        WaitBusReady(0x8c);

    if(HCmdNoData(RCA1, RCA0, 0x00, 0x00, 0x37, 0x38, 0x11))
		goto fail;

    if(HCmdNoData(0x00, 0x00, 0x00, 0x02, 0x06, 0x38, 0x11))
		goto fail;
    return 0;
fail:

    return 1;
}

//------------------------------------------------------------------------------
void InitCardReader()
{

    word_write(SDR_DMA_TCCH0_REG, XFER_COUNT);
    word_write(SDR_DMA_TCCH1_REG, XFER_COUNT);

    word_write(SDR_Card_BLOCK_SET_REG,(0x00000100|BLK_CNT));
    word_write(SDR_Error_Enable_REG, 0x6f);			
}
//------------------------------------------------------------------------------

