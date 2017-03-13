/*
 * (C) Copyright 2016
 * David Müller ELSOFT AG Switzerland. d.mueller@elsoft.ch
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/************************************************
 * NAME	    : s3c24xx.h
 * Version  : 12.6.2016
 *
 * common stuff for SAMSUNG S3C24XX SoC
 ************************************************/

#ifndef __S3C24XX_H__
#define __S3C24XX_H__

/* Memory controller (see manual chapter 5) */
struct s3c24xx_memctl {
	u32	bwscon;
	u32	bankcon1;
	u32	bankcon2;
	u32	bankcon3;
	u32	refresh;
	u32 timeout;
};

struct s3c24xx_ebi {
	u32 bpriority0;
	u32 bpriority1;
	u32 ebicon;
};

struct s3c24xx_ssmc {
	u32 smbidcyr0;
	u32 smbwstrdr0;
	u32 smbwstwrr0;
	u32 smbwstoenr0;
	u32 smbwstwenr0;
	u32 smbcr0;
	u32 smbsr0;
	u32 smbwstbrdr0;
	
	u32 smbidcyr1;
	u32 smbwstrdr1;
	u32 smbwstwrr1;
	u32 smbwstoenr1;
	u32 smbwstwenr1;
	u32 smbcr1;
	u32 smbsr1;
	u32 smbwstbrdr1;

	u32 smbidcyr2;
	u32 smbwstrdr2;
	u32 smbwstwrr2;
	u32 smbwstoenr2;
	u32 smbwstwenr2;
	u32 smbcr2;
	u32 smbsr2;
	u32 smbwstbrdr2;

	u32 smbidcyr3;
	u32 smbwstrdr3;
	u32 smbwstwrr3;
	u32 smbwstoenr3;
	u32 smbwstwenr3;
	u32 smbcr3;
	u32 smbsr3;
	u32 smbwstbrdr3;

	u32 smbidcyr4;
	u32 smbwstrdr4;
	u32 smbwstwrr4;
	u32 smbwstoenr4;
	u32 smbwstwenr4;
	u32 smbcr4;
	u32 smbsr4;
	u32 smbwstbrdr4;

	u32 smbidcyr5;
	u32 smbwstrdr5;
	u32 smbwstwrr5;
	u32 smbwstoenr5;
	u32 smbwstwenr5;
	u32 smbcr5;
	u32 smbsr5;
	u32 smbwstbrdr5;

};

/* USB HOST (see manual chapter 12) */
struct s3c24xx_usb_host {
	u32	HcRevision;
	u32	HcControl;
	u32	HcCommonStatus;
	u32	HcInterruptStatus;
	u32	HcInterruptEnable;
	u32	HcInterruptDisable;
	u32	HcHCCA;
	u32	HcPeriodCuttendED;
	u32	HcControlHeadED;
	u32	HcControlCurrentED;
	u32	HcBulkHeadED;
	u32	HcBuldCurrentED;
	u32	HcDoneHead;
	u32	HcRmInterval;
	u32	HcFmRemaining;
	u32	HcFmNumber;
	u32	HcPeriodicStart;
	u32	HcLSThreshold;
	u32	HcRhDescriptorA;
	u32	HcRhDescriptorB;
	u32	HcRhStatus;
	u32	HcRhPortStatus1;
	u32	HcRhPortStatus2;
};


/* INTERRUPT (see manual chapter 14) */
struct s3c24xx_interrupt {
	u32	srcpnd1;
	u32	intmod1;
	u32	intmsk1;
	u32 res1[1];
	u32 intpnd1;
	u32 intoffset1;
	u32 subsrcpnd;
	u32 intsubmsk;
	u32 res2[4];
	u32	priority_mode1;
	u32 priority_update1;
	u32 res3[2];
	u32 srcpnd2;
	u32 intmod2;
	u32 intmsk2;
	u32 res4[1];
	u32 intpnd2;
	u32 intoffset2;
	u32 res5[6];
	u32 priority_mode2;
	u32 priority_update2;
};


