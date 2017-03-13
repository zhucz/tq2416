/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002, 2010
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <netdev.h>
#include <asm/io.h>
#include <asm/arch/s3c24xx_cpu.h>

DECLARE_GLOBAL_DATA_PTR;

#define FCLK_SPEED 1

#if (FCLK_SPEED == 0)		/* Fout = 400MHz, Fin = 12MHz for Audio */
#define M_MDIV	0x190
#define M_PDIV	0x3
#define M_SDIV	0x1
#elif (FCLK_SPEED == 1)		/* Fout = 202.8MHz */
#define M_MDIV	0x190
#define M_PDIV	0x3
#define M_SDIV	0x1
#endif

#define USB_CLOCK 1

#if (USB_CLOCK == 0)
#define U_M_MDIV	0xA1
#define U_M_PDIV	0x3
#define U_M_SDIV	0x1
#elif (USB_CLOCK == 1)
#define U_M_MDIV	0x48
#define U_M_PDIV	0x3
#define U_M_SDIV	0x2
#endif


static unsigned short FrameBffer[480*272];
unsigned short *pBuffer;


//static void uart0_tx(unsigned char dat);

static inline void pll_delay(unsigned long loops)
{
	__asm__ volatile ("1:\n"
	  "subs %0, %1, #1\n"
	  "bne 1b" : "=r" (loops) : "0" (loops));
}

/*
 * Miscellaneous platform dependent initialisations
 */


#if 0
static int my_s3c2416_nand_enable_cs(void)
{
	struct s3c24xx_nand *nand_reg = s3c24xx_get_base_nand();
	unsigned int cur = 0;

	cur = readl(&nand_reg->nfcont);
	cur &= ~(1 << 1);
	writel(cur, &nand_reg->nfcont);

	return 0;
}

static int my_s3c2416_nand_disable_cs(void)
{
	struct s3c24xx_nand *nand_reg = s3c24xx_get_base_nand();
	unsigned int cur = 0;

	cur = readl(&nand_reg->nfcont);
	cur |= (1 << 1);
	writel(cur, &nand_reg->nfcont);

	return 0;
}

static int my_s3c2416_write_addr(unsigned int address)
{
	struct s3c24xx_nand *nand_reg = s3c24xx_get_base_nand();
	writeb(address,&nand_reg->nfaddr);
	return 0;
}

static int my_s3c2416_write_command(unsigned char cmd)
{
	struct s3c24xx_nand *nand_reg = s3c24xx_get_base_nand();
	writeb(cmd,&nand_reg->nfcmmd);
	return 0;
}

static int my_s3c2416_nand_wait_ready(void)
{
	struct s3c24xx_nand *nand = s3c24xx_get_base_nand();
	while(!((readl(&nand->nfstat)) & (1 << 4))){
	}
	return 1;

}

static int my_s3c2416_nand_Rnb(void)
{
	struct s3c24xx_nand *nand_reg = s3c24xx_get_base_nand();
	u_int32_t cfg_v;

	cfg_v = 0;
	cfg_v = readl(&nand_reg->nfstat);
	cfg_v |= (1 << 4);
	writel(cfg_v,&nand_reg->nfstat);
	return 0;
}

static int my_s3c2416_nand_reset(void)
{
	my_s3c2416_nand_enable_cs();
	my_s3c2416_nand_Rnb();
	my_s3c2416_write_command(0xFF);
	my_s3c2416_nand_wait_ready();	
	my_s3c2416_nand_disable_cs();
	return 0;
}

static int my_s3c2416_nand_device_ready(void)
{
	struct s3c24xx_nand *nand = s3c24xx_get_base_nand();
	while(!((readl(&nand->nfstat)) & (1 << 0))){}
	return 1;
}

