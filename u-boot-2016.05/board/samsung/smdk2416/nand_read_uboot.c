/*
 * (C) Copyright 2016
 * David Mueller <zhuczloveai910@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#if 0
#include <common.h>
#include <asm/io.h>
#include <asm/arch/s3c24xx_cpu.h>


int copy_code_to_sdram(unsigned int addr,unsigned char *src,unsigned int len)
{

	printf("Starting Copy U-BOOT Code To mSDRAM............\n");

	return 0;
}
#else



#define BEEP_DURATION	10000000	

/*
 *	UART
 */
#define ULCON0		(*(volatile unsigned char *)0x50000000)
#define ULCON1		(*(volatile unsigned char *)0x50004000)
#define ULCON2		(*(volatile unsigned char *)0x50008000)

#define UCON0		(*(volatile unsigned short *)0x50000004)
#define UCON1		(*(volatile unsigned short *)0x50004004)
#define UCON2		(*(volatile unsigned short *)0x50008004)

#define UFCON0		(*(volatile unsigned char *)0x50000008)
#define UFCON1		(*(volatile unsigned char *)0x50004008)
#define UFCON2		(*(volatile unsigned char *)0x50008008)

#define UMCON0		(*(volatile unsigned char *)0x5000000C)
#define UMCON1		(*(volatile unsigned char *)0x5000400C)
#define UMCON2		(*(volatile unsigned char *)0x5000800C)

#define UTRSTAT0	(*(volatile unsigned char *)0x50000010)
#define UTRSTAT1	(*(volatile unsigned char *)0x50004010)
#define UTRSTAT2	(*(volatile unsigned char *)0x50008010) 

#define UTXH0   	(*(volatile unsigned char *)0x50000020)
#define UTXH1   	(*(volatile unsigned char *)0x50004020)
#define UTXH2   	(*(volatile unsigned char *)0x50008020)

#define UBRDIV0		(*(volatile unsigned short *)0x50000028)
#define UBRDIV1		(*(volatile unsigned short *)0x50004028)
#define UBRDIV2		(*(volatile unsigned short *)0x50008028)

#define UDIVSLOT0	(*(volatile unsigned short *)0x5000002c)
#define UDIVSLOT1	(*(volatile unsigned short *)0x5000402c)
#define UDIVSLOT2	(*(volatile unsigned short *)0x5000802c)

/*
 *	GPA 
 */
#define GPACON 		(*(volatile unsigned int *)0x56000000)
#define GPADAT 		(*(volatile unsigned int *)0x56000004)

/*
 *	GPB
 */
#define GPBCON 		(*(volatile unsigned int *)0x56000010)
#define GPBDAT 		(*(volatile unsigned int *)0x56000014)
#define GPBUDP 		(*(volatile unsigned int *)0x56000018)
#define GPBSEL 		(*(volatile unsigned int *)0x5600001c)
/*
 *	GPH
 */
#define GPHCON 		(*(volatile unsigned int *)0x56000070)
#define GPHDAT 		(*(volatile unsigned int *)0x56000074)
#define GPHUDP 		(*(volatile unsigned int *)0x56000078)

/*
 *	LED & BEEP 
 * */
#define RUNLED  	0
#define PIDLED  	1
#define BEEP  		2
#define ALARMLED  	3



/*
 *	DM9000x
 *
 **/

#define DM9000A_BASE	0x20000300
#define DM9000A_PPTR	(*(volatile unsigned short *)(DM9000A_BASE))
#define DM9000A_PDATA	(*(volatile unsigned short *)(DM9000A_BASE+8))


#define DM9000A_VIDL	0x28
#define DM9000A_VIDH	0x29
#define DM9000A_PIDL	0x2a 
#define DM9000A_PIDH	0x2b
#define DM9000A_CHIP	0x2C


/*
 * NAND FLASH
 *
 * */