/* DMAS (see manual chapter 8) */
struct s3c24xx_dma {
	u32	disrc;
	u32	disrcc;
	u32	didst;
	u32	didstc;
	u32	dcon;
	u32	dstat;
	u32	dcsrc;
	u32	dcdst;
	u32	dmasktrig;
	u32	dmareqsel;
};

struct s3c24xx_dmas {
	struct s3c24xx_dma	dma[6];
};


/* CLOCK & POWER MANAGEMENT (see S3C2400 manual chapter 6) */
/*                          (see S3C2410 manual chapter 7) */
struct s3c24xx_clock_power {
	u32 lockcon0;
	u32 lockcon1;
	u32 oscset;
	u32 res1[1];
	u32 mpllcon;
	u32 res2[1];
	u32 epllcon;
	u32 epllcon_k;
	u32 clksrc;
	u32 clkdiv0;
	u32 clkdiv1;
	u32 clkdiv2;
	u32 hclkcon;
	u32 pclkcon;
	u32 sclkcon;
	u32 res3[1];
	u32 pwrmode;
	u32 swrst;
	u32 res4[2];
	u32 buspri0;
	u32 res5[3];
	u32 pwrcfg;
	u32 rstcon;
	u32 rststat;
	u32 wkupstat;
	u32 inform0;
	u32 inform1;
	u32 inform2;
	u32 inform3;
	u32 usb_phyctrl;
	u32 usb_phypwr;
	u32 usb_rstcon;
	u32 usb_clkcon;
};


/* LCD CONTROLLER (see manual chapter 15) */
struct s3c24xx_lcd {
	u32 vidcon0;
	u32 vidcon1;
	u32 vidtcon0;
	u32 vidtcon1;
	u32 vidtcon2;
	u32 wincon0;
	u32 wincon1;
	u32 res1[3];
	u32 vidosd0a;
	u32 vidosd0b;
	u32 res2[1];
	u32 vidosd1a;
	u32 vidosd1b;
	u32 vidosd1c;
	u32 res3[9];
	u32 vidw00add0b0;
	u32 vidw00add0b1;
	u32 vidw01add0;
	u32 res4[3];
	u32 vidw00add1b0;
	u32 vidw00add1b1;
	u32 vidw01add1;
	u32 res5[3];
	u32 vidw00add2b0;
	u32 vidw00add2b1;
	u32 vidw01add2;
	u32 res6[3];
	u32 vidintcon;
	u32 w1keycon0;
	u32 w1keycon1;
	u32 w2keycon0;
	u32 w2keycon1;
	u32 w3keycon0;
	u32 w3keycon1;
	u32 w4keycon0;
	u32 w4keycon1;
	u32 win0map;
	u32 win1map;
	u32 res7[3];
	u32 wpalcon;
	u32 res8[18];
	u32 sysifcon0;
	u32 sysifcon1;
	u32 dithmode;
	u32 sifccon0;
	u32 sifccon1;
	u32 sifccon2;
	u32 res9[6];
	u32 cputrigcon2;
	u32 res10[167];
	u32 win0_palette_ram;
	u32 res11[1020];
	u32 win1_palette_ram;

};


/* NAND FLASH (see manual chapter 6) */
struct s3c24xx_nand {
	u32	nfconf;
	u32	nfcont;
	u32	nfcmmd;
	u32	nfaddr;
	u32	nfdata;
	u32 nfmeccd0;
	u32 nfmeccd1;
	u32 nfseccd;
	u32 nfsblk;
	u32 nfeblk;
	u32 nfstat;
	u32 nfeccerr0;
	u32 nfeccerr1;
	u32 nfmecc0;
	u32 nfmecc1;
	u32 nfsecc;
	u32 nfmlcbitpt;
	u32 nf8eccerr0;
	u32 nf8eccerr1;
	u32 nf8eccerr2;
	u32 nfm8ecc0;
	u32 nfm8ecc1;
	u32 nfm8ecc2;
	u32 nfm8ecc3;
	u32 nfmlc8bitpt0;
	u32 nfmlc8bitpt1;
};