static int my_s3c2416_nand_init(void)
{
	u_int32_t cfg;
	struct s3c24xx_nand *nand_reg = s3c24xx_get_base_nand();
	struct s3c24xx_gpio * const gpio = s3c24xx_get_base_gpio();
	
	cfg = 0;
	cfg = readl(&gpio->gpacon);
	cfg |= (0x3f << 17);
	writel(cfg,&gpio->gpacon);

	cfg = 0;
	cfg = readl(&nand_reg->nfconf);
	cfg |= (1<<12)|(2<<8)|(1<<4)|(0<<3)|(0<<2)|(1<<1)|(0<<0);
	writel(cfg, &nand_reg->nfconf);


	cfg = 0;
	cfg = readl(&nand_reg->nfcont);
	cfg |=(0<<12)|(0<<10)|(0<<9)|(0<<8)|(0x3<<6)|(0x3<<4)|(0x3<<1)|(1<<0);
	writel(cfg, &nand_reg->nfcont);


	my_s3c2416_nand_reset();

	return 0;
}

static int my_s3c2416_nand_erase(unsigned int block)
{

	struct s3c24xx_nand *nand_reg = s3c24xx_get_base_nand();
	unsigned char status = 0;


	my_s3c2416_nand_enable_cs();
	my_s3c2416_nand_Rnb();	


	my_s3c2416_write_command(0x60);
	my_s3c2416_write_addr((block << 6)&0xff);
	my_s3c2416_write_addr((block >> 2)& 0xff);
	my_s3c2416_write_addr((block >> 10)&0xff);
	my_s3c2416_write_command(0xD0); 
	my_s3c2416_nand_wait_ready();			//BUSY
//	my_s3c2416_nand_device_ready();

	my_s3c2416_write_command(0x70);
	do{
		status = readb(&nand_reg->nfdata); 
	}while(!(status & (1 << 6)));								   

	if(status & (1 << 0)){	//I/O0 = 0 Successful Erase 
				        //I/O0 = 1 Error n Erase 
		printf("%s : %d erase error status = %x \n",__func__,__LINE__,status);
		return 1;
	}
	printf("%s : %d erase ok status = %x \n",__func__,__LINE__,status);
	return 0;
}

static int my_s3c2416_nand_read(int block,int page)
{

	struct s3c24xx_nand *nand_reg = s3c24xx_get_base_nand();
	unsigned int i = 0;
	unsigned char data[20] = {'\0'};

	int page_addr = (block * 64) + page;


	printf("%s : %d s3c_nand_read start............... \n",__func__,__LINE__);

	my_s3c2416_nand_enable_cs();
	my_s3c2416_nand_Rnb();	

	my_s3c2416_write_command(0x00);
	my_s3c2416_write_addr(0);
	my_s3c2416_write_addr(0);
	my_s3c2416_write_addr(page_addr & 0xff);
	my_s3c2416_write_addr((page_addr >> 8)& 0xff);
	my_s3c2416_write_addr((page_addr >> 16)&0xff);
	my_s3c2416_write_command(0x30);
	my_s3c2416_nand_wait_ready();			
//	my_s3c2416_nand_device_ready();
	for(i = 0;i<10;i++){
		data[i] = readb(&nand_reg->nfdata); //I/O0 = 0 Successful Erase 
	}

	for(i = 0;i<10;i++){
		uart0_tx(data[i]);
		pll_delay(8000);
	}

	return 0;
}


