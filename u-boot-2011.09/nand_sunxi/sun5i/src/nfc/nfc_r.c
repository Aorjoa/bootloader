/*********************************************************************************
*                                           NAND FLASH DRIVER
*								(c) Copyright 2008, SoftWinners Co,Ld.
*                                          All Right Reserved
*file : nfc_r.c
*description : this file provides some physic functions for upper nand driver layer.
*history :
*	v0.1  2008-03-26 Richard
*	        offer direct accsee read method to nand flash control machine.
*   v0.2  2009.09.09 penggang
**********************************************************************************/
#include "../include/nfc.h"

__u32	nand_board_version=0;
__u32 	pagesize=0;
__u32   nand_io_base=0;

volatile __u32 irq_value=0;


__u8 read_retry_reg_adr[READ_RETRY_MAX_REG_NUM]={0};
__u8 read_retry_default_val[MAX_CHIP_SELECT_CNT][READ_RETRY_MAX_REG_NUM]={0};
__s16 read_retry_val[READ_RETRY_MAX_CYCLE][READ_RETRY_MAX_REG_NUM]={0};
__u8 hynix_read_retry_otp_value[MAX_CHIP_SELECT_CNT][8][8]={0};
__u8 read_retry_mode=0;
__u8 read_retry_cycle=0;
__u8 read_retry_reg_num=0;

const __s16 para0[6][4] = {	{0x00,  0x06,  0x0A,  0x06},
    						{0x00, -0x03, -0x07, -0x08},
    						{0x00, -0x06, -0x0D, -0x0F},
    						{0x00, -0x0B, -0x14, -0x17},
    						{0x00,  0x00, -0x1A, -0x1E},
    						{0x00,  0x00, -0x20, -0x25}
					};
const __s16 para1[6][4] = {	{0x00,  0x06,  0x0a,  0x06},
    						{0x00, -0x03, -0x07, -0x08},
    						{0x00, -0x06, -0x0d, -0x0f},
    						{0x00, -0x09, -0x14, -0x17},
    						{0x00,  0x00, -0x1a, -0x1e},
    						{0x00,  0x00, -0x20, -0x25}
					};

const   __s16 para0x10[5] = {0x04, 0x7c, 0x78, 0x74, 0x08};

const __s16 para0x11[7][5] = {	{0x04,  0x04,  0x7c,  0x7e, 0x00},
    						{0x00, 0x7c, 0x78, 0x78, 0x00},
    						{0x7c, 0x76, 0x74, 0x72, 0x00},
    						{0x08, 0x08, 0x00, 0x00, 0x00},
    						{0x0b, 0x7e, 0x76, 0x74, 0x00},
    						{0x10, 0x76, 0x72, 0x70, 0x00},
    						{0x02, 0x00, 0x7e, 0x7c, 0x00},
					};
const   __s16 para0x20[15][4] ={{0x00, 0x00, 0x00, 0x00},    //0
                         {0x05, 0x0A, 0x00, 0x00},    //1
                         {0x28, 0x00, 0xEC, 0xD8},    //2
                         {0xED, 0xF5, 0xED, 0xE6},    //3
                         {0x0A, 0x0F, 0x05, 0x00},    //4
                         {0x0F, 0x0A, 0xFB, 0xEC},    //5
                         {0xE8, 0xEF, 0xE8, 0xDC},    //6
                         {0xF1, 0xFB, 0xFE, 0xF0},    //7
                         {0x0A, 0x00, 0xFB, 0xEC},    //8
                         {0xD0, 0xE2, 0xD0, 0xC2},    //9
                         {0x14, 0x0F, 0xFB, 0xEC},    //10
                         {0xE8, 0xFB, 0xE8, 0xDC},    //11
                         {0x1E, 0x14, 0xFB, 0xEC},    //12
                         {0xFB, 0xFF, 0xFB, 0xF8},    //13
                         {0x07, 0x0C, 0x02, 0x00}     //14
	                    };

const	__s16 param0x30low[16][2] ={{0xF0,0XF0},
									{0xE0,0XE0},
									{0xD0,0XD0},
									{0x10,0X10},
									{0x20,0X20},
									{0x30,0X30},
									{0xC0,0XD0},
									{0x00,0X10},
									{0x00,0X20},
									{0x10,0X20},
									{0xB0,0XD0},
									{0xA0,0XD0},
									{0x90,0XD0},
									{0xB0,0XC0},
									{0xA0,0XC0},
									{0x90,0XC0}
									};
const	__s16 param0x30high[20][2] ={{0x00,0XF0},
									{0x0F,0XE0},
									{0x0F,0XD0},
									{0x0E,0XE0},
									{0x0E,0XD0},
									{0x0D,0XF0},
									{0x0D,0XE0},
									{0x0D,0XD0},
									{0x01,0X10},
									{0x02,0X20},
									{0x02,0X10},
									{0x03,0X20},
									{0x0F,0X00},
									{0x0E,0XF0},
									{0x0D,0XC0},
									{0x0F,0XF0},
									{0x01,0X00},
									{0x02,0X00},
									{0x0D,0XB0},
									{0x0C,0XA0}
									};
const	__s16 param0x31[9][3] =	   {{0x00,0XF0,0x00},
									{0x00,0XE0,0x00},
									{0xFF,0XF0,0xF0},
									{0xEE,0XE0,0xE0},
									{0xDE,0XD0,0xD0},
									{0xCD,0XC0,0xC0},
									{0x01,0X00,0x00},
									{0x02,0X00,0x00},
									{0x03,0X00,0x00},
									};
const	__s16 param0x32[19][4] ={{0x04,0x00,0x7C,0x7C},
								 {0x08,0x00,0x04,0x04},
								 {0x7C,0x7C,0x00,0x7C},
								 {0x7C,0x7C,0x00,0x00},
								 {0x0C,0x00,0x78,0x78},
								 {0x10,0x00,0x08,0x08},
								 {0x7C,0x7C,0x78,0x78},
								 {0x7C,0x7C,0x04,0x04},
								 {0x7C,0x7C,0x78,0x74},
								 {0x14,0x04,0x04,0x00},
								 {0x78,0x78,0x00,0x7C},
								 {0x78,0x78,0x78,0x78},
								 {0x78,0x78,0x78,0x74},
								 {0x78,0x78,0x04,0x00},
								 {0x78,0x74,0x78,0x74},
								 {0x78,0x74,0x74,0x70},
								 {0x78,0x74,0x78,0x78},
								 {0x78,0x70,0x78,0x74},
								 {0x78,0x70,0x70,0x70}
									};
const	__s16 param0x40[10] = {0x0,0x0,0x0,0x1,0x2,0x3,0x04,0x05,0x06,0x07};						

const	__s16 param0x50[7] = {0x1,0x2,0x3,0x0,0x1,0x2,0x3};	


__u32 ddr_param[8]={0};


static void dumphex32(char* name, char* base, int len)
{
	__u32 i;

	PRINT("dump %s registers:", name);
	for (i=0; i<len*4; i+=4) {
		if (!(i&0xf))
			PRINT("\n0x%p : ", base + i);
		PRINT("0x%x ", *((volatile unsigned int *)(base + i)));
	}
	PRINT("\n");
}


static void dumpreg()
{
	
	dumphex32("nand0 reg", (char*)0x01c03000, 60);
	
	dumphex32("gpio reg", (char*)0x01c20848, 50);
	
	dumphex32("dma reg", (char*)0x01c02300, 50);

	dumphex32("clk reg", (char*)0x01c00080, 10);

}


void NFC_InitDDRParam(__u32 chip, __u32 param)
{
    if(chip<8)
        ddr_param[chip] = param;
}

void nfc_repeat_mode_enable(void)
{
    __u32 reg_val;


	reg_val = NFC_READ_REG(NFC_REG_CTL);
	if(((reg_val>>18)&0x3)>1)   //ddr type
	{
    	reg_val |= 0x1<<20;
    	NFC_WRITE_REG(NFC_REG_CTL, reg_val);
    }

}

void nfc_repeat_mode_disable(void)
{
    __u32 reg_val;

    reg_val = NFC_READ_REG(NFC_REG_CTL);
	if(((reg_val>>18)&0x3)>1)   //ddr type
	{
    	reg_val &= (~(0x1<<20));
    	NFC_WRITE_REG(NFC_REG_CTL, reg_val);
    }
}

/*******************wait nfc********************************************/
__s32 _wait_cmdfifo_free(void)
{
	__s32 timeout = 0xffff;

	while ( (timeout--) && (NFC_READ_REG(NFC_REG_ST) & NFC_CMD_FIFO_STATUS) );
	if (timeout <= 0)
	{
	    PRINT("nand _wait_cmdfifo_free time out, status:0x%x\n", NFC_READ_REG(NFC_REG_ST));
		dumpreg();
		return -ERR_TIMEOUT;
    }
	return 0;
}

__s32 _wait_cmd_finish(void)
{
	__s32 timeout = 0xffff;
	while( (timeout--) && !(NFC_READ_REG(NFC_REG_ST) & NFC_CMD_INT_FLAG) );
	if (timeout <= 0)
	{
	    PRINT("nand _wait_cmd_finish time out, status:0x%x\n", NFC_READ_REG(NFC_REG_ST));
		dumpreg();
		return -ERR_TIMEOUT;
    }
	NFC_WRITE_REG(NFC_REG_ST, NFC_READ_REG(NFC_REG_ST) & NFC_CMD_INT_FLAG);
	return 0;
}

