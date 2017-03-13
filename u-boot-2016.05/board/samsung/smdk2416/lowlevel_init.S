/*
 * Memory Setup stuff - taken from blob memsetup.S
 *
 * Copyright (C) 1999 2000 2001 Erik Mouw (J.A.K.Mouw@its.tudelft.nl) and
 *                     Jan-Derk Bakker (J.D.Bakker@its.tudelft.nl)
 *
 * Modified for the Samsung SMDK2410 by
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */


#include <config.h>

/* some parameters for the board */

/*
 *
 * Taken from linux/arch/arm/boot/compressed/head-s3c2410.S
 *
 * Copyright (C) 2002 Samsung Electronics SW.LEE  <hitchcar@sec.samsung.com>
 *
 */

#define BWSCON					0x48000000

#define LED1_CON_BASE 			0x56000010
#define LED1_DAT_BASE 			0x56000014


#define ELFIN_GPIO_BASE			0x56000000
#define GPADAT_OFFSET			0x04
#define ELFIN_NAND_BASE			0x4E000000
#define NFCONF_OFFSET			0x00
#define NFCONT_OFFSET			0x04


#define CFG_BANK_CFG_VAL		0x0004920d
#define CFG_BANK_CON1_VAL		0x04000040
#define CFG_BANK_CFG_VAL_ALT	0x00048904
#define CFG_BANK_CON1_VAL_ALT	0x04000040
#define CFG_BANK_CFG_VAL_DDR2	0x00049253
#define CFG_BANK_CON1_VAL_DDR2	0x44000040

#define CFG_BANK_CON2_VAL		0x0057003a
#define CFG_BANK_CON2_VAL_DDR2	0x005d0035
#define CFG_BANK_CON3_VAL		0x80000030
#define CFG_BANK_REFRESH_VAL	0x00000313 



/* BWSCON */
#define DW8				(0x0)
#define DW16			(0x1)
#define DW32			(0x2)
#define WAIT			(0x1<<2)
#define UBLB			(0x1<<3)

#define B1_BWSCON		(DW32)
#define B2_BWSCON		(DW16)
#define B3_BWSCON		(DW16 + WAIT + UBLB)
#define B4_BWSCON		(DW16)
#define B5_BWSCON		(DW16)
#define B6_BWSCON		(DW32)
#define B7_BWSCON		(DW32)

/* BANK0CON */
#define B0_Tacs			0x0	/*  0clk */
#define B0_Tcos			0x0	/*  0clk */
#define B0_Tacc			0x7	/* 14clk */
#define B0_Tcoh			0x0	/*  0clk */
#define B0_Tah			0x0	/*  0clk */
#define B0_Tacp			0x0
#define B0_PMC			0x0	/* normal */

/* BANK1CON */
#define B1_Tacs			0x0	/*  0clk */
#define B1_Tcos			0x0	/*  0clk */
#define B1_Tacc			0x7	/* 14clk */
#define B1_Tcoh			0x0	/*  0clk */
#define B1_Tah			0x0	/*  0clk */
#define B1_Tacp			0x0
#define B1_PMC			0x0

#define B2_Tacs			0x0
#define B2_Tcos			0x0
#define B2_Tacc			0x7
#define B2_Tcoh			0x0
#define B2_Tah			0x0
#define B2_Tacp			0x0
#define B2_PMC			0x0

#define B3_Tacs			0x0	/*  0clk */
#define B3_Tcos			0x3	/*  4clk */
#define B3_Tacc			0x7	/* 14clk */
#define B3_Tcoh			0x1	/*  1clk */
#define B3_Tah			0x0	/*  0clk */
#define B3_Tacp			0x3     /*  6clk */
#define B3_PMC			0x0	/* normal */

#define B4_Tacs			0x0	/*  0clk */
#define B4_Tcos			0x0	/*  0clk */
#define B4_Tacc			0x7	/* 14clk */
#define B4_Tcoh			0x0	/*  0clk */
#define B4_Tah			0x0	/*  0clk */
#define B4_Tacp			0x0
#define B4_PMC			0x0	/* normal */