static int my_s3c2416_nand_write(int block, int page)
{
	struct s3c24xx_nand *nand_reg = s3c24xx_get_base_nand();
	unsigned char w_data[2048] = {'\0'};
	unsigned int i = 0;
	unsigned char status = 0;



	for(i=0;i<2048;i++){
		w_data[i] = i;
	//	printf("%s : %d w_data[%d] = %x \n",__func__,__LINE__,i,w_data[i]);
	}

	int page_addr = (block * 64) + page;

	printf("%s : %d s3c_nand_write start............... \n",__func__,__LINE__);

	my_s3c2416_nand_enable_cs();
	my_s3c2416_nand_Rnb();

	my_s3c2416_write_command(0x80);
	my_s3c2416_write_addr(0);
	my_s3c2416_write_addr(0);
	my_s3c2416_write_addr(page_addr & 0xff);
	my_s3c2416_write_addr((page_addr >> 8)& 0xff);
	my_s3c2416_write_addr((page_addr >> 16)&0xff);

	for(i=0;i<2048;i++){
		writeb(w_data[i],&nand_reg->nfdata);
	}
	my_s3c2416_write_command(0x10);
	my_s3c2416_nand_wait_ready();		

	my_s3c2416_write_command(0x70);
	do{
		status = readb(&nand_reg->nfdata); 
	}while(!(status & (1 << 6)));								   
										
	if(status & 0x01){	//I/O0 = 0 Successful Erase 
				        //I/O0 = 1 Error n Erase 
		printf("%s : %d erase error status = %x \n",__func__,__LINE__,status);
		return 1;
	}
	printf("%s : %d erase ok status = %x \n",__func__,__LINE__,status);
	return 0;
}
static void uart0_tx(unsigned char dat)
{
	struct s3c24xx_uart * const uart_reg = s3c24xx_get_base_uart(0);
	while(!(readb(&uart_reg->utrstat) & 0x02));
	writeb(dat,&uart_reg->utxh);
}
#endif

static unsigned short *get_framebuffer_add(void)
{
	return FrameBffer;
}


/**************lcd start****************/
static void mys3c2416_lcd_init(void)
{
	struct s3c24xx_gpio * const gpio = s3c24xx_get_base_gpio();
	struct s3c24xx_lcd  * const lcd  = s3c24xx_get_base_lcd();

	unsigned char gImage_picture[768000] = {
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x85,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x7d,0x46,0x7d,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x7d,0x46,0x7d,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x7d,0x46,0x7d,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x7d,0x46,0x7d,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x7d,0x46,0x7d,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x7d,0x46,0x7d,
	0x26,0x85,0x26,0x85,0x26,0x85,0x46,0x85,0x46,0x85,0x46,0x85,0x46,0x7d,0x46,0x7d,
	
	};


	unsigned int reg_vidcon0 = 0;
	unsigned int reg_vidcon1 = 0;
	unsigned int reg_vidtcon0 = 0;
	unsigned int reg_vidtcon1 = 0;
	unsigned int reg_vidtcon2 = 0;
	unsigned int reg_wincon0 = 0;
	unsigned int reg_wincon1 = 0;
	unsigned int reg_vidosd0a = 0;
	unsigned int reg_vidosd0b = 0;
	unsigned int reg_vidosd1a = 0;
	unsigned int reg_vidosd1b = 0;
	unsigned int reg_vidosd1c = 0;
	unsigned int reg_vidw00add2b0 = 0;

	unsigned int lcd_light = 0;

	unsigned int i = 0;

	reg_vidcon0 = readl(&lcd->vidcon0);
	reg_vidcon1 = readl(&lcd->vidcon1);
	reg_vidtcon0 = readl(&lcd->vidtcon0);
	reg_vidtcon1 = readl(&lcd->vidtcon1);
	reg_vidtcon2 = readl(&lcd->vidtcon2);
	reg_wincon0 = readl(&lcd->wincon0);
	reg_wincon1 = readl(&lcd->wincon1);
	reg_vidosd0a = readl(&lcd->vidosd0a);
	reg_vidosd0b = readl(&lcd->vidosd0b);
	reg_vidosd1a = readl(&lcd->vidosd1a);
	reg_vidosd1b = readl(&lcd->vidosd1b);
	reg_vidosd1c = readl(&lcd->vidosd1c);
	reg_vidw00add2b0 = readl(&lcd->vidw00add2b0);
	lcd_light = readl(&gpio->gpcdat);


	reg_vidcon0 &= ~(0x3 << 0);
	writel(reg_vidcon0,&lcd->vidcon0);

	reg_wincon0 |= (5<<2)|(1<<16)|(0<<23);
	reg_wincon1 |= (5<<2)|(1<<16)|(1<<6);
	reg_vidcon0 |= (0<<22)|(0<<13)|(0<<12)|(2<<6)|(1<<5)|(1<<4)|(0<<2)|(0<<0);
	reg_vidcon1 |= (0<<7)|(1<<6)|(1<<5)|(0<<4);
	reg_vidtcon0 |= (1<<16)|(1<<8)|(9<<0);
	reg_vidtcon1 |= (1<<16)|(1<<8)|(40<<0);
	reg_vidtcon2 |= (271<<11)|(479<<0);
	reg_vidosd0a |= (0<<11)|(0<<0);
	reg_vidosd0b |= (479<<11)|(271<<0);
	reg_vidosd1a |= (0<<11)|(0<<0);
	reg_vidosd1b |= (479<<11)|(271<<0);
	reg_vidosd1c = 0xfff000;

	writel(reg_wincon0,&lcd->wincon0);
	writel(reg_wincon1,&lcd->wincon1);
	writel(reg_vidcon0,&lcd->vidcon0);
	writel(reg_vidcon1,&lcd->vidcon1);

	writel(reg_vidtcon0,&lcd->vidtcon0);
	writel(reg_vidtcon1,&lcd->vidtcon1);
	writel(reg_vidtcon2,&lcd->vidtcon2);
	writel(reg_vidosd1c,&lcd->vidosd1c);

//	&lcd->vidw00add0b0 = (unsigned int)FrameBffer;
//	&lcd->vidw00add1b0 = ((unsigned int)(FrameBffer + 480*272)&0xffffff);
	writel((unsigned int)FrameBffer,&lcd->vidw00add0b0);
	writel((unsigned int)(FrameBffer+480*272)&0xffffff,&lcd->vidw00add1b0);


//	virt_to_phys();

	reg_vidw00add2b0 |= (0<<13)|((480*2)<<0);
	writel(reg_vidw00add2b0,&lcd->vidw00add2b0);

	reg_wincon0 |= (1 << 1);
	writel(reg_wincon0,&lcd->wincon0);

	reg_vidcon0 |= (0x3 << 0);
	writel(reg_vidcon0,&lcd->vidcon0);

	lcd_light |= (1 << 0);
	writel(lcd_light,&gpio->gpcdat);

	pBuffer = get_framebuffer_add();

	for(i=0;i<480*272;i++){
		pBuffer[i] = ((unsigned short *)gImage_picture)[i]; 
	}


}
/**************lcd end******************/