/* UART (see manual chapter 11) */
struct s3c24xx_uart {
	u32	ulcon;
	u32	ucon;
	u32	ufcon;
	u32	umcon;
	u32	utrstat;
	u32	uerstat;
	u32	ufstat;
	u32	umstat;
#ifdef __BIG_ENDIAN
	u8 res1[3];
	u8 utxh;
	u8 res2[3];
	u32 urxh;
#else
	u8 utxh;
	u8 res1[3];
	u32 urxh;
	u8 res2[3];
#endif
	u32	ubrdiv;
	u32 udivslot;
};


/* PWM TIMER (see manual chapter 10) */
struct s3c24xx_timer {
	u32	tcntb;
	u32	tcmpb;
	u32	tcnto;
};

struct s3c24xx_timers {
	u32	tcfg0;
	u32	tcfg1;
	u32	tcon;
	struct s3c24xx_timer	ch[4];
	u32	tcntb4;
	u32	tcnto4;
};


/* USB DEVICE (see manual chapter 13) */
struct s3c24xx_usb_dev_fifos {
#ifdef __BIG_ENDIAN
	u8	res[3];
	u8	ep_fifo_reg;
#else /*  little endian */
	u8	ep_fifo_reg;
	u8	res[3];
#endif
};

struct s3c24xx_usb_dev_dmas {
#ifdef __BIG_ENDIAN
	u8	res1[3];
	u8	ep_dma_con;
	u8	res2[3];
	u8	ep_dma_unit;
	u8	res3[3];
	u8	ep_dma_fifo;
	u8	res4[3];
	u8	ep_dma_ttc_l;
	u8	res5[3];
	u8	ep_dma_ttc_m;
	u8	res6[3];
	u8	ep_dma_ttc_h;
#else /*  little endian */
	u8	ep_dma_con;
	u8	res1[3];
	u8	ep_dma_unit;
	u8	res2[3];
	u8	ep_dma_fifo;
	u8	res3[3];
	u8	ep_dma_ttc_l;
	u8	res4[3];
	u8	ep_dma_ttc_m;
	u8	res5[3];
	u8	ep_dma_ttc_h;
	u8	res6[3];
#endif
};

struct s3c24xx_usb_device {
#ifdef __BIG_ENDIAN
	u8	res1[3];
	u8	func_addr_reg;
	u8	res2[3];
	u8	pwr_reg;
	u8	res3[3];
	u8	ep_int_reg;
	u8	res4[15];
	u8	usb_int_reg;
	u8	res5[3];
	u8	ep_int_en_reg;
	u8	res6[15];
	u8	usb_int_en_reg;
	u8	res7[3];
	u8	frame_num1_reg;
	u8	res8[3];
	u8	frame_num2_reg;
	u8	res9[3];
	u8	index_reg;
	u8	res10[7];
	u8	maxp_reg;
	u8	res11[3];
	u8	ep0_csr_in_csr1_reg;
	u8	res12[3];
	u8	in_csr2_reg;
	u8	res13[7];
	u8	out_csr1_reg;
	u8	res14[3];
	u8	out_csr2_reg;
	u8	res15[3];
	u8	out_fifo_cnt1_reg;
	u8	res16[3];
	u8	out_fifo_cnt2_reg;
#else /*  little endian */
	u8	func_addr_reg;
	u8	res1[3];
	u8	pwr_reg;
	u8	res2[3];
	u8	ep_int_reg;
	u8	res3[15];
	u8	usb_int_reg;
	u8	res4[3];
	u8	ep_int_en_reg;
	u8	res5[15];
	u8	usb_int_en_reg;
	u8	res6[3];
	u8	frame_num1_reg;
	u8	res7[3];
	u8	frame_num2_reg;
	u8	res8[3];
	u8	index_reg;
	u8	res9[7];
	u8	maxp_reg;
	u8	res10[7];
	u8	ep0_csr_in_csr1_reg;
	u8	res11[3];
	u8	in_csr2_reg;
	u8	res12[3];
	u8	out_csr1_reg;
	u8	res13[7];
	u8	out_csr2_reg;
	u8	res14[3];
	u8	out_fifo_cnt1_reg;
	u8	res15[3];
	u8	out_fifo_cnt2_reg;
	u8	res16[3];
#endif /*  __BIG_ENDIAN */
	struct s3c24xx_usb_dev_fifos	fifo[5];
	struct s3c24xx_usb_dev_dmas		dma[5];
};


