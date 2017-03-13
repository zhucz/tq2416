/*
 * (C) Copyright 2006 OpenMoko, Inc.
 * Author: Harald Welte <laforge@openmoko.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>

#include <nand.h>
#include <asm/arch/s3c24xx_cpu.h>
#include <asm/io.h>

#define S3C2416_NFSTAT_RnB		   (1<<0)
#define S3C2416_NFCONT_EN          (1<<1)//Mode 1 enable  0 disable
#define S3C2416_NFCONT_nFCE        (1<<1)//select_chip 0  disable chip 1
#define S3C2416_NFCONT_INITECC     (3<<4)//InitMECC  InitSECC


#define S3C2416_NFCONF_1024BYTE    (1<<3)
#define S3C2416_NFCONF_TACLS(x)    ((x)<<12)
#define S3C2416_NFCONF_TWRPH0(x)   ((x)<<8)
#define S3C2416_NFCONF_TWRPH1(x)   ((x)<<4)

#define S3C2416_ADDR_NALE 4
#define S3C2416_ADDR_NCLE 8

#ifdef CONFIG_NAND_SPL

#define DEBUG 1

/* in the early stage of NAND flash booting, printf() is not available */
#define printf(fmt, args...)

static void nand_read_buf(struct mtd_info *mtd, u_char *buf, int len)
{
	int i;
	struct nand_chip *this = mtd->priv;

	for (i = 0; i < len; i++)
		buf[i] = readb(this->IO_ADDR_R);
}
#endif

#if 0
static void s3c24xx_hwcontrol(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	struct nand_chip *chip = mtd->priv;
	struct s3c24xx_nand *nand = s3c24xx_get_base_nand();

	debug("hwcontrol(): 0x%02x 0x%02x\n", cmd, ctrl);

	if (ctrl & NAND_CTRL_CHANGE) {
		ulong IO_ADDR_W = (ulong)nand;

		if (!(ctrl & NAND_CLE))
			IO_ADDR_W |= S3C2416_ADDR_NCLE;
		if (!(ctrl & NAND_ALE))
			IO_ADDR_W |= S3C2416_ADDR_NALE;

		chip->IO_ADDR_W = (void *)IO_ADDR_W;

		if (ctrl & NAND_NCE)
			writel(readl(&nand->nfcont) & ~S3C2416_NFCONT_nFCE, &nand->nfcont);
		else
			writel(readl(&nand->nfcont) | S3C2416_NFCONT_nFCE, &nand->nfcont);
	}

	if (cmd != NAND_CMD_NONE)
		writeb(cmd, chip->IO_ADDR_W);
}
#else 


static void s3c24xx_hwcontrol(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	struct s3c24xx_nand *nand_reg = s3c24xx_get_base_nand();

	unsigned int cur = 0;

//	debug("hwcontrol(): 0x%02x 0x%02x\n", cmd, ctrl);
	if (ctrl & NAND_CTRL_CHANGE) {
		if(ctrl & NAND_NCE){
			if(cmd != NAND_CMD_NONE){
				cur = readl(&nand_reg->nfcont);
				cur &= ~S3C2416_NFCONT_EN;
				writel(cur,&nand_reg->nfcont);
			}
		}else{
				cur = readl(&nand_reg->nfcont);
				cur |= S3C2416_NFCONT_EN;
				writel(cur,&nand_reg->nfcont);
		}
	}

	if (cmd != NAND_CMD_NONE){
		if(ctrl & NAND_CLE){
			writeb(cmd,&nand_reg->nfcmmd);
		}else if(ctrl & NAND_ALE){
			writeb(cmd,&nand_reg->nfaddr);
		}
	}
}
#endif


static int s3c24xx_dev_ready(struct mtd_info *mtd)
{
	struct s3c24xx_nand *nand = s3c24xx_get_base_nand();
	while(!((readl(&nand->nfstat)) & S3C2416_NFSTAT_RnB));
	return 1;

}

static int s3c24xx_nand_scan_bbt(struct mtd_info *mtdinfo)
{
	return nand_default_bbt(mtdinfo);
}