void _dma_config_start(__u8 rw, __u32 buff_addr, __u32 len)
{
	NAND_DMAConfigStart(rw, buff_addr, len);
}

__s32 _wait_dma_end(void)
{
	__s32 timeout = 0xffff;

	while( (timeout--) && ( NAND_QueryDmaStat()) );
	if (timeout <= 0)
	{
	    PRINT("nand _wait_dma_end time out, status:0x%x\n", NFC_READ_REG(NFC_REG_ST));

		dumpreg();
		return -ERR_TIMEOUT;
    }

	return 0;
}

__s32 _reset(void)
{
	__u32 cfg;

	__s32 timeout = 0xffff;

	/*reset NFC*/
	cfg = NFC_READ_REG(NFC_REG_CTL);
	cfg |= NFC_RESET;
	NFC_WRITE_REG(NFC_REG_CTL, cfg);
	//waiting reset operation end
	while((timeout--) && (NFC_READ_REG(NFC_REG_CTL) & NFC_RESET));
	if (timeout <= 0)
	{
	    PRINT("nand _reset time out, status:0x%x\n", NFC_READ_REG(NFC_REG_ST));
		return -ERR_TIMEOUT;
    }
	return 0;
}

/***************ecc function*****************************************/
__s32 _check_ecc(__u32 eblock_cnt)
{
	__u32 i;
	__u32 ecc_mode;
	__u32 max_ecc_bit_cnt = 16;
	__u32 cfg;
	__u32 ecc_cnt_w[4];
	__u8 *ecc_cnt;
	__u8 ecc_tab[9] = {16, 24, 28, 32, 40, 48, 56, 60, 64};

	ecc_mode = (NFC_READ_REG(NFC_REG_ECC_CTL)>>12)&0xf;
	max_ecc_bit_cnt = ecc_tab[ecc_mode];

	//check ecc errro
	cfg = NFC_READ_REG(NFC_REG_ECC_ST)&0xffff;
	for (i = 0; i < eblock_cnt; i++)
	{
		if (cfg & (1<<i))
			return -ERR_ECC;
	}

    //check ecc limit
    ecc_cnt_w[0]= NFC_READ_REG(NFC_REG_ECC_CNT0);
    ecc_cnt_w[1]= NFC_READ_REG(NFC_REG_ECC_CNT1);
    ecc_cnt_w[2]= NFC_READ_REG(NFC_REG_ECC_CNT2);
    ecc_cnt_w[3]= NFC_READ_REG(NFC_REG_ECC_CNT3);

    ecc_cnt = (__u8 *)((__u32)(ecc_cnt_w));
	for (i = 0; i < eblock_cnt; i++)
	{
		if((max_ecc_bit_cnt - 4) <= ecc_cnt[i])
			return ECC_LIMIT;
	}

	return 0;
}

void _disable_ecc(void)
{
	__u32 cfg = NFC_READ_REG(NFC_REG_ECC_CTL);
	cfg &= ( (~NFC_ECC_EN)&0xffffffff );
	NFC_WRITE_REG(NFC_REG_ECC_CTL, cfg);
}

void _enable_ecc(__u32 pipline)
{
	__u32 cfg = NFC_READ_REG(NFC_REG_ECC_CTL);
	if (pipline ==1 )
		cfg |= NFC_ECC_PIPELINE;
	else
		cfg &= ((~NFC_ECC_PIPELINE)&0xffffffff);


	/*after erased, all data is 0xff, but ecc is not 0xff,
			so ecc asume it is right*/
	//if random open, disable exception
	if(cfg&(0x1<<9))
	    cfg &= (~(0x1<<4));
	else
	    cfg |= (1 << 4);

	//cfg |= (1 << 1); 16 bit ecc

	cfg |= NFC_ECC_EN;
	NFC_WRITE_REG(NFC_REG_ECC_CTL, cfg);
}

__s32 _enter_nand_critical(void)
{
	return 0;
}

__s32 _exit_nand_critical(void)
{
	return 0;
}

void _set_addr(__u8 *addr, __u8 cnt)
{
	__u32 i;
	__u32 addr_low = 0;
	__u32 addr_high = 0;

	for (i = 0; i < cnt; i++){
		if (i < 4)
			addr_low |= (addr[i] << (i*8) );
		else
			addr_high |= (addr[i] << ((i - 4)*8));
	}

	NFC_WRITE_REG(NFC_REG_ADDR_LOW, addr_low);
	NFC_WRITE_REG(NFC_REG_ADDR_HIGH, addr_high);
}

__s32 _read_in_page_mode(NFC_CMD_LIST  *rcmd,void *mainbuf,void *sparebuf,__u8 dma_wait_mode)
{
	__s32 ret,ret1;
	__s32 i;
	__u32 cfg;
	NFC_CMD_LIST *cur_cmd,*read_addr_cmd;
	__u32 read_data_cmd,random_read_cmd0,random_read_cmd1;

	ret = 0;
	read_addr_cmd = rcmd;
	cur_cmd = rcmd;
	cur_cmd = cur_cmd->next;
	random_read_cmd0 = cur_cmd->value;
	cur_cmd = cur_cmd->next;
	random_read_cmd1 = cur_cmd->value;
	cur_cmd = cur_cmd->next;
	read_data_cmd = cur_cmd->value;

	//access NFC internal RAM by DMA bus
	NFC_WRITE_REG(NFC_REG_CTL, (NFC_READ_REG(NFC_REG_CTL)) | NFC_RAM_METHOD);

	/*set dma and run*/
	_dma_config_start(0, (__u32)mainbuf, pagesize);

	/*wait cmd fifo free*/
	ret = _wait_cmdfifo_free();
	if (ret)
		return ret;

	/*set NFC_REG_CNT*/
	NFC_WRITE_REG(NFC_REG_CNT,1024);

	/*set NFC_REG_RCMD_SET*/
	cfg = 0;
	cfg |= (read_data_cmd & 0xff);
	cfg |= ((random_read_cmd0 & 0xff) << 8);
	cfg |= ((random_read_cmd1 & 0xff) << 16);
	NFC_WRITE_REG(NFC_REG_RCMD_SET, cfg);

	/*set NFC_REG_SECTOR_NUM*/
	NFC_WRITE_REG(NFC_REG_SECTOR_NUM, pagesize/1024);

	/*set addr*/
	_set_addr(read_addr_cmd->addr,read_addr_cmd->addr_cycle);

	/*set NFC_REG_CMD*/
	cfg  = 0;
	cfg |= read_addr_cmd->value;
	/*set sequence mode*/
	//cfg |= 0x1<<25;
	cfg |= ( (read_addr_cmd->addr_cycle - 1) << 16);
	cfg |= (NFC_SEND_ADR | NFC_DATA_TRANS | NFC_SEND_CMD1 | NFC_SEND_CMD2 | NFC_WAIT_FLAG | NFC_DATA_SWAP_METHOD);
	cfg |= ((__u32)0x2 << 30);//page command

	if (pagesize/1024 == 1)
		cfg |= NFC_SEQ;

	/*enable ecc*/
	_enable_ecc(1);
	NFC_WRITE_REG(NFC_REG_CMD,cfg);

    NAND_WaitDmaFinish();

	/*wait cmd fifo free and cmd finish*/
	ret = _wait_cmdfifo_free();
	ret |= _wait_cmd_finish();
	if (ret){
		_disable_ecc();
		return ret;
	}
	/*get user data*/
	for (i = 0; i < pagesize/1024;  i++){
		*(((__u32*) sparebuf)+i) = NFC_READ_REG(NFC_REG_USER_DATA(i));
	}

	/*ecc check and disable ecc*/
	ret = _check_ecc(pagesize/1024);
	_disable_ecc();

	/*if dma mode is wait*/
	if(0 == dma_wait_mode){
		ret1 = _wait_dma_end();
		if (ret1)
			return ret1;
	}

	return ret;
}

/*******************************************************************************
*								NFC_Read
*
* Description 	: read some sectors data from flash in single plane mode.
* Arguments	: *rcmd	-- the read command sequence list head��
*			  *mainbuf	-- point to data buffer address, 	it must be four bytes align.
*                     *sparebuf	-- point to spare buffer address.
*                     dma_wait_mode	-- how to deal when dma start, 0 = wait till dma finish,
							    1 = dma interrupt was set and now sleep till interrupt occurs.
*			  page_mode  -- 0 = normal command, 1 = page mode
* Returns		: 0 = success.
			  1 = success & ecc limit.
			  -1 = too much ecc err.
* Notes		:  if align page data required��page command mode is used., if the commands do
			   not fetch data��ecc is not neccesary.
********************************************************************************/
__s32 NFC_Read(NFC_CMD_LIST  *rcmd, void *mainbuf, void *sparebuf, __u8 dma_wait_mode,__u8 page_mode )
{

	__u32 ret ;

	_enter_nand_critical();

	ret = _read_in_page_mode(rcmd, mainbuf,sparebuf, dma_wait_mode);

	/*switch to ahb*/
	NFC_WRITE_REG(NFC_REG_CTL, (NFC_READ_REG(NFC_REG_CTL)) & (~NFC_RAM_METHOD));

	_exit_nand_critical();


	return ret;
}