/* WATCH DOG TIMER (see manual chapter 18) */
struct s3c24xx_watchdog {
	u32	wtcon;
	u32	wtdat;
	u32	wtcnt;
};

/* IIS (see manual chapter 21) */
struct s3c24xx_i2s {
	u32	iiscon;
	u32	iismod;
	u32 iisfic;
	u32	iispsr;
	u32	iistxd;
	u32	iisrxd;
};


/* I/O PORT (see manual chapter 9) */
struct s3c24xx_gpio {
#ifdef CONFIG_S3C2416
	u32 gpacon;
	u32 gpadat;
	u32 res1[2];
	u32 gpbcon;
	u32 gpbdat;
	u32 gpbudp;
	u32 gpbsel;
	u32 gpccon;
	u32 gpcdat;
	u32 gpcudp;
	u32 res2;
	u32 gpdcon;
	u32 gpddat;
	u32 gpdudp;
	u32 res3;
	u32 gpecon;
	u32 gpedat;
	u32 gpeudp;
	u32 gpesel;
	u32 gpfcon;
	u32 gpfdat;
	u32 gpfudp;
	u32 res4;
	u32 gpgcon;
	u32 gpgdat;
	u32 gpgudp;
	u32 res5;
	u32 gphcon;
	u32 gphdat;
	u32 gphudp;
	u32 res6;

	u32 misccr;
	u32 dclkcon;
	u32 extint[3];
	u32 res7[2];
	u32 eintflt2;
	u32 eintflt3;
	u32 eintmask;
	u32 eintpend;
	u32 gstatus[2];
	u32 res8[3];
	u32 dsc[4];

	u32 gpjcon;
	u32 gpjdat;
	u32 gpjudp;
	u32 gpjsel;
	u32 gpkcon;
	u32 gpkdat;
	u32 gpkudp;
	u32 res9;
	u32 gplcon;
	u32 gpldat;
	u32 gpludp;
	u32 gplsel;
	u32 gpmcon;
	u32 gpmdat;
	u32 gpmudp;
	u32 res10;

	u32 dsc3;
	u32 pddmcon;
	u32 pdsmcon;
#endif
};


/* RTC (see manual chapter 17) */
struct s3c24xx_rtc {
#ifdef __BIG_ENDIAN
	u8 _res1[67];
	u8 rtccon;
	u8 _res2[3];
	u8 ticnt0;
	u8 _res3[3];
	u8 ticnt2;
	u8 _res4[3];
	u8 ticnt1;
	u8 _res5[3];
	u8 rtcalm;
	u8 _res6[3];
	u8 almsec;
	u8 _res7[3];
	u8 almmin;
	u8 _res8[3];
	u8 almhour;
	u8 _res9[3];
	u8 almdate;
	u8 _res10[3];
	u8 almmon;
	u8 _res11[3];
	u8 almyear;
	u8 _res12[7];
	u8 bcdsec;
	u8 _res13[3];
	u8 bcdmin;
	u8 _res14[3];
	u8 bcdhour;
	u8 _res15[3];
	u8 bcddate;
	u8 _res16[3];
	u8 bcdday;
	u8 _res17[3];
	u8 bcdmon;
	u8 _res18[3];
	u8 bcdyear;
#else /*  little endian */
	u8 _res0[64];
	u8 rtccon;
	u8 _res1[3];
	u8 ticnt0;
	u8 _res2[3];
	u8 ticnt2;
	u8 _res3[3];
	u8 ticnt1;
	u8 _res4[3];
	u8 rtcalm;
	u8 _res5[3];
	u8 almsec;
	u8 _res6[3];
	u8 almmin;
	u8 _res7[3];
	u8 almhour;
	u8 _res8[3];
	u8 almdate;
	u8 _res9[3];
	u8 almmon;
	u8 _res10[3];
	u8 almyear;
	u8 _res11[7];
	u8 bcdsec;
	u8 _res12[3];
	u8 bcdmin;
	u8 _res13[3];
	u8 bcdhour;
	u8 _res14[3];
	u8 bcddate;
	u8 _res15[3];
	u8 bcdday;
	u8 _res16[3];
	u8 bcdmon;
	u8 _res17[3];
	u8 bcdyear;
	u8 _res18[3];
	u8 tickcnt;
	u8 _res19[3];
#endif
};