int board_early_init_f(void)
{
	struct s3c24xx_clock_power * const clk_power = s3c24xx_get_base_clock_power();
	struct s3c24xx_gpio * const gpio = s3c24xx_get_base_gpio();

	unsigned int reg_val = 0;

	/* to reduce PLL lock time, adjust the LOCKTIME register */
	writel(0xFFFFFF, &clk_power->lockcon0);

	/* configure MPLL */
	writel((M_MDIV << 14) + (M_PDIV << 5) + M_SDIV, &clk_power->mpllcon);

	/* some delay between MPLL and UPLL */
	pll_delay(4000);

	/* configure UPLL */
	writel((U_M_MDIV << 16) + (U_M_PDIV << 8) + U_M_SDIV, &clk_power->epllcon);

	/* some delay between MPLL and UPLL */
	pll_delay(8000);

	/* set up the I/O ports */
	writel(0x007FFFFF, &gpio->gpacon);
	writel(0x00044555, &gpio->gpbcon);
	writel(0x000007FF, &gpio->gpbudp);
	writel(0xAAAAAAA9, &gpio->gpccon);
	writel(0x0000FFFF, &gpio->gpcudp);
	writel(0xAAAAAAAA, &gpio->gpdcon);
	writel(0x0000FFFF, &gpio->gpdudp);
	writel(0xAAAAAAAA, &gpio->gpecon);
	writel(0x0000FFFF, &gpio->gpeudp);
	writel(0x000055AA, &gpio->gpfcon);
	writel(0x000000FF, &gpio->gpfudp);
	writel(0xFF95FFBA, &gpio->gpgcon);
	writel(0x0000FFFF, &gpio->gpgudp);
	writel(0x002AAAAA, &gpio->gphcon);//GPH0---TX0 GPH1---RX0
	writel(0x000007AA, &gpio->gphudp);


	writel(~(0x3 << 10),&gpio->gpbcon);
	writel( (0x1 << 10),&gpio->gpbcon);
	writel(~(0x3 << 10),&gpio->gpbudp);
	writel( (0x2 << 10),&gpio->gpbudp);
	writel( (0x1 << 5),&gpio->gpbdat);


	reg_val = readl(&gpio->gpccon);
	reg_val &= ~(0x3 << 0);
	writel(reg_val,&gpio->gpccon);
	reg_val |=  (0x1 << 0);
	writel(reg_val,&gpio->gpccon);

	reg_val = readl(&gpio->gpcudp);
	reg_val &= ~(0x3 << 0);
	writel(reg_val,&gpio->gpcudp);
	reg_val |=  (0x2 << 0);
	writel(reg_val,&gpio->gpcudp);

	writel( (0x1 << 0),&gpio->gpcdat);





//	writel(~(0xf << 28),&gpio->gpecon);
//	writel( (0x5 << 28),&gpio->gpecon);
//	writel((0 << 14),&gpio->gpedat);

#if 0
	uart0_tx(0x7e);
	uart0_tx(0x1d);
/*-------------------nand test start ------------------*/
	my_s3c2416_nand_init();				

	uart0_tx(0x7e);
	uart0_tx(0x2d);
	my_s3c2416_nand_read(2046,0);

	uart0_tx(0x7e);
	uart0_tx(0x3d);

//	my_s3c2416_nand_erase(2046);

	uart0_tx(0x7e);
	uart0_tx(0x4d);
	my_s3c2416_nand_read(2046,0);


	uart0_tx(0x7e);
	uart0_tx(0x4d);
	my_s3c2416_nand_write(2046,0);

	uart0_tx(0x7e);
	uart0_tx(0x5d);
	my_s3c2416_nand_read(2046,0);

//	while(1);
#endif
/*-------------------nand test end --------------------*/


/********************lcd start*********************/
	mys3c2416_lcd_init();
/********************lcd end***********************/

	return 0;
}