/*finish the comand list */
__s32 nfc_set_cmd_register(NFC_CMD_LIST *cmd)
{
	__u32 cfg;
	__s32 ret;

	NFC_CMD_LIST *cur_cmd = cmd;
	while(cur_cmd != NULL){
		/*wait cmd fifo free*/
		ret = _wait_cmdfifo_free();
		if (ret)
			return ret;

		cfg = 0;
		/*set addr*/
		if (cur_cmd->addr_cycle){
			_set_addr(cur_cmd->addr,cur_cmd->addr_cycle);
			cfg |= ( (cur_cmd->addr_cycle - 1) << 16);
			cfg |= NFC_SEND_ADR;
		}

		/*set NFC_REG_CMD*/
		/*set cmd value*/
		cfg |= cur_cmd->value;
		/*set sequence mode*/
		//cfg |= 0x1<<25;
		/*wait rb?*/
		if (cur_cmd->wait_rb_flag){
			cfg |= NFC_WAIT_FLAG;
		}
		if (cur_cmd->data_fetch_flag){
			NFC_WRITE_REG(NFC_REG_CTL, (NFC_READ_REG(NFC_REG_CTL)) & (~NFC_RAM_METHOD));
			cfg |= NFC_DATA_TRANS;
			NFC_WRITE_REG(NFC_REG_CNT, cur_cmd->bytecnt);
		}
		/*send command*/
		cfg |= NFC_SEND_CMD1;
		NFC_WRITE_REG(NFC_REG_CMD, cfg);
		cur_cmd = cur_cmd ->next;
	}
	return 0;
}

__s32 NFC_SetRandomSeed(__u32 random_seed)
{
	__u32 cfg;


	  cfg = NFC_READ_REG(NFC_REG_ECC_CTL);
	  cfg &= 0x0000ffff;
	  cfg |= (random_seed<<16);
	  NFC_WRITE_REG(NFC_REG_ECC_CTL,cfg);

	return 0;
}

__s32 NFC_RandomEnable(void)
{
	__u32 cfg;


	cfg = NFC_READ_REG(NFC_REG_ECC_CTL);
	cfg |= (0x1<<9);
	NFC_WRITE_REG(NFC_REG_ECC_CTL,cfg);


	return 0;
}

__s32 NFC_RandomDisable(void)
{
	__u32 cfg;


	cfg = NFC_READ_REG(NFC_REG_ECC_CTL);
	cfg &= (~(0x1<<9));
	NFC_WRITE_REG(NFC_REG_ECC_CTL,cfg);


	return 0;
}



/*******************************************************************************
*								NFC_GetId
*
* Description 	: get chip id.
* Arguments	: *idcmd	-- the get id command sequence list head.

* Returns		: 0 = success.
			  -1 = fail.
* Notes		:
********************************************************************************/
__s32 NFC_GetId(NFC_CMD_LIST  *idcmd ,__u8 *idbuf)
{
	__u32 i;
	__s32 ret;

	_enter_nand_critical();

    nfc_repeat_mode_enable();
	ret = nfc_set_cmd_register(idcmd);
	if (ret){
		_exit_nand_critical();
		return ret;
	}

	ret = _wait_cmdfifo_free();
	ret |= _wait_cmd_finish();

	/*get 5 bytes id value*/
	for (i = 0; i < 6; i++){
		*(idbuf + i) = NFC_READ_RAM_B(NFC_RAM0_BASE+i);
	}

    nfc_repeat_mode_disable();

	_exit_nand_critical();
	return ret;
}


/*******************************************************************************
*								NFC_GetStatus
*
* Description 	: get status.
* Arguments	: *scmd	-- the get status command sequence list head.

* Returns		: status result
* Notes		: some cmd must be sent with addr.
********************************************************************************/
__s32 NFC_GetStatus(NFC_CMD_LIST  *scmd)
{
	__s32 ret;

	_enter_nand_critical();
	nfc_repeat_mode_enable();
	ret = nfc_set_cmd_register(scmd);
	if (ret){
		_exit_nand_critical();
		return ret;
	}

	ret = _wait_cmdfifo_free();
	ret |= _wait_cmd_finish();
	if(ret){
		_exit_nand_critical();
		return ret;
	}

    nfc_repeat_mode_disable();
	_exit_nand_critical();
	return (NFC_READ_RAM_B(NFC_RAM0_BASE));

}
/*******************************************************************************
*								NFC_ResetChip
*
* Description 	: reset nand flash.
* Arguments	: *reset_cmd	-- the reset command sequence list head.

* Returns		: sucess or fail
* Notes		:
********************************************************************************/
__s32 NFC_ResetChip(NFC_CMD_LIST *reset_cmd)

{
	__s32 ret;

	_enter_nand_critical();

	ret = nfc_set_cmd_register(reset_cmd);
	if (ret){
		_exit_nand_critical();
		return ret;
	}
	ret = _wait_cmdfifo_free();
	ret |= _wait_cmd_finish();
	_exit_nand_critical();
	return ret;
}

/*******************************************************************************
*								NFC_SelectChip
*
* Description 	: enable chip ce.
* Arguments	: chip	-- chip no.

* Returns		: 0 = sucess -1 = fail
* Notes		:
********************************************************************************/
__s32 NFC_SelectChip( __u32 chip)
{
	__u32 cfg;


    cfg = NFC_READ_REG(NFC_REG_CTL);
    cfg &= ( (~NFC_CE_SEL) & 0xffffffff);
    cfg |= ((chip & 0x7) << 24);
#if 0
    if(((read_retry_mode == 0)||(read_retry_mode == 1))&&(read_retry_cycle))
        cfg |= (0x1<<6);
#endif
    NFC_WRITE_REG(NFC_REG_CTL,cfg);

    if((cfg>>18)&0x3) //ddr nand
    {
        //set ddr param
        NFC_WRITE_REG(NFC_REG_TIMING_CTL,ddr_param[0]);
    }

	return 0;
}

/*******************************************************************************
*								NFC_SelectRb
*
* Description 	: select rb.
* Arguments	: rb	-- rb no.

* Returns		: 0 = sucess -1 = fail
* Notes		:
********************************************************************************/
__s32 NFC_SelectRb( __u32 rb)
{
	__s32 cfg;


	  cfg = NFC_READ_REG(NFC_REG_CTL);
	  cfg &= ( (~NFC_RB_SEL) & 0xffffffff);
	  cfg |= ((rb & 0x1) << 3);
	  NFC_WRITE_REG(NFC_REG_CTL,cfg);

	  return 0;

}





__s32 NFC_DeSelectChip( __u32 chip)
{
#if 0
    __u32 cfg;

    if(((read_retry_mode == 0)||(read_retry_mode == 1))&&(read_retry_cycle))
    {
        cfg = NFC_READ_REG(NFC_REG_CTL);
        cfg &= (~(0x1<<6));
        NFC_WRITE_REG(NFC_REG_CTL,cfg);
    }
#endif
	return 0;
}

__s32 NFC_DeSelectRb( __u32 rb)
{





	return 0;
}


/*******************************************************************************
*								NFC_CheckRbReady
*
* Description 	: check rb if ready.
* Arguments	: rb	-- rb no.

* Returns		: 0 = sucess -1 = fail
* Notes		:
********************************************************************************/

__s32 NFC_CheckRbReady( __u32 rb)
{
	__s32 ret;
	__u32 cfg = NFC_READ_REG(NFC_REG_ST);


	cfg &= (NFC_RB_STATE0 << (rb & 0x3));

	if (cfg)
		ret = 0;
	else
		ret = -1;

	return ret;
}

/*******************************************************************************
*								NFC_ChangeMode
*
* Description 	: change serial access mode when clock change.
* Arguments	: nand_info -- structure with flash bus width,pagesize ,serial access mode and other configure parametre

* Returns		: 0 = sucess -1 = fail
* Notes		: NFC must be reset before seial access mode changes.
********************************************************************************/
__s32 NFC_ChangMode(NFC_INIT_INFO *nand_info )
{
	__u32 cfg;

	pagesize = nand_info->pagesize * 512;

	/*reset nfc*/
	_reset();

	/*set NFC_REG_CTL*/
	cfg = 0;
	cfg |= NFC_EN;
	cfg |= ( (nand_info->bus_width & 0x1) << 2);
	cfg |= ( (nand_info->ce_ctl & 0x1) << 6);
	cfg |= ( (nand_info->ce_ctl1 & 0x1) << 7);
	if(nand_info->pagesize == 2 )            /*  1K  */
	   cfg |= ( 0x0 << 8 );
	else if(nand_info->pagesize == 4 )       /*  2K  */
	   cfg |= ( 0x1 << 8 );
	else if(nand_info->pagesize == 8 )       /*  4K  */
	   cfg |= ( 0x2 << 8 );
    else if(nand_info->pagesize == 16 )       /*  8K  */
	   cfg |= ( 0x3 << 8 );
	else if(nand_info->pagesize == 32 )       /*  16K  */
	   cfg |= ( 0x4 << 8 );
	else                                      /* default 4K */
	   cfg |= ( 0x2 << 8 );
	cfg |= ((nand_info->ddr_type & 0x3) << 18);   //set ddr type
	cfg |= ((nand_info->debug & 0x1) << 31);
	NFC_WRITE_REG(NFC_REG_CTL,cfg);

	/*set NFC_TIMING */
	cfg = 0;
	if((nand_info->ddr_type & 0x3) == 0)
	    cfg |=((nand_info->serial_access_mode & 0x1) & 0xf)<<8;
	else if((nand_info->ddr_type & 0x3) == 2)
	{
	    cfg |= 0x3f;
	    cfg |= 0x3<<8;
    }
    else if((nand_info->ddr_type & 0x3) == 3)
	{
	    cfg |= 0x1f;
	    cfg |= 0x2<<8;
	}
	NFC_WRITE_REG(NFC_REG_TIMING_CTL,cfg);
	NFC_WRITE_REG(NFC_REG_TIMING_CFG,0xff);
	/*set NFC_SPARE_AREA */
	NFC_WRITE_REG(NFC_REG_SPARE_AREA, pagesize);

	return 0;
}