#define B5_Tacs			0x0	/*  0clk */
#define B5_Tcos			0x0	/*  0clk */
#define B5_Tacc			0x7	/* 14clk */
#define B5_Tcoh			0x0	/*  0clk */
#define B5_Tah			0x0	/*  0clk */
#define B5_Tacp			0x0
#define B5_PMC			0x0	/* normal */

#define B6_MT			0x3	/* SDRAM */
#define B6_Trcd			0x1
#define B6_SCAN			0x1	/* 9bit */

#define B7_MT			0x3	/* SDRAM */
#define B7_Trcd			0x1	/* 3clk */
#define B7_SCAN			0x1	/* 9bit */

/* REFRESH parameter */
#define REFEN			0x1	/* Refresh enable */
#define TREFMD			0x0	/* CBR(CAS before RAS)/Auto refresh */
#define Trp				0x0	/* 2clk */
#define Trc				0x3	/* 7clk */
#define Tchr			0x2	/* 3clk */
#define REFCNT			1037	/* period=15.6us, HCLK=60Mhz, (2048+1-15.6*60) */
/**************************************/




/*------M DDR init*/
#define BANKCFG		0x48000000
#define BANKCON1	0x48000004
#define BANKCON2	0x48000008
#define	BANKCON3	0x4800000C



/*-------BANKCFG bit defined-------*/
#define RASBW0		2
#define RASBW1 		2
#define CASBW0		2
#define CASBW1 		2
#define ADDRCFG0	1
#define ADDRCFG1	1
#define MEMCFG		6
#define BW			1

#define BANKCFG_VAL	(BW << 0)|(MEMCFG << 1)|(ADDRCFG1 << 4)|(ADDRCFG0 << 6)|(CASBW1 << 8)|(CASBW0 << 11)|(RASBW1<<14)|(RASBW0<<17)

/*-------BANKCON1 bit defined-------*/
#define DQSlnDLL	3
#define BStop		0
#define WBUF		1
#define AP			0
#define PWRDN		1
//#define INIT

#define BANKCON1_VAL	(PWRDN << 4)|(AP << 5)|(WBUF << 6)|(BStop << 7)|(DQSlnDLL << 28)

/*-------BANKCON2 bit defined ------*/
#define tRAS		5
#define tARFC		10
#define CAS 		3
#define tRCD		2
#define tRP			2

#define BANKCON2_VAL	(tRP << 0)|(tRCD << 2)|(CAS << 4)|(tARFC << 16)|(tRAS << 20)

/*-------BANKCON3 bit define -------*/
#define BA_EMRS 	2
#define DS			0
#define PASR		0
#define BA_MRS		0
#define CAS_MRS		3

#define BANKCON3_VAL 	(CAS_MRS << 4)|(BA_MRS << 14) |(PASR << 16) | (DS << 21) |(BA_EMRS << 30) 


#define REFRESH		0x48000010
#define TIMEOUT		0x48000014
#define REFCYC		1037


.globl lowlevel_init
lowlevel_init:
/*-----------------Start clock Init----------------------*/
	ldr r0, =0x4C000000
	ldr r1, =0xFFFF
	str r1, [r0]

	ldr r0, =0x4C000004
	ldr r1, =0xFFFF
	str r1, [r0]

	ldr r0, = 0x4C000010
	ldr r1, = (1 << 0) |(3 << 5) |(400 << 14) |(0 << 24) | (0 << 25)
	str r1, [r0]

	ldr r0, = 0x4C000018
	ldr r1, = (2 << 0) |(1 << 8) |(32 << 16) | (0 << 24) | (0 << 25)
	str r1, [r0]



	ldr r0, = 0x4C000024
	ldr r1, = (1<<0)|(1 << 2)|(1 << 3)|(2 << 4)|(1 << 9)
	str r1, [r0]

	ldr r0, = 0x4C000028
	ldr r1, = (0<<4)|(3 << 6)|(0 << 8)|(0 << 12)|(0 << 16)|(0<<24)
	str r1, [r0]

	ldr r0, = 0x4C000020
	ldr r1, [r0]
	orr r1, r1, #((1 << 4) |(1 << 6))
	str r1 ,[r0]


	/* wait at least 200us to stablize all clock */ 
	ldr r1, =10000