int board_init(void)
{
	struct s3c24xx_ebi * const ebi = s3c24xx_get_base_ebi();
	struct s3c24xx_ssmc * const ssmc = s3c24xx_get_base_ssmc();

	/* arch number of SMDK2416-Board */
	gd->bd->bi_arch_number = MACH_TYPE_SMDK2416;

	/* adress of boot parameters */
	gd->bd->bi_boot_params = 0x30000100;
	
	icache_enable();
	dcache_enable();

	writel(~((1<<8)|(1<<9)|(1<<0)),&ebi->ebicon);
	writel(0xF,&ssmc->smbidcyr4);
	writel(12,&ssmc->smbwstrdr4);
	writel(12,&ssmc->smbwstwrr4);
	writel(2,&ssmc->smbwstoenr4);
	writel(2,&ssmc->smbwstwenr4);

	return 0;
}

int dram_init(void)
{
	/* dram_init must store complete ramsize in gd->ram_size */
	gd->ram_size = PHYS_SDRAM_1_SIZE;
	return 0;
}

#ifdef CONFIG_CMD_NET
int board_eth_init(bd_t *bis)
{
	int rc = 0;
#ifdef CONFIG_DRIVER_DM9000
	rc = dm9000_initialize(bis);
#endif
	return rc;
}
#endif

/*
 * Hardcoded flash setup:
 * Flash 0 is a non-CFI AMD AM29LV800BB flash.
 */
ulong board_flash_get_legacy(ulong base, int banknum, flash_info_t *info)
{
	info->portwidth = FLASH_CFI_16BIT;
	info->chipwidth = FLASH_CFI_BY16;
	info->interface = FLASH_CFI_X16;
	return 1;
} 