__s32 NFC_SetEccMode(__u8 ecc_mode)
{
    __u32 cfg = NFC_READ_REG(NFC_REG_ECC_CTL);


    cfg &=	((~NFC_ECC_MODE)&0xffffffff);
    cfg |= (NFC_ECC_MODE & (ecc_mode<<12));

	NFC_WRITE_REG(NFC_REG_ECC_CTL, cfg);

	return 0;
}
/*******************************************************************************
*								NFC_Init
*
* Description 	: init hardware, set NFC, set TIMING, request dma .
* Arguments	: nand_info -- structure with flash bus width,pagesize ,serial access mode and other configure parametre

* Returns		: 0 = sucess -1 = fail
* Notes		: .
********************************************************************************/
__s32 NFC_Init(NFC_INIT_INFO *nand_info )
{
	__s32 ret;
    __s32 i;

    PRINT("[NAND] nand driver(A%d) version: 0x%x, 0x%x\n",PLATFORM, NAND_VERSION_0, NAND_VERSION_1);
	
    //init ddr_param
    for(i=0;i<8;i++)
        ddr_param[i] = 0x21f;

    nand_io_base = (__u32)NAND_IORemap(NAND_IO_BASE_ADDR, 4096);

    //init clk
    NAND_ClkRequest();
    NAND_AHBEnable();
    NAND_SetClk(10);
    NAND_ClkEnable();

    //init pin
    NAND_PIORequest();

    //init dma
	NFC_SetEccMode(0);

	/*init nand control machine*/
	ret = NFC_ChangMode( nand_info);

	/*request special dma*/
	if (NAND_RequestDMA() < 0)
	{
	    PRINT("NAND_RequestDMA  fail\n");
	    return -1;
	}
	else
	{
	    //PRINT("NAND_RequestDMA  ok\n");
	}


	return ret;

}

/*******************************************************************************
*								NFC_Exit
*
* Description 	: free hardware resource, free dma , disable NFC.
* Arguments	: nand_info -- structure with flash bus width,pagesize ,serial access mode and other configure parametre

* Returns		: 0 = sucess -1 = fail
* Notes		: .
********************************************************************************/
void NFC_Exit( void )
{
	__u32 cfg;
	/*disable NFC*/
	cfg = NFC_READ_REG(NFC_REG_CTL);
	cfg &= ( (~NFC_EN) & 0xffffffff);
	NFC_WRITE_REG(NFC_REG_CTL,cfg);

	 //init clk
	NAND_ClkDisable();
    NAND_AHBDisable();
    NAND_ClkRelease();

    //init pin
    NAND_PIORelease();

    //init dma
    NAND_ReleaseDMA();
}

/*******************************************************************************
*								NFC_QueryINT
*
* Description 	: get nand interrupt info.
* Arguments	:
* Returns		: interrupt no. 0 = RB_B2R,1 = SINGLE_CMD_FINISH,2 = DMA_FINISH,
								5 = MULTI_CMD_FINISH
* Notes		:
********************************************************************************/
__s32 _vender_get_param(__u8 *para, __u8 *addr, __u32 count)
{
    __u32 i, cfg;
    __u32 cmd_r = 0;
    __s32 ret = 0;

    _enter_nand_critical();

    if(read_retry_mode <0x10) //hynix mode
    {
        cmd_r = 0x37;
    }
    else if((read_retry_mode >=0x10)&&(read_retry_mode <0x20)) //toshiba mode
    {
        _exit_nand_critical();
		return ret;
    }

    for(i=0; i<count; i++)
	{
		_set_addr(&addr[i], 1);

        //set data cnt
		NFC_WRITE_REG(NFC_REG_CNT, 1);

		/*set NFC_REG_CMD*/
		cfg = cmd_r;
		cfg |= (NFC_SEND_ADR | NFC_DATA_TRANS | NFC_SEND_CMD1 );
		NFC_WRITE_REG(NFC_REG_CMD, cfg);

		ret = _wait_cmdfifo_free();
		ret |= _wait_cmd_finish();

		if(ret)
		{
			_exit_nand_critical();
			return ret;
		}

		*(para+i) = NFC_READ_RAM_B(NFC_RAM0_BASE);
	}

    _exit_nand_critical();
	return ret;
}

__s32 _vender_set_param(__u8 *para, __u8 *addr, __u32 count)
{
    __u32 i, cfg;
    __u32 cmd_w=0xff, cmd_end=0xff, cmd_done0 =0xff, cmd_done1=0xff;
    __s32 ret = 0;

    _enter_nand_critical();

    if(read_retry_mode <0x10) //hynix mode
    {
        cmd_w = 0x36;
        cmd_end = 0x16;
        cmd_done0 = 0xff;
        cmd_done1 = 0xff;
    }
    else if((read_retry_mode >=0x10)&&(read_retry_mode <0x20)) //toshiba mode
    {
        cmd_w = 0x55;
        cmd_end = 0xff;
        cmd_done0 = 0x26;
        cmd_done1 = 0x5D;

    }
    else if((read_retry_mode >=0x20)&&(read_retry_mode <0x30)) //Samsung mode
    {
        cmd_w = 0xA1;
        cmd_end = 0xff;
        cmd_done0 = 0xff;
        cmd_done1 = 0xff;
    }
	else if((read_retry_mode >=0x30)&&(read_retry_mode <0x40)) //Sandisk mode
    {
		if((0x30==read_retry_mode) || (0x31==read_retry_mode))
		{
	        cmd_w = 0x53;
	        cmd_end = 0xff;
	        cmd_done0 = 0xB6;
	        cmd_done1 = 0xff;
		}
		else if(0x32==read_retry_mode)
		{
			cmd_w = 0xEF;
			cmd_end = 0xff;
			cmd_done0 = 0x5D;
			cmd_done1 = 0xff;
		}
    }
	else if((read_retry_mode >=0x40)&&(read_retry_mode <0x50)) //micron mode
    {
        cmd_w = 0xef;
        cmd_end = 0xff;
        cmd_done0 = 0xff;
        cmd_done1 = 0xff;
    }
	else if((read_retry_mode >=0x50)&&(read_retry_mode <0x60)) //intel mode
    {
        cmd_w = 0xef;
        cmd_end = 0xff;
        cmd_done0 = 0xff;
        cmd_done1 = 0xff;
    }
    else
    {
        return -1;
    }  
    
    for(i=0; i<count; i++)
	{
	    if((read_retry_mode >=0x20)&&(read_retry_mode <0x30)) //samsung mode
	    {
	        /* send cmd to set param */
	        NFC_WRITE_RAM_B(NFC_RAM0_BASE, 0x00);
	        NFC_WRITE_RAM_B(NFC_RAM0_BASE+1, addr[i]);
    		NFC_WRITE_RAM_B(NFC_RAM0_BASE+2, para[i]);

    		NFC_WRITE_REG(NFC_REG_CNT, 3);

    		/*set NFC_REG_CMD*/
    		cfg = cmd_w;
    		cfg |= ( NFC_DATA_TRANS | NFC_ACCESS_DIR | NFC_SEND_CMD1);
    		nfc_repeat_mode_enable();
    		NFC_WRITE_REG(NFC_REG_CMD, cfg);
    		nfc_repeat_mode_disable();
	    }
	    else if((read_retry_mode>=0x40)&&(read_retry_mode<0x50))  //micron read retry mode
	    {
	    		 /* send cmd to set param */
	        NFC_WRITE_RAM_B(NFC_RAM0_BASE, para[i]);
	        NFC_WRITE_RAM_B(NFC_RAM0_BASE+1, 0x0);
    		NFC_WRITE_RAM_B(NFC_RAM0_BASE+2, 0x0);
				NFC_WRITE_RAM_B(NFC_RAM0_BASE+3, 0x0);
    		NFC_WRITE_REG(NFC_REG_CNT, 4);
    		
    		/*set NFC_REG_CMD*/
    		cfg = cmd_w;
    		cfg |= (NFC_SEND_ADR | NFC_DATA_TRANS | NFC_ACCESS_DIR | NFC_SEND_CMD1 | NFC_WAIT_FLAG);
    		_set_addr(&addr[i], 1);
    		NFC_WRITE_REG(NFC_REG_CMD, cfg);
	    	
	    	
	    }
		else if((read_retry_mode>=0x50)&&(read_retry_mode<0x60))  //intel read retry mode
	    {
	    		 /* send cmd to set param */
	        NFC_WRITE_RAM_B(NFC_RAM0_BASE, para[i]);
	        NFC_WRITE_RAM_B(NFC_RAM0_BASE+1, 0x0);
    		NFC_WRITE_RAM_B(NFC_RAM0_BASE+2, 0x0);
				NFC_WRITE_RAM_B(NFC_RAM0_BASE+3, 0x0);
    		NFC_WRITE_REG(NFC_REG_CNT, 4);
    		
    		/*set NFC_REG_CMD*/
    		cfg = cmd_w;
    		cfg |= (NFC_SEND_ADR | NFC_DATA_TRANS | NFC_ACCESS_DIR | NFC_SEND_CMD1 | NFC_WAIT_FLAG);
    		_set_addr(&addr[i], 1);
    		NFC_WRITE_REG(NFC_REG_CMD, cfg);
	    	
	    	
	    }
	    else //hynix & toshiba mode
	    {
	    	if(0x32 == read_retry_mode)
	    	{
		        NFC_WRITE_RAM_B(NFC_RAM0_BASE, para[0]);
		        NFC_WRITE_RAM_B(NFC_RAM0_BASE+1, para[1]);
	    		NFC_WRITE_RAM_B(NFC_RAM0_BASE+2, para[2]);
				NFC_WRITE_RAM_B(NFC_RAM0_BASE+3, para[3]);

	    	    _set_addr(&addr[0], 1);

	    	    NFC_WRITE_REG(NFC_REG_CNT,4);

	    	    cfg  = 0;
	    	    cfg |= cmd_w;

	    	    cfg |= (NFC_SEND_ADR | NFC_ACCESS_DIR | NFC_DATA_TRANS | NFC_SEND_CMD1 | NFC_WAIT_FLAG );

	    	    //nfc_repeat_mode_enable();

	    	    NFC_WRITE_REG(NFC_REG_CMD,cfg);

	    		//nfc_repeat_mode_disable();

	    	}
	    	else
	    	{
		        /* send cmd to set param */
	    		NFC_WRITE_RAM_B(NFC_RAM0_BASE, para[i]);
	    		_set_addr(&addr[i], 1);
	    		NFC_WRITE_REG(NFC_REG_CNT, 1);

	    		/*set NFC_REG_CMD*/
	    		cfg = cmd_w;
	    		cfg |= (NFC_SEND_ADR | NFC_DATA_TRANS | NFC_ACCESS_DIR | NFC_SEND_CMD1 | NFC_WAIT_FLAG);
	    		NFC_WRITE_REG(NFC_REG_CMD, cfg);
	    	}
	    }

		ret = _wait_cmdfifo_free();
		ret |= _wait_cmd_finish();

		if(ret)
		{
			_exit_nand_critical();
			return ret;
		}

		/* send cmd to end */
		if(cmd_end != 0xff)
		{
		    /*set NFC_REG_CMD*/
    		cfg = cmd_end;
    		cfg |= ( NFC_SEND_CMD1);
    		NFC_WRITE_REG(NFC_REG_CMD, cfg);

    		ret = _wait_cmdfifo_free();
    		ret |= _wait_cmd_finish();

    		if(ret)
    		{
    			_exit_nand_critical();
    			return ret;
    		}
		}

	}

    if(0x11 == read_retry_mode)
    {
    	if((0x02 == para[0])&&(0x00 == para[1])&&(0x7e == para[2])&&(0x7c == para[3]))
    	{
    		cfg = 0xB3;
    		cfg |= ( NFC_SEND_CMD1);
    		NFC_WRITE_REG(NFC_REG_CMD, cfg);

    		ret = _wait_cmdfifo_free();
    		ret |= _wait_cmd_finish();

    		if(ret)
    		{
    			_exit_nand_critical();
    			return ret;
    		}
    	}
    	else if((0x00 == para[0])&&(0x00 == para[1])&&(0x00 == para[2])&&(0x00 == para[3]))
    	{
    		_exit_nand_critical();
    		return ret;
    	}
    }
	if(cmd_done0!=0xff)
	{
	    /*set NFC_REG_CMD*/
		cfg = cmd_done0;
		cfg |= ( NFC_SEND_CMD1);
		NFC_WRITE_REG(NFC_REG_CMD, cfg);

		ret = _wait_cmdfifo_free();
		ret |= _wait_cmd_finish();

		if(ret)
		{
			_exit_nand_critical();
			return ret;
		}
	}

	if(cmd_done1!=0xff)
	{
	    /*set NFC_REG_CMD*/
		cfg = cmd_done1;
		cfg |= ( NFC_SEND_CMD1);
		NFC_WRITE_REG(NFC_REG_CMD, cfg);

		ret = _wait_cmdfifo_free();
		ret |= _wait_cmd_finish();

		if(ret)
		{
			_exit_nand_critical();
			return ret;
		}
	}



    _exit_nand_critical();
	return ret;
}

