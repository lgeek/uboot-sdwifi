
void InitCardReader(void);
int CardRead_single_pin(unsigned int buf_dest_addr, unsigned int blk_no,int blk_num );
int CardRead_dma(unsigned int buf_dest_addr, unsigned int blk_no, int blk_order);
int CardErase_single(unsigned int blk_no);
void data_compare(int blk_order);
void write_512_buffer(unsigned int dest_addr, unsigned int DMA_RADDR_start);
int InitCard(void);
int WaitCardReady(void);
void WaitBusReady(unsigned int expStatus);
int InCaIsSDVer(int Ver, int CCap);
int ACMD41tillReady(int CCap);
int HCmdNoData(unsigned int CMD_ARG3,unsigned int CMD_ARG2, unsigned int CMD_ARG1,unsigned  int CMD_ARG0, unsigned int CMD, unsigned int CTRL, unsigned int reader_set);
int WaitResp(unsigned int expResp, int delay);
void WaitDMAIntr(unsigned int channel);

void show_str(char *start,unsigned int input,char *end, int end_len);
void sim_uart_tx(char tmp);

int CardWrite_single_pin(unsigned int blk_no, int blk_num);
int CardWrite_cmd(unsigned int blk_no, int blk_order);