#ifdef CONFIG_S3C2416_NAND_HWECC
void s3c24xx_nand_enable_hwecc(struct mtd_info *mtd, int mode)
{
	struct s3c24xx_nand *nand = s3c24xx_get_base_nand();
	debug("s3c24xx_nand_enable_hwecc(%p, %d)\n", mtd, mode);
	writel(readl(&nand->nfconf) | S3C2416_NFCONF_INITECC, &nand->nfconf);
}

static int s3c24xx_nand_calculate_ecc(struct mtd_info *mtd, const u_char *dat, u_char *ecc_code)
{
	struct s3c24xx_nand *nand = s3c24xx_get_base_nand();
	ecc_code[0] = readb(&nand->nfecc);
	ecc_code[1] = readb(&nand->nfecc + 1);
	ecc_code[2] = readb(&nand->nfecc + 2);
	debug("s3c24xx_nand_calculate_hwecc(%p,): 0x%02x 0x%02x 0x%02x\n", mtd , ecc_code[0], ecc_code[1], ecc_code[2]);

	return 0;
}

static int s3c24xx_nand_correct_data(struct mtd_info *mtd, u_char *dat, u_char *read_ecc, u_char *calc_ecc)
{
	if (read_ecc[0] == calc_ecc[0] &&
	    read_ecc[1] == calc_ecc[1] &&
	    read_ecc[2] == calc_ecc[2])
		return 0;

	printf("s3c24xx_nand_correct_data: not implemented\n");
	return -1;
}
#endif


#if 1
static int s3c_nand_erase(struct mtd_info *mtd, int page)
{
	struct s3c24xx_nand *nand_reg = s3c24xx_get_base_nand();
	unsigned char status = 0;
	unsigned int block_addr = 0;
	unsigned int row1_addr = 0;
	unsigned int row2_addr = 0;
	unsigned int row3_addr = 0;


	block_addr = (page / 64);
	row1_addr = ((block_addr << 6)&0xff);
	row1_addr = ((block_addr >> 2)&0xff);
	row1_addr = ((block_addr >> 10)&0xff);

	debug("%s : %d block_addr = %d \n",__func__,__LINE__,block_addr);
	debug("%s : %d row1_addr = %d \n",__func__,__LINE__,row1_addr);
	debug("%s : %d row2_addr = %d \n",__func__,__LINE__,row2_addr);
	debug("%s : %d row3_addr = %d \n",__func__,__LINE__,row3_addr);

	s3c24xx_hwcontrol(0,NAND_CMD_ERASE1,NAND_NCE|NAND_CLE|NAND_CTRL_CHANGE);// Auto Block Erase Setup Command 0x60
	s3c24xx_hwcontrol(0,row1_addr,NAND_CTRL_CHANGE | NAND_NCE | NAND_ALE);  // Row Add1
	s3c24xx_hwcontrol(0,row2_addr,NAND_NCE | NAND_ALE);					 // Row Add2
	s3c24xx_hwcontrol(0,row3_addr,NAND_NCE | NAND_ALE);					 // Row Add3
	s3c24xx_hwcontrol(0,NAND_CMD_ERASE2,NAND_NCE|NAND_CLE|NAND_CTRL_CHANGE);// Erase Command 0xD0
	s3c24xx_dev_ready(0);												 //BUSY

	s3c24xx_hwcontrol(0,NAND_CMD_STATUS,NAND_NCE|NAND_CLE|NAND_CTRL_CHANGE);//Read Status Command

		status = readb(&nand_reg->nfdata);
//	status = readb(nand->IO_ADDR_R); //I/O0 = 0 Successful Erase 
									 //I/O0 = 1 Error n Erase
	if(!(status & 0x01)){
		debug("%s : %d erase ok status = %x \n",__func__,__LINE__,status);
		return 0;
	}
	debug("%s : %d erase error status = %x \n",__func__,__LINE__,status);
	return 1;

}
#endif