__s32 _vender_pre_condition(void)
{
    __u32 i, cfg;
    __u32 cmd[2]= {0x5c, 0xc5};
    __s32 ret = 0;

    _enter_nand_critical();
    if(((read_retry_mode>=0x10)&&(read_retry_mode<0x20))||((read_retry_mode>=0x30)&&(read_retry_mode<0x40)))  //toshiba mode & sandisk mode
    {
		if((read_retry_mode>=0x10)&&(read_retry_mode<0x20))
		{
			cmd[0] = 0x5c;
			cmd[1] = 0xc5;
		}
		else if((0x30 == read_retry_mode) || (0x31 == read_retry_mode))
		{
			cmd[0] = 0x3B;
			cmd[1] = 0xB9;
		}	
		for(i=0;i<2;i++)
        {
        	/*set NFC_REG_CMD*/
        	cfg = cmd[i];
        	cfg |= (NFC_SEND_CMD1);
        	NFC_WRITE_REG(NFC_REG_CMD, cfg);

        	ret = _wait_cmdfifo_free();
        	ret |= _wait_cmd_finish();

        	if (ret)
        	{
        		_exit_nand_critical();
        		return ret;
        	}
        }
    }
    _exit_nand_critical();

	return ret;
}

__s32 _vender_get_param_otp_hynix(__u8 *para, __u8 *addr, __u32 count)
{
    __u32 i, j, cfg;
    __s32 error_flag,ret = 0;
    __u8 address[8];
    __u8 param_reverse[64];
    __u8 reg_addr[2] = {0x0, 0x0};
    __u8 w_data[2] = {0x0, 0x0};

    _enter_nand_critical();

	if(read_retry_mode == 2)
	{
			reg_addr[0] = 0xFF;
			reg_addr[1] = 0xCC;
			w_data[0] = 0x40;
			w_data[1] = 0x4D;
	}
	else if(read_retry_mode == 3)
	{
			reg_addr[0] = 0xAE;
			reg_addr[1] = 0xB0;
			w_data[0] = 0x00;
			w_data[1] = 0x4D;
	}
	else
	{
	    return -1;
	}

    // send 0xFF cmd
	cfg = (NFC_SEND_CMD1 | NFC_WAIT_FLAG| 0xff);
	_wait_cmdfifo_free();
	NFC_WRITE_REG(NFC_REG_CMD, cfg);
	 _wait_cmd_finish();

	//send cmd 0x36, addr 0xff, data 0x40
	NFC_WRITE_REG(NFC_REG_CNT, 1);
	NFC_WRITE_RAM_B(NFC_RAM0_BASE, w_data[0]);
	address[0] = reg_addr[0];
	_set_addr(&address[0], 1);
	cfg = (NFC_SEND_CMD1 | NFC_DATA_TRANS |NFC_ACCESS_DIR | NFC_SEND_ADR |0x36);
	_wait_cmdfifo_free();
	NFC_WRITE_REG(NFC_REG_CMD, cfg);
	 _wait_cmd_finish();

	//send addr 0xCC
	address[0] = reg_addr[1];
	_set_addr(&address[0], 1);
	cfg = (NFC_SEND_ADR);
	_wait_cmdfifo_free();
	NFC_WRITE_REG(NFC_REG_CMD, cfg);
	 _wait_cmd_finish();

	//send data 0x4D
	NFC_WRITE_REG(NFC_REG_CNT, 1);
	NFC_WRITE_RAM_B(NFC_RAM0_BASE, w_data[1]);
	cfg = (NFC_DATA_TRANS | NFC_ACCESS_DIR);
	_wait_cmdfifo_free();
	NFC_WRITE_REG(NFC_REG_CMD, cfg);
	 _wait_cmd_finish();

	//send cmd 0x16, 0x17, 0x04, 0x19, 0x00
    _wait_cmdfifo_free();
    cfg = (NFC_SEND_CMD1|0x16);
    NFC_WRITE_REG(NFC_REG_CMD, cfg);
     _wait_cmd_finish();
    _wait_cmdfifo_free();
    cfg = (NFC_SEND_CMD1|0x17);
    NFC_WRITE_REG(NFC_REG_CMD, cfg);
     _wait_cmd_finish();
    _wait_cmdfifo_free();
    cfg = (NFC_SEND_CMD1|0x04);
    NFC_WRITE_REG(NFC_REG_CMD, cfg);
     _wait_cmd_finish();
    _wait_cmdfifo_free();
    cfg = (NFC_SEND_CMD1|0x19);
    NFC_WRITE_REG(NFC_REG_CMD, cfg);
    _wait_cmd_finish();

    _wait_cmdfifo_free();
    cfg = (NFC_SEND_CMD1|0x00);
    NFC_WRITE_REG(NFC_REG_CMD, cfg);
     _wait_cmd_finish();

    //send addr 00, 00, 00, 02, 00
	address[0] = 0x00;
	address[1] = 0x00;
	address[2] = 0x00;
	address[3] = 0x02;
	address[4] = 0x00;
	_set_addr(&address[0], 5);
	cfg = (NFC_SEND_ADR|(0x4<<16));
	_wait_cmdfifo_free();
	NFC_WRITE_REG(NFC_REG_CMD, cfg);
	 _wait_cmd_finish();

	//send cmd 0x30, read data
	_wait_cmdfifo_free();

	NFC_WRITE_REG(NFC_REG_CNT, 2);
	cfg = (NFC_SEND_CMD1|NFC_WAIT_FLAG|NFC_DATA_TRANS|0x30);
	NFC_WRITE_REG(NFC_REG_CMD, cfg);
   _wait_cmd_finish();
   //get param data
    if ((NFC_READ_RAM_B(NFC_RAM0_BASE)!=0x08)||((NFC_READ_RAM_B(NFC_RAM0_BASE+1)!=0x08)))
    {
        PRINT("hynix OTP RegCount value error: 0x%x, 0x%x \n",NFC_READ_RAM_B(NFC_RAM0_BASE), NFC_READ_RAM_B(NFC_RAM0_BASE+1) );
        ret = -1;
    }

		_wait_cmdfifo_free();
	  NFC_WRITE_REG(NFC_REG_CNT, 1024);
    cfg = (NFC_DATA_TRANS);
    NFC_WRITE_REG(NFC_REG_CMD, cfg);
    _wait_cmd_finish();

    for(j=0;j<8;j++)
    {
        error_flag = 0;
        for(i=0;i<64;i++)
        {
            para[i] = NFC_READ_RAM_B(NFC_RAM0_BASE+128*j+i);
            param_reverse[i] = NFC_READ_RAM_B(NFC_RAM0_BASE+128*j+64+i);
            if((para[i]+param_reverse[i])!= 0xff)
            {
                error_flag = 1;
                break;
            }
        }
        if(!error_flag)
        {
        	PRINT("otp copy %d is ok!\n",j);
        	break;
        }

    }

    if(error_flag)
        ret = -1;

	// send 0xFF cmd
	cfg = (NFC_SEND_CMD1 | NFC_WAIT_FLAG| 0xff);
	_wait_cmdfifo_free();
	NFC_WRITE_REG(NFC_REG_CMD, cfg);
    _wait_cmd_finish();

	// send 0x38 cmd
	cfg = (NFC_SEND_CMD1 | NFC_WAIT_FLAG| 0x38);
	_wait_cmdfifo_free();
	NFC_WRITE_REG(NFC_REG_CMD, cfg);
    _wait_cmd_finish();

    _exit_nand_critical();
	return ret;
}