#define NFCONF			(*(volatile unsigned int *)0x4E000000)
#define NFCONT			(*(volatile unsigned int *)0x4E000004)
#define NFCMMD			(*(volatile unsigned char *)0x4E000008)
#define NFADDR			(*(volatile unsigned char *)0x4E00000C)
#define NFDATA			(*(volatile unsigned char *)0x4E000010)
#define NFMECCD0		(*(volatile unsigned int *)0x4E000014)
#define NFMECCD1		(*(volatile unsigned int *)0x4E000018)
#define NFSECCD			(*(volatile unsigned int *)0x4E00001C)
#define NFSBLK			(*(volatile unsigned int *)0x4E000020)
#define NFEBLK			(*(volatile unsigned int *)0x4E000024)
#define NFSTAT			(*(volatile unsigned char *)0x4E000028)
#define NFECCERR0		(*(volatile unsigned int *)0x4E00002C)
#define NFECCERR1		(*(volatile unsigned int *)0x4E000030)
#define NFMECC0			(*(volatile unsigned int *)0x4E000034)
#define NFMECC1			(*(volatile unsigned int *)0x4E000038)
#define NFSECC			(*(volatile unsigned int *)0x4E00003C)
#define NFMLCBITPT		(*(volatile unsigned int *)0x4E000040)
#define NF8ECCERR0		(*(volatile unsigned int *)0x4E000044)
#define NF8ECCERR1		(*(volatile unsigned int *)0x4E000048)
#define NF8ECCERR2		(*(volatile unsigned int *)0x4E00004C)
#define NFM8ECC0		(*(volatile unsigned int *)0x4E000050)
#define NFM8ECC1		(*(volatile unsigned int *)0x4E000054)
#define NFM8ECC2		(*(volatile unsigned int *)0x4E000058)
#define NFM8ECC3		(*(volatile unsigned int *)0x4E00005C)	
#define NFMLC8BITPT0	(*(volatile unsigned int *)0x4E000060)
#define NFMLC8BITPT1	(*(volatile unsigned int *)0x4E000064)


/* 发送命令  */
#define NF_CMD(data)		{ NFCMMD = (data);}
/* 写地址  */
#define NF_ADDR(addr)		{ NFADDR = (addr);}
/* 读一个字节  */
#define NF_READ_BYTE()		( NFDATA)
/* 写一个字节  */
#define NF_WRITE_BYTE(data) { NFDATA = (data);}
/* 使能芯片  */
#define NF_CE_ENABLE()		{ NFCONT &= ~(1 << 1);}
/* 关闭芯片  */
#define NF_CE_DISABLE()		{ NFCONT |= (1 << 1);}
/* 传输完成会置位 NFSTAT[4]  */
#define NF_WAIT_READY()		{ while(!(NFSTAT & (1 << 4)));}
/* 获得nand RnB引脚状态，1为准备号，0为忙  */
#define NF_GET_STATE_RB()	{ NFSTAT & (1 << 0);}
/* 清除nand 传输标志  */
#define NF_CLEAR_RB()		{ NFSTAT |= (1 << 4);}

//#define DEBUG

unsigned char txBuf[10] = {0x7e,0x00,0x05,0x20,0x21,0x5a,0x00,0x00,0x00,0x00};


static inline void delay(unsigned long loops)
{
	__asm__ volatile ("1: \n"
				"subs %0, %1, #1\n"
				"bne 1b":"=r"(loops):"0"(loops));
} 

void led_off(void)
{
	GPBDAT |= 0xf;
	delay(BEEP_DURATION);
}

void led_on(unsigned char led_no)
{
	switch(led_no){
	case RUNLED:
		GPBDAT &= ~(1 << 0);
	break;	
	case PIDLED:
		GPBDAT &= ~(1 << 1);
	break;	
	case BEEP:
		GPBDAT &= ~(1 << 2);
	break;	
	case ALARMLED:
		GPBDAT &= ~(1 << 3);
	break;	
	}

	delay(BEEP_DURATION);
}


void led_gpio_init(void)
{
	GPBCON &= ~(0xf << 0);	
	GPBCON |=  (0x5 << 0);

	GPBUDP &= ~(0xf << 0);
	GPBUDP |=  (0xa << 0);

	GPBCON &= ~(0xf << 0);	
	GPBCON &= ~(0xf << 4);	

	GPBCON |= (0x5 << 0);
	GPBCON |= (0x5 << 4);
}

void uart_gpio_init(void)
{
	GPHCON &= ~(0xf << 0);	
	GPHCON &= ~(0xf << 4);	

	GPHCON |= (0xa << 0);
	GPHCON |= (0xa << 4);
	GPHCON |= (0xa << 8);
	GPHCON |= (0xa << 12);

}

void uart_port_init(void)
{
	/* uart0 */
	UFCON0 = 0x0;
	UMCON0 = 0x0;
	ULCON0 = 0x03;
	UCON0  = 0x45;
	UBRDIV0 = 0x22;
	UDIVSLOT0 = 0xdfdd; 

}

void debug_printf(unsigned char *src)
{
	int i;
	for(i = 0; i < 10; i++){
		while(!(UTRSTAT0 & 0x2));
		UTXH0 = *src++;
	}

	return ;
}




static void Nand_Reset(void)
{
	NF_CE_ENABLE();
	NF_CLEAR_RB();
	NF_CMD(0xFF);
	NF_WAIT_READY();
	NF_CE_DISABLE();
}