int board_nand_init(struct nand_chip *nand)
{
	u_int32_t cfg;
//	u_int8_t tacls, twrph0, twrph1;
	struct s3c24xx_nand *nand_reg = s3c24xx_get_base_nand();
	struct s3c24xx_gpio * const gpio = s3c24xx_get_base_gpio();

	debug("board_nand_init()\n");

	/* initialize hardware */
#if defined(CONFIG_S3C24XX_CUSTOM_NAND_TIMING)
	tacls  = CONFIG_S3C24XX_TACLS;
	twrph0 = CONFIG_S3C24XX_TWRPH0;
	twrph1 =  CONFIG_S3C24XX_TWRPH1;
#else
//	tacls = 1;
//	twrph0 = 0;
//	twrph1 = 0;
#endif

#if 0
	cfg = S3C2416_NFCONT_EN;
	writel(cfg, &nand_reg->nfcont);

	cfg |= S3C2416_NFCONF_TACLS(tacls - 1);
	cfg |= S3C2416_NFCONF_TWRPH0(twrph0 - 1);
	cfg |= S3C2416_NFCONF_TWRPH1(twrph1 - 1);
	writel(cfg, &nand_reg->nfconf);
#endif 
	writel((0x3f << 17), &gpio->gpadat);
#if 0
	cfg = 0;
	cfg = readl(&nand_reg->nfconf);
	cfg |= (1<<12)|(2<<8)|(1<<4)|(0<<0);
	writel(cfg, &nand_reg->nfconf);


	cfg = 0;
	cfg = readl(&nand_reg->nfcont);
	cfg |= (0<<12)|(0<<10)|(0<<9)|(0<<8)|(3<<6)|(3<<4)|(3<<1)|(1<<0);
	writel(cfg, &nand_reg->nfcont);

	cfg = readl(&nand_reg->nfcont);
	cfg &= ~(1 << 16);
	writel(cfg, &nand_reg->nfcont);
#else
	cfg = 0;
	cfg = readl(&nand_reg->nfconf);
	cfg |= (0<<23)|(1<<12)|(2<<8)|(1<<4)|(0<<3)|(0<<2)|(1<<1)|(0<<0);
	writel(cfg, &nand_reg->nfconf);


	cfg = 0;
	cfg = readl(&nand_reg->nfcont);
	cfg |=(0<<16)|(0<<12)|(0<<10)|(0<<9)|(0<<8)|(0<<7)|(0<<6)|(3<<4)|(1<<2)|(0<<1)|(1<<0);
	writel(cfg, &nand_reg->nfcont);

#endif


	/* initialize nand_chip data structure */
	nand->IO_ADDR_R = (void *)&nand_reg->nfdata;
	nand->IO_ADDR_W = (void *)&nand_reg->nfdata;

	nand->select_chip = NULL;


	/* read_buf and write_buf are default */
	/* read_byte and write_byte are default */
#ifdef CONFIG_NAND_SPL
	nand->read_buf = nand_read_buf;
#endif

	/* hwcontrol always must be implemented */
	nand->cmd_ctrl = s3c24xx_hwcontrol;
	nand->dev_ready = s3c24xx_dev_ready;
	nand->scan_bbt = s3c24xx_nand_scan_bbt;
	nand->erase  = s3c_nand_erase;
	nand->options = 0;
	nand->options |= NAND_SKIP_BBTSCAN;


#ifdef CONFIG_S3C2416_NAND_HWECC
	nand->ecc.hwctl = s3c24xx_nand_enable_hwecc;
	nand->ecc.calculate = s3c24xx_nand_calculate_ecc;
	nand->ecc.correct = s3c24xx_nand_correct_data;
	nand->ecc.mode = NAND_ECC_HW;
	nand->ecc.size = CONFIG_SYS_NAND_ECCSIZE;
	nand->ecc.bytes = CONFIG_SYS_NAND_ECCBYTES;
	nand->ecc.strength = 1;
#else
	nand->ecc.mode = NAND_ECC_SOFT;
#endif

#ifdef CONFIG_S3C2416_NAND_BBT
	nand->bbt_options |= NAND_BBT_USE_FLASH;
#endif

	debug("end of nand_init\n");

	return 0;
}