//for offset from defaul value
__s32 NFC_ReadRetry(__u32 chip, __u32 retry_count, __u32 read_retry_type)
{
    __u32 i;
    __s32 ret=0;
	__u32 toggle_mode_flag = 0;
    __s16 temp_val;
    __u8 param[READ_RETRY_MAX_REG_NUM];
    __u32 nand_clk_bak;
    __u8 addr_0x32[2];

	if(retry_count >read_retry_cycle)
		return -1;

    if(read_retry_mode<0x10)  //for hynix read retry mode
    {
		if((read_retry_mode == 0)||(read_retry_mode == 1))
		{
		    if(retry_count == 0)
	            ret = _vender_set_param(&read_retry_default_val[chip][0], &read_retry_reg_adr[0], read_retry_reg_num);
	        else
	        {
	            for(i=0; i<read_retry_reg_num; i++)
            	{

            	    temp_val = (read_retry_default_val[chip][i] + read_retry_val[retry_count-1][i]);
            	    if(temp_val >255)
            	        temp_val = 0xff;
            	    else if(temp_val <0)
        				temp_val = 0;
        			else
            	        temp_val &= 0xff;

            	    param[i] = (__u8)temp_val;

            	}

    		    //fix 0
    			if((retry_count >=2)&&(retry_count<=6))
    				param[0] = 0;

    			if((retry_count == 5)||(retry_count == 6))
        	    	param[1] = 0;

        	    ret =_vender_set_param(&param[0], &read_retry_reg_adr[0], read_retry_reg_num);
	        }

		}
		else if((read_retry_mode == 2)||(read_retry_mode == 3))
		{
		    for(i=0; i<read_retry_reg_num; i++)
		        param[i] = hynix_read_retry_otp_value[chip][retry_count][i];


		    ret =_vender_set_param(&param[0], &read_retry_reg_adr[0], read_retry_reg_num);

    	}

    }
    else if((read_retry_mode>=0x10)&&(read_retry_mode<0x20))  //for toshiba readretry mode
    {
    
    	nand_clk_bak = NAND_GetClk();
    	NAND_SetClk(10);

		if(retry_count == 1)
		    _vender_pre_condition();

    	for(i=0; i<read_retry_reg_num; i++)
        	param[i] = (__u8)read_retry_val[retry_count-1][i];
        if((NFC_READ_REG(NFC_REG_CTL)<<18)&0x3) //change to legacy mode from toggle mode  after 0x53h cmd
		{
			NFC_WRITE_REG(NFC_REG_CTL,NFC_READ_REG(NFC_REG_CTL)&(~(0x3<<18)));
			toggle_mode_flag = 1;
		}
		
		ret =_vender_set_param(&param[0], &read_retry_reg_adr[0], read_retry_reg_num);
		
		if(toggle_mode_flag == 1) //change to toggle mode from legacy mode  after set param
			NFC_WRITE_REG(NFC_REG_CTL,NFC_READ_REG(NFC_REG_CTL)|(0x3<<18));
		
        NAND_SetClk(nand_clk_bak);

    
    }
    else if((read_retry_mode>=0x20)&&(read_retry_mode<0x30))
    {
        for(i=0; i<read_retry_reg_num; i++)
            param[i] = (__u8)read_retry_val[retry_count][i];
            
        ret =_vender_set_param(&param[0], &read_retry_reg_adr[0], read_retry_reg_num);
    }
	else if((read_retry_mode>=0x30)&&(read_retry_mode<0x40))  //for sandisk readretry mode
    {
        

		if(0x30 == read_retry_mode)//for sandisk 19nm flash
		{
			nand_clk_bak = NAND_GetClk();
	        NAND_SetClk(10);
        
	        if(retry_count == 1)
	            _vender_pre_condition();
			for(i=0; i<read_retry_reg_num; i++)
	            param[i] = 0x0;
			if(read_retry_cycle==16)
			{
				param[0] = (__u8)param0x30low[retry_count-1][0];
				param[3] = (__u8)param0x30low[retry_count-1][1];

			}
	        else if(read_retry_cycle==20)
			{
				param[0] = (__u8)param0x30high[retry_count-1][0];
				param[1] = (__u8)param0x30high[retry_count-1][1];

			}
			if((NFC_READ_REG(NFC_REG_CTL)<<18)&0x3) //change to legacy mode from toggle mode 
			{
				NFC_WRITE_REG(NFC_REG_CTL,NFC_READ_REG(NFC_REG_CTL)&(~(0x3<<18)));
				toggle_mode_flag = 1;
			}
		
	        ret =_vender_set_param(&param[0], &read_retry_reg_adr[0], read_retry_reg_num);

			if(toggle_mode_flag == 1) //change to toggle mode from legacy mode  after set param
				NFC_WRITE_REG(NFC_REG_CTL,NFC_READ_REG(NFC_REG_CTL)|(0x3<<18));
		
	        NAND_SetClk(nand_clk_bak);
		}
		else if(0x31 == read_retry_mode)//for sandisk 24nm flash
		{
			nand_clk_bak = NAND_GetClk();
	        NAND_SetClk(10);
        
	        if(retry_count == 1)
	            _vender_pre_condition();
			for(i=0; i<read_retry_reg_num; i++)
				param[i] = 0x0;
			
			param[0] = (__u8)param0x31[retry_count-1][0];
			param[1] = (__u8)param0x31[retry_count-1][1];
			param[3] = (__u8)param0x31[retry_count-1][2];	
			
			if((NFC_READ_REG(NFC_REG_CTL)<<18)&0x3) //change to legacy mode from toggle mode 
			{
				NFC_WRITE_REG(NFC_REG_CTL,NFC_READ_REG(NFC_REG_CTL)&(~(0x3<<18)));
				toggle_mode_flag = 1;
			}
		
	        ret =_vender_set_param(&param[0], &read_retry_reg_adr[0], read_retry_reg_num);

			if(toggle_mode_flag == 1) //change to toggle mode from legacy mode  after set param
				NFC_WRITE_REG(NFC_REG_CTL,NFC_READ_REG(NFC_REG_CTL)|(0x3<<18));
		
	        NAND_SetClk(nand_clk_bak);		
		}
			
		else if(0x32==read_retry_mode)
		{

			addr_0x32[0] = 0x11;

			nand_clk_bak = NAND_GetClk();
	        NAND_SetClk(10);

			for(i=0; i<read_retry_reg_num; i++)
				param[i] = 0x0;

			for(i=0; i<4; i++)
			{
				param[i] = (__u8)param0x32[retry_count-1][i];
			}

			if((NFC_READ_REG(NFC_REG_CTL)<<18)&0x3) //change to legacy mode from toggle mode
			{
				NFC_WRITE_REG(NFC_REG_CTL,NFC_READ_REG(NFC_REG_CTL)&(~(0x3<<18)));
				toggle_mode_flag = 1;
			}

			ret =_vender_set_param(&param[0], &addr_0x32[0], 1);

			if(toggle_mode_flag == 1) //change to toggle mode from legacy mode  after set param
				NFC_WRITE_REG(NFC_REG_CTL,NFC_READ_REG(NFC_REG_CTL)|(0x3<<18));

			NAND_SetClk(nand_clk_bak);

		}
		
    }
	else if((read_retry_mode>=0x40)&&(read_retry_mode<0x50))  //for micron readretry mode
	{
		for(i=0; i<read_retry_reg_num; i++)
            param[i] = (__u8)read_retry_val[retry_count-1][i];
            
        ret =_vender_set_param(&param[0], &read_retry_reg_adr[0], read_retry_reg_num);

	}
	else if((read_retry_mode>=0x50)&&(read_retry_mode<0x60))  //for intel readretry mode
	{
		for(i=0; i<read_retry_reg_num; i++)
            param[i] = (__u8)read_retry_val[retry_count-1][i];

		if(retry_count==4)
		{
			__u8 param_intel[1] ={0x01};  //enable advanced read retry
			__u8 adr_intel[1] = {0x93};
			ret = _vender_set_param(&param_intel[0], &adr_intel[0], read_retry_reg_num);

		}
        ret =_vender_set_param(&param[0], &read_retry_reg_adr[0], read_retry_reg_num);

	}
	return ret;
}