1:	subs r1, r1, #1
	bne 1b 
/*-----------------End clock Init----------------------*/


/*-----------------Start Nand Flash Init----------------------*/
#if 0
	ldr	r0, =ELFIN_GPIO_BASE
	ldr	r1, [r0]
	orr	r1, r1, #(0x3f<<17)
	str r1, [r0]
	/* EVT1 has modification in NAND */
#if 1
	ldr	r1, =0x1aa8a
	str r1, [r0, #GPADAT_OFFSET]
#endif

	ldr	r0, =ELFIN_NAND_BASE
	ldr	r1, [r0, #NFCONF_OFFSET]
	orr	r1, r1, #0x70
	orr	r1, r1, #0x7700
	str     r1, [r0, #NFCONF_OFFSET]

	ldr	r1, [r0, #NFCONT_OFFSET]
	orr	r1, r1, #0x03
	str     r1, [r0, #NFCONT_OFFSET]
#endif
/*----------------End Nand Flash Init-------------------------*/

/*---------------------Start LED-------------------------------*/
#if 0
	ldr r0, =LED1_CON_BASE
	ldr r1, [r0]
	orr r1, r1, #(1<<10)
	str r1,[r0]

	ldr r0, =LED1_DAT_BASE
	ldr r1, [r0]
	bic r1, r1, #(1<<5)
	str r1,[r0]
#endif
/*---------------------End   LED-------------------------------*/


	/* memory control configuration */
	/* make r0 relative the current location so that it */
	/* reads SMRDATA out of FLASH rather than memory ! */
	ldr     r0, =SMRDATA
	ldr	r1, =CONFIG_SYS_TEXT_BASE
	sub	r0, r0, r1
	ldr	r1, =BWSCON	/* Bus Width Status Controller */
	add     r2, r0, #13*4
0:
	ldr     r3, [r0], #4
	str     r3, [r1], #4
	cmp     r2, r0
	bne     0b

#if 0
	ldr r0, =LED1_CON_BASE
	ldr r1, [r0]
	orr r1, r1, #(1<<10)
	str r1,[r0]

	ldr r0, =LED1_DAT_BASE
	ldr r1, [r0]
	bic r1, r1, #(1<<5)
	str r1,[r0]
#endif

/*-----------------Start sdram Init----------------------*/
	ldr r0, =BANKCFG
	ldr r1, =BANKCFG_VAL
	str r1, [r0]

	ldr r0, =BANKCON1
	ldr r1, =BANKCON1_VAL
	str r1, [r0]

	ldr r0, =BANKCON2
	ldr r1, =BANKCON2_VAL
	str r1, [r0]

	ldr r0, =BANKCON3
	ldr r1, =BANKCON3_VAL
	str r1, [r0]

	ldr r2, =BANKCON1
	ldr r1, [r2]      /* 这里r1里面的值就是BANKCON1的值*/
	bic r1, r1, #(0x3 << 0)
	orr r1, r1, #(0x1 << 0)
	str r1, [r2]

	ldr r4, =REFRESH
	ldr r0, =0xFF 
	str r0, [r4]

	mov r0, #0x100
2:
	subs r0, r0, #1
	bne 2b

	bic r1, r1, #(0x3<<0)
	orr r1, r1, #(0x2<<0)
	str r1, [r2]

	ldr r4, =REFRESH
	ldr r0, =REFCYC
	str r0, [r4]

	orr r1, r1, #(0x3<<0)
	str r1, [r2]

	bic r1, r1, #(0x3<<0)
	str r1, [r2]


/*-----------------End sdram Init----------------------*/




	/* everything is fine now */
	mov	pc, lr

	.ltorg
/* the literal pools origin */

SMRDATA:
    .word 0x0004920d	/*BANKCFG	0x48000000*/
    .word 0x44000040	/*BANKCON1	0x48000004*/
    .word 0x0057003a	/*BANKCON2	0x48000008*/
    .word 0x80000030	/*BANKCON3	0x4800000C*/
    .word 0x00000313	/*REFRESH	0x48000010*/