/* ADC (see manual chapter 16) */
struct s3c2416_adc {
	u32	adccon;
	u32	adctsc;
	u32	adcdly;
	u32	adcdat0;
	u32	adcdat1;
	u32 adcupdn;
	u32 adcmux;
};


/* SPI (see manual chapter 22) */
struct s3c24xx_spi_channel {
	u8	spcon;
	u8	res1[3];
	u8	spsta;
	u8	res2[3];
	u8	sppin;
	u8	res3[3];
	u8	sppre;
	u8	res4[3];
	u8	sptdat;
	u8	res5[3];
	u8	sprdat;
	u8	res6[3];
	u8	res7[16];
};

struct s3c24xx_spi {
	struct s3c24xx_spi_channel	ch[S3C24XX_SPI_CHANNELS];
};


/* MMC INTERFACE (see S3C2400 manual chapter 19) */
struct s3c2416_mmc {
#ifdef __BIG_ENDIAN
	u8	res1[3];
	u8	mmcon;
	u8	res2[3];
	u8	mmcrr;
	u8	res3[3];
	u8	mmfcon;
	u8	res4[3];
	u8	mmsta;
	u16	res5;
	u16	mmfsta;
	u8	res6[3];
	u8	mmpre;
	u16	res7;
	u16	mmlen;
	u8	res8[3];
	u8	mmcr7;
	u32	mmrsp[4];
	u8	res9[3];
	u8	mmcmd0;
	u32	mmcmd1;
	u16	res10;
	u16	mmcr16;
	u8	res11[3];
	u8	mmdat;
#else
	u8	mmcon;
	u8	res1[3];
	u8	mmcrr;
	u8	res2[3];
	u8	mmfcon;
	u8	res3[3];
	u8	mmsta;
	u8	res4[3];
	u16	mmfsta;
	u16	res5;
	u8	mmpre;
	u8	res6[3];
	u16	mmlen;
	u16	res7;
	u8	mmcr7;
	u8	res8[3];
	u32	mmrsp[4];
	u8	mmcmd0;
	u8	res9[3];
	u32	mmcmd1;
	u16	mmcr16;
	u16	res10;
	u8	mmdat;
	u8	res11[3];
#endif
};


/* SD INTERFACE (see S3C2410 manual chapter 19) */
struct s3c24xx_sdi {
	u32	sdicon;
	u32	sdipre;
	u32	sdicarg;
	u32	sdiccon;
	u32	sdicsta;
	u32	sdirsp0;
	u32	sdirsp1;
	u32	sdirsp2;
	u32	sdirsp3;
	u32	sdidtimer;
	u32	sdibsize;
	u32	sdidcon;
	u32	sdidcnt;
	u32	sdidsta;
	u32	sdifsta;
#ifdef CONFIG_S3C2410
	u32	sdidat;
	u32	sdiimsk;
#else
	u32	sdiimsk;
	u32	sdidat;
#endif
};

#ifdef CONFIG_CMD_MMC
#include <mmc.h>
int s3cmmc_initialize(bd_t *bis, int (*getcd)(struct mmc *),int (*getwp)(struct mmc *));
#endif

#endif /*__S3C24XX_H__*/