__s32 NFC_ReadRetryInit(__u32 read_retry_type)
{
	__u32 i,j;
	//init
	read_retry_mode = (read_retry_type>>16)&0xff;
	read_retry_cycle =(read_retry_type>>8)&0xff;
	read_retry_reg_num = (read_retry_type>>0)&0xff;

	if(read_retry_mode == 0)  //mode0  H27UCG8T2MYR
	{
		read_retry_reg_adr[0] = 0xAC;
		read_retry_reg_adr[1] = 0xAD;
		read_retry_reg_adr[2] = 0xAE;
		read_retry_reg_adr[3] = 0xAF;

		//set read retry level
		for(i=0;i<read_retry_cycle;i++)
		{
			for(j=0; j<read_retry_reg_num;j++)
			{
				read_retry_val[i][j] = para0[i][j];
			}
		}

	}
	else if(read_retry_mode == 1) //mode1  H27UBG8T2BTR
	{
		read_retry_reg_adr[0] = 0xA7;
		read_retry_reg_adr[1] = 0xAD;
		read_retry_reg_adr[2] = 0xAE;
		read_retry_reg_adr[3] = 0xAF;

		//set read retry level
		for(i=0;i<read_retry_cycle;i++)
		{
			for(j=0; j<read_retry_reg_num;j++)
			{
				read_retry_val[i][j] = para1[i][j];
			}

		}
	}
	else if(read_retry_mode == 2) //mode2  H27UCG8T2ATR
	{
		read_retry_reg_adr[0] = 0xCC;
		read_retry_reg_adr[1] = 0xBF;
		read_retry_reg_adr[2] = 0xAA;
		read_retry_reg_adr[3] = 0xAB;
		read_retry_reg_adr[4] = 0xCD;
		read_retry_reg_adr[5] = 0xAD;
		read_retry_reg_adr[6] = 0xAE;
		read_retry_reg_adr[7] = 0xAF;
	}
	else if(read_retry_mode ==3) //mode2  H27UCG8T2ATR
	{
		read_retry_reg_adr[0] = 0xB0;
		read_retry_reg_adr[1] = 0xB1;
		read_retry_reg_adr[2] = 0xB2;
		read_retry_reg_adr[3] = 0xB3;
		read_retry_reg_adr[4] = 0xB4;
		read_retry_reg_adr[5] = 0xB5;
		read_retry_reg_adr[6] = 0xB6;
		read_retry_reg_adr[7] = 0xB7;
	}
	else if((read_retry_mode>=0x10)&&(read_retry_mode<0x20))  //mode0x10  toshiba readretry mode0
	{
		if(0x10 == read_retry_mode)
		{
		    read_retry_reg_adr[0] = 0x04;
			read_retry_reg_adr[1] = 0x05;
			read_retry_reg_adr[2] = 0x06;
			read_retry_reg_adr[3] = 0x07;

		    //set read retry level
			for(i=0;i<read_retry_cycle;i++)
			{
				for(j=0; j<read_retry_reg_num;j++)
				{
					read_retry_val[i][j] = para0x10[i];
				}
			}
		}
		else if(0x11 == read_retry_mode)
		{
			read_retry_reg_adr[0] = 0x04;
			read_retry_reg_adr[1] = 0x05;
			read_retry_reg_adr[2] = 0x06;
			read_retry_reg_adr[3] = 0x07;
			read_retry_reg_adr[4] = 0x0D;
			for(i=0;i<read_retry_cycle;i++)
			{
				for(j=0; j<read_retry_reg_num;j++)
				{
					read_retry_val[i][j] = para0x11[i][j];
				}
			}
		}

	}
	else if(read_retry_mode == 0x20)  //mode0x10  Samsung mode0
	{
	    read_retry_reg_adr[0] = 0xA7;
		read_retry_reg_adr[1] = 0xA4;
		read_retry_reg_adr[2] = 0xA5;
		read_retry_reg_adr[3] = 0xA6;

	    //set read retry level
		for(i=0;i<read_retry_cycle+1;i++)
		{
			for(j=0; j<read_retry_reg_num;j++)
			{
				read_retry_val[i][j] = para0x20[i][j];
			}
			
		}
	}
	else if((read_retry_mode>=0x30)&&(read_retry_mode<0x40))
	{

		read_retry_reg_adr[0] = 0x04;
		read_retry_reg_adr[1] = 0x05;
		read_retry_reg_adr[2] = 0x06;
		read_retry_reg_adr[3] = 0x07;
		read_retry_reg_adr[4] = 0x08;
		read_retry_reg_adr[5] = 0x09;
		read_retry_reg_adr[6] = 0x0a;
		read_retry_reg_adr[7] = 0x0b;
		read_retry_reg_adr[8] = 0x0c;
	}
	else if(read_retry_mode == 0x40) //mode 0x40 micron mode
	{
		read_retry_reg_adr[0] = 0x89;

		for(i=0;i<read_retry_cycle;i++)
		{
			for(j=0; j<read_retry_reg_num;j++)
			{
				read_retry_val[i][j] = param0x40[i];
			}
			
		}
	}
	else if(read_retry_mode == 0x50) //mode 0x50 intel mode
	{
		read_retry_reg_adr[0] = 0x89;

		for(i=0;i<read_retry_cycle;i++)
		{
			for(j=0; j<read_retry_reg_num;j++)
			{
				read_retry_val[i][j] = param0x50[i];
			}
			
		}
	}
	
	return 0;
}

void NFC_GetOTPValue(__u32 chip, __u8* otp_value, __u32 read_retry_type)
{
    __u8 *pdata;
    __u32 i;

    if((read_retry_mode == 0x2)||(read_retry_mode == 0x3))
    {
        pdata = (__u8 *)(&hynix_read_retry_otp_value[chip][0][0]);
        for(i=0; i<64; i++)
            pdata[i] = otp_value[i];
    }
}

__s32 NFC_GetDefaultParam(__u32 chip,__u8* default_value, __u32 read_retry_type)
{
    __s32 ret;
    __u32 i, j, Count;


    if(read_retry_mode<0x10)  //hynix read retry mode
    {
        if((read_retry_mode == 0x0)||(read_retry_mode == 0x1))
        {
            ret =_vender_get_param(&read_retry_default_val[chip][0], &read_retry_reg_adr[0], read_retry_reg_num);
            for(i=0; i<read_retry_reg_num; i++)
            {
                default_value[i] = read_retry_default_val[chip][i];
            }

        	return ret;
        }
        else if((read_retry_mode == 0x2)||(read_retry_mode == 0x3))
        {
            for(Count =0; Count<5; Count++)
            {
                PRINT("_vender_get_param_otp_hynix time %d!\n", Count);
                ret = _vender_get_param_otp_hynix(&hynix_read_retry_otp_value[chip][0][0], &read_retry_reg_adr[0], 64);
                if(!ret)
                    break;
            }
            if(ret)
                PRINT("_vender_get_param_otp_hynix error!\n");

            //set read retry level
    		for(i=0;i<8;i++)
    		{
    			for(j=0; j<8;j++)
    			{
    				default_value[8*i+j] = hynix_read_retry_otp_value[chip][i][j];
    			}

    		}
        }

	}

    return 0;

}