static void Nand_Read_ID(void)
{
	NF_CE_ENABLE();
	NF_CLEAR_RB();
	NF_CMD(0x90);
	NF_ADDR(0x00);
	txBuf[0] = NF_READ_BYTE();
	txBuf[1] = NF_READ_BYTE();
	txBuf[2] = NF_READ_BYTE();
	txBuf[3] = NF_READ_BYTE();
	txBuf[4] = NF_READ_BYTE();

#ifndef DEBUG
	debug_printf(txBuf);
	delay(BEEP_DURATION);
#endif
}

static void Nand_Init(void)
{
	GPACON = (GPACON & ~(0x3f << 17))|(0x3f << 17);
	NFCONF = (1 << 12) | (2 << 8)|(1 << 4)|(0 << 0);
	NFCONT = (0<<12)|(0<<10)|(0<<9)|(0<<8)|(0x3<<6)|(0x3<<4)|(0x3<<1)|(1<<0);
	Nand_Reset();
}

static void Nand_Send_Address(unsigned int add)
{
	unsigned int col = add % 2048;
	unsigned int page = add / 2048;

	NFADDR = col & 0xff;
	NFADDR = (col >> 8) & 0xff;

	NFADDR = page & 0xff;
	NFADDR = (page >> 8) & 0xff;
	NFADDR = (page >> 16) & 0xff;

	return;
}

static int Nand_Read_Data_Buf(unsigned int addr,unsigned char *buf,unsigned int len)
{
	unsigned int col = addr % 2048;
	unsigned int i = 0;


	txBuf[0] = ((len>>24) & 0xFF);
	txBuf[1] = ((len>>16) & 0xFF);
	txBuf[2] = ((len>>8)  & 0xFF);
	txBuf[3] = (len & 0xFF);
	debug_printf(txBuf);
	delay(BEEP_DURATION);



	while(i < len){
		NF_CE_ENABLE();
		NF_CLEAR_RB();
		NF_CMD(0x00);
		Nand_Send_Address(addr);
		NF_CMD(0x30);
		NF_WAIT_READY();
		NF_CMD(0x00);
		for(;(col < 2048) && (i < len); col++){
			buf[i] = NF_READ_BYTE();
			i++;
			addr++;
		}
		col = 0;
#ifdef DEBUG 

	txBuf[0] = ((i>>24) & 0xFF);
	txBuf[1] = ((i>>16) & 0xFF);
	txBuf[2] = ((i>>8) & 0xFF);
	txBuf[3] = (i & 0xFF);
	debug_printf(buf);
	delay(BEEP_DURATION);
#endif
	}
	NF_CE_DISABLE();
	return 0;
}


int copy_code_to_sdram(unsigned char *src, unsigned char *dest, unsigned int len)
{
	/* LED */
	led_gpio_init();

	/* UARTS */
	uart_gpio_init();
	uart_port_init();

	Nand_Init();
	Nand_Read_ID();

#ifdef DEBUG

	unsigned int len_uboot = 0;
	txBuf[0] = (len & 0xFF);
	txBuf[1] = ((len>>8) & 0xFF);
	txBuf[2] = ((len>>16) & 0xFF);
	txBuf[3] = ((len>>24) & 0xFF);
	debug_printf(txBuf);
	delay(BEEP_DURATION);//uboot_len

	len_uboot = (unsigned int )dest;
	txBuf[0] = (len_uboot & 0xFF);
	txBuf[1] = ((len_uboot>>8) & 0xFF);
	txBuf[2] = ((len_uboot>>16) & 0xFF);
	txBuf[3] = ((len_uboot>>24) & 0xFF);
	debug_printf(txBuf);
	delay(BEEP_DURATION);//uboot at nand first address


	len_uboot = (unsigned int )src;
	txBuf[0] = (len_uboot & 0xFF);
	txBuf[1] = ((len_uboot>>8) & 0xFF);
	txBuf[2] = ((len_uboot>>16) & 0xFF);
	txBuf[3] = ((len_uboot>>24) & 0xFF);
	debug_printf(txBuf);
	delay(BEEP_DURATION);//uboot in sdram address

    Nand_Read_Data_Buf((unsigned int)(src),dest,len);

	GPBDAT &= ~(1 << 3);//GPB1 --- LED1 
	delay(BEEP_DURATION);
	GPBDAT |= (1 << 3);//GPB1 --- LED1
	delay(BEEP_DURATION);

//	while(1)
//	{
		GPBDAT &= ~(1 << 0);//GPB0 --- LED1 
		delay(BEEP_DURATION);
		GPBDAT |= (1 << 0);//GPB0 --- LED1
		delay(BEEP_DURATION);
//	}
#else //end of DEBUG 
//	Nand_Reset();
	led_off();
	led_on(RUNLED);
	led_on(PIDLED);
	led_on(ALARMLED);
	delay(BEEP_DURATION);
    Nand_Read_Data_Buf((unsigned int)(src),dest,len);
	led_off();

#endif 

	return 0;
}


#endif