__s32 NFC_SetDefaultParam(__u32 chip,__u8* default_value,__u32 read_retry_type)
{
    __s32 ret = 0;
    __u32 i,cfg,nand_clk_bak;
	__u32 toggle_mode_flag = 0;
	__u8 addr_0x32[2];
    
    if(read_retry_mode<0x10)  //hynix read retry mode
    {
        for(i=0; i<read_retry_reg_num; i++)
        {
            if((read_retry_mode == 0x0)||(read_retry_mode == 0x1))
                default_value[i] = read_retry_default_val[chip][i];
            else if((read_retry_mode == 0x2)||(read_retry_mode == 0x3))   
                default_value[i] = hynix_read_retry_otp_value[chip][0][i];
        }
        ret =_vender_set_param(default_value, &read_retry_reg_adr[0], read_retry_reg_num);
        
    	return ret;
    }
    else if((read_retry_mode>=0x10)&&(read_retry_mode<0x20))
    {
    	if(0x11 == read_retry_mode)
    	{
    		nand_clk_bak = NAND_GetClk();
	        NAND_SetClk(10);
			ret = _vender_pre_condition();
    		for(i=0; i<read_retry_reg_num; i++)
    		{
    			default_value[i] = 0x0;
    		}

    		if((NFC_READ_REG(NFC_REG_CTL)<<18)&0x3) //change to legacy mode from toggle mode
    		{
    		    NFC_WRITE_REG(NFC_REG_CTL,NFC_READ_REG(NFC_REG_CTL)&(~(0x3<<18)));
    		    toggle_mode_flag = 1;
    		}

    		ret =_vender_set_param(default_value, &read_retry_reg_adr[0], read_retry_reg_num);

    		if(toggle_mode_flag == 1) //change to toggle mode from legacy mode  after set param
    		    NFC_WRITE_REG(NFC_REG_CTL,NFC_READ_REG(NFC_REG_CTL)|(0x3<<18));

    		cfg = 0xFF;
    		cfg |= ( NFC_SEND_CMD1 | NFC_WAIT_FLAG);
    		NFC_WRITE_REG(NFC_REG_CMD, cfg);


    		ret |= _wait_cmdfifo_free();
    		ret |= _wait_cmd_finish();
    		NAND_SetClk(nand_clk_bak);
    		if(ret)
    		{
    			_exit_nand_critical();
    			return ret;
    		}
    	}
    }
    else if((read_retry_mode>=0x20)&&(read_retry_mode<0x30))  //samsung read retry mode
    {
        for(i=0; i<read_retry_reg_num; i++)
        {
            default_value[i] = (__u8)read_retry_val[0][i];
        }
        ret =_vender_set_param(default_value, &read_retry_reg_adr[0], read_retry_reg_num);
        
    	return ret;
    }
	else if((read_retry_mode>=0x30)&&(read_retry_mode<0x40))  //sandisk read retry mode
	{
		if((0x30 == read_retry_mode) || (0x31 == read_retry_mode))
		{
			nand_clk_bak = NAND_GetClk();
	        NAND_SetClk(10);

			ret = _vender_pre_condition();
			for(i=0; i<read_retry_reg_num; i++)
	        {
	            default_value[i] = 0x0;
	        }
		
			if((NFC_READ_REG(NFC_REG_CTL)<<18)&0x3) //change to legacy mode from toggle mode 
			{
				NFC_WRITE_REG(NFC_REG_CTL,NFC_READ_REG(NFC_REG_CTL)&(~(0x3<<18)));
				toggle_mode_flag = 1;
			}
		
	        ret |= _vender_set_param(default_value,&read_retry_reg_adr[0],read_retry_reg_num);

			if(toggle_mode_flag == 1) //change to toggle mode from legacy mode  after set param
				NFC_WRITE_REG(NFC_REG_CTL,NFC_READ_REG(NFC_REG_CTL)|(0x3<<18));

			cfg = 0xD6; 
			cfg |= ( NFC_SEND_CMD1);
			NFC_WRITE_REG(NFC_REG_CMD, cfg);

			ret |= _wait_cmdfifo_free();
			ret |= _wait_cmd_finish();

			NAND_SetClk(nand_clk_bak);
			if(ret)
			{
				_exit_nand_critical();
				return ret;
			}
		}
		else if(0x32 == read_retry_mode)
		{
			addr_0x32[0] = 0x11;

			nand_clk_bak = NAND_GetClk();
	        NAND_SetClk(10);

			for(i=0; i<read_retry_reg_num; i++)
			{
				default_value[i] = 0x0;
			}

			if((NFC_READ_REG(NFC_REG_CTL)<<18)&0x3) //change to legacy mode from toggle mode
			{
				NFC_WRITE_REG(NFC_REG_CTL,NFC_READ_REG(NFC_REG_CTL)&(~(0x3<<18)));
				toggle_mode_flag = 1;
			}

			ret |= _vender_set_param(default_value,&addr_0x32[0],1);

			if(toggle_mode_flag == 1) //change to toggle mode from legacy mode  after set param
				NFC_WRITE_REG(NFC_REG_CTL,NFC_READ_REG(NFC_REG_CTL)|(0x3<<18));

			cfg = 0xFF;
			cfg |= ( NFC_SEND_CMD1 | NFC_WAIT_FLAG);
			NFC_WRITE_REG(NFC_REG_CMD, cfg);


			ret |= _wait_cmdfifo_free();
			ret |= _wait_cmd_finish();
			NAND_SetClk(nand_clk_bak);
			if(ret)
			{
				_exit_nand_critical();
				return ret;
			}
		}
	}
	else if((read_retry_mode>=0x40)&&(read_retry_mode<0x50))  //micron read retry mode
    {
        for(i=0; i<read_retry_reg_num; i++)
        {
            default_value[i] = 0x0;
        }
        ret =_vender_set_param(default_value, &read_retry_reg_adr[0], read_retry_reg_num);
        
    	return ret;
    }
	else if((read_retry_mode>=0x50)&&(read_retry_mode<0x60))  //intel read retry mode
    {
        for(i=0; i<read_retry_reg_num; i++)
        {
            default_value[i] = 0x0;
        }
        ret =_vender_set_param(default_value, &read_retry_reg_adr[0], read_retry_reg_num);
		{
			__u8 param_intel[1] ={0x00};  //disable advanced read retry
			__u8 adr_intel[1] = {0x93};
			ret = _vender_set_param(&param_intel[0], &adr_intel[0], read_retry_reg_num);	
        }
    	return ret;
    }
	
	else
    {
        return 0;
    }
        
    
}


__s32 NFC_ReadRetry_off(__u32 chip) //sandisk readretry exit
{
	__u32 cfg;
    __s32 ret = 0;
	__u8 default_value[64];

	if((0x30 == read_retry_mode) || (0x31 == read_retry_mode))
	{
		cfg = 0xD6;
		cfg |= ( NFC_SEND_CMD1);
		NFC_WRITE_REG(NFC_REG_CMD, cfg);
	}
	else if(0x32 == read_retry_mode)
	{
		cfg = 0xFF;
		cfg |= ( NFC_SEND_CMD1 | NFC_WAIT_FLAG);
		NFC_WRITE_REG(NFC_REG_CMD, cfg);
	}

	ret = _wait_cmdfifo_free();
	ret |= _wait_cmd_finish();
	
	if(ret)
	{
		_exit_nand_critical();
		return ret;
	}

	NFC_SetDefaultParam(chip,default_value,read_retry_mode);

}

__s32 NFC_ReadRetry_0x32_UpperPage(void)
{
	__u32 cfg;
	__s32 ret;

	cfg = 0x25;
	cfg |= ( NFC_SEND_CMD1);
	NFC_WRITE_REG(NFC_REG_CMD, cfg);

	ret = _wait_cmdfifo_free();
	ret |= _wait_cmd_finish();

	if(ret)
	{
		_exit_nand_critical();
		return ret;
	}

	return 0;
}

__s32 NFC_ReadRetryExit(__u32 read_retry_type)
{

	return 0;
}


void NFC_RbIntEnable(void)
{
    //enable interrupt
	NFC_WRITE_REG(NFC_REG_INT, NFC_READ_REG(NFC_REG_INT)|NFC_B2R_INT_ENABLE);
}

void NFC_RbIntDisable(void)
{
    //disable rb interrupt
	NFC_WRITE_REG(NFC_REG_INT, NFC_READ_REG(NFC_REG_INT)&(~NFC_B2R_INT_ENABLE));
}

void NFC_RbIntClearStatus(void)
{
    //clear interrupt
	NFC_WRITE_REG(NFC_REG_ST,NFC_RB_B2R);
}

__u32 NFC_RbIntGetStatus(void)
{
    //clear interrupt
	return (NFC_READ_REG(NFC_REG_ST)&NFC_RB_B2R);
}

__u32 NFC_RbIntOccur(void)
{
	return ((NFC_READ_REG(NFC_REG_ST)&NFC_RB_B2R)&&(NFC_READ_REG(NFC_REG_INT)&NFC_B2R_INT_ENABLE));
}

__u32 NFC_GetRbSelect(void)
{
    return (( NFC_READ_REG(NFC_REG_CTL) & NFC_RB_SEL ) >>3);
}

__u32 NFC_GetRbStatus(__u32 rb)
{
    if(rb == 0)
        return (NFC_READ_REG(NFC_REG_ST) & NFC_RB_STATE0);
    else if(rb == 1)
        return (NFC_READ_REG(NFC_REG_ST) & NFC_RB_STATE1); 
    else 
        return 0;        
}

void NFC_DmaIntEnable(void)
{
    //enable interrupt
	NFC_WRITE_REG(NFC_REG_INT, NFC_READ_REG(NFC_REG_INT)|NFC_DMA_INT_ENABLE);
}


void NFC_DmaIntDisable(void)
{
    //disable dma interrupt
	NFC_WRITE_REG(NFC_REG_INT, NFC_READ_REG(NFC_REG_INT)&(~NFC_DMA_INT_ENABLE));
}

void NFC_DmaIntClearStatus(void)
{
    //clear interrupt
	NFC_WRITE_REG(NFC_REG_ST,NFC_DMA_INT_FLAG);
}

__u32 NFC_DmaIntGetStatus(void)
{
    //clear interrupt
	return (NFC_READ_REG(NFC_REG_ST)&NFC_DMA_INT_FLAG);
}

__u32 NFC_DmaIntOccur(void)
{
	return ((NFC_READ_REG(NFC_REG_ST)&NFC_DMA_INT_FLAG)&&(NFC_READ_REG(NFC_REG_INT)&NFC_DMA_INT_ENABLE));
}





