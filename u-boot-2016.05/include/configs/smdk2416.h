/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 * Gary Jennejohn <garyj@denx.de>
 * David Mueller <d.mueller@elsoft.ch>
 *
 * Configuation settings for the SAMSUNG SMDK2416 board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#if 0
#define DEBUG	1
#endif

#if 0
#define S3C_NAND_DEBUG
#define CONFIG_MTD_DEBUG 
#define CONFIG_MTD_DEBUG_VERBOSE 3
#endif

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_S3C24XX		/* This is a SAMSUNG S3C24xx-type SoC */
#define CONFIG_S3C2416		/* specifically a SAMSUNG S3C2416 SoC */
#define CONFIG_SMDK2416		/* on a SAMSUNG SMDK2416 Board */


#if 1
#define CONFIG_AUTO_COMPLETE
#if 1
#define CONFIG_BOOTCOMMAND 		"ping 192.168.30.200"
#endif
#endif


#if 0
/*running in SDRAM JTAG */
/*#define CONFIG_SKIP_LOWLEVEL_INIT*/
#define CONFIG_SYS_TEXT_BASE	0x30008000
#else 
/*SPL*/
#define CONFIG_SPL_TEXT_BASE	0x0
#define CONFIG_SPL_MAX_SIZE		0x1000
#define CONFIG_SPL_STACK		0x8192

#define CONFIG_UBOOT_MTD_ADDR	0x20000
#define CONFIG_UBOOT_LENGTH		0x100000 
#define CONFIG_SYS_TEXT_BASE	0x30008000
/*#define CONFIG_SKIP_LOWLEVEL_INIT*/
#endif

#define CONFIG_SYS_ARM_CACHE_WRITETHROUGH

/* input clock of PLL (the SMDK2416 has 12MHz input clock) */
#define CONFIG_SYS_CLK_FREQ	12000000
#define CONFIG_SYS_HZ	1000 

#define CONFIG_CMDLINE_TAG	/* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG


/*
 * Hardware drivers
 */
#if 0
#define CONFIG_CS8900		/* we have a CS8900 on-board */
#define CONFIG_CS8900_BASE	0x19000300
#define CONFIG_CS8900_BUS16	/* the Linux driver does accesses as shorts */
#endif

/*#define CONFIG_DM9000_DEBUG			1*/
#define CONFIG_DM9000_BASE			0x20000300
#define CONFIG_DRIVER_DM9000				1

#define DM9000_IO 					CONFIG_DM9000_BASE
#define DM9000_DATA					(CONFIG_DM9000_BASE + 8)
#define CONFIG_DM9000_USE_16BIT
#define CONFIG_NET_MULTI			
#define CONFIG_DM9000_NO_SROM		1


/*
 * select serial console configuration
 */
#define CONFIG_S3C24XX_SERIAL
#define CONFIG_SERIAL1		1	/* we use SERIAL 1 on SMDK2416 */

/************************************************************
 * USB support (currently only works with D-cache off)
 ************************************************************/
#if 1
#define CONFIG_USB_OHCI
#define CONFIG_USB_OHCI_S3C24XX
#define CONFIG_USB_KEYBOARD
#define CONFIG_USB_STORAGE
#define CONFIG_DOS_PARTITION
#endif

/************************************************************
 * RTC
 ************************************************************/
#define CONFIG_RTC_S3C24XX

#define CONFIG_BAUDRATE		115200

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME

/*
 * Command line configuration.
 */
#define CONFIG_CMD_BSP
#define CONFIG_CMD_DATE
#define CONFIG_CMD_NAND
#define CONFIG_CMD_REGINFO

#define CONFIG_CMDLINE_EDITING
/* autoboot */
#define CONFIG_BOOTDELAY	5
#define CONFIG_BOOT_RETRY_TIME	-1
#define CONFIG_RESET_TO_RETRY
#define CONFIG_ZERO_BOOTDELAY_CHECK

#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_IPADDR		192.168.30.220
#define CONFIG_SERVERIP		192.168.30.200
#define CONFIG_ETHADDR 		00:40:5C:26:0A:5B
#define CONFIG_BOOTARGS "root=nfs rw nfsroot=192.168.30.128:/home/tq2416/nfsroot/rootfs/ init=/linuxrc console=ttySAC0,115200 ip=192.168.30.200"




#if defined(CONFIG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	115200	/* speed to run kgdb serial port */
#endif

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_CBSIZE	256
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT)+16)
#define CONFIG_SYS_MAXARGS	16
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE

#define CONFIG_DISPLAY_CPUINFO				/* Display cpu info */

#define CONFIG_SYS_MEMTEST_START	0x30000000	/* memtest works on */
#define CONFIG_SYS_MEMTEST_END		0x33F00000	/* 63 MB in DRAM */

#define CONFIG_SYS_LOAD_ADDR		0x30800000

/* support additional compression methods */
#define CONFIG_BZIP2
#define CONFIG_LZO
#define CONFIG_LZMA

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	1          /* we have 1 bank of DRAM */
#define PHYS_SDRAM_1			0x30000000 /* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE		0x08000000 /* 128 MB */

#define PHYS_FLASH_1			0x00000000 /* Flash Bank #0 */

#define CONFIG_SYS_FLASH_BASE	PHYS_FLASH_1

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */

#define CONFIG_SYS_FLASH_CFI
#define CONFIG_FLASH_CFI_DRIVER
#define CONFIG_FLASH_CFI_LEGACY
#define CONFIG_SYS_FLASH_LEGACY_512Kx16
#define CONFIG_FLASH_SHOW_PROGRESS			45

#define CONFIG_SYS_MAX_FLASH_BANKS			1
#define CONFIG_SYS_FLASH_BANKS_LIST     	{ CONFIG_SYS_FLASH_BASE }
#define CONFIG_SYS_MAX_FLASH_SECT			(1024)

#define CONFIG_ENV_ADDR						(CONFIG_SYS_FLASH_BASE)
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_SIZE						0x20000
/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE 

#if 1
#define CONFIG_ENV_OFFSET 0x0FFC0000
#define CONFIG_ENV_RANGE  0x20000
#else
#define CONFIG_ENV_OFFSET 0x0FFC0000
#define CONFIG_ENV_RANGE  0x20000
#endif 

#define CONFIG_NAND_BL1_8BIT_ECC


/*
 * Size of malloc() pool
 * BZIP2 / LZO / LZMA need a lot of RAM
 */
#define CONFIG_SYS_MALLOC_LEN	(4 * 1024 * 1024)

#define CONFIG_SYS_MONITOR_LEN	(448 * 1024)
#define CONFIG_SYS_MONITOR_BASE	CONFIG_SYS_FLASH_BASE

/*
 * NAND configuration
 */
#ifdef CONFIG_CMD_NAND
#define CONFIG_NAND_S3C2416
#if 0
#define CONFIG_SYS_S3C2416_NAND_HWECC
#endif
#define CFG_MAX_NAND_DEVICE				1
#define CFG_NAND_BASE					0x4E000000 
#define CONFIG_SYS_MAX_NAND_DEVICE		CFG_MAX_NAND_DEVICE
#define CONFIG_SYS_NAND_BASE			CFG_NAND_BASE
#endif

/*
 * File system
 */
#if 1
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_YAFFS2
#define CONFIG_RBTREE
#endif

/* additions for new relocation code, must be added to all boards */
#define CONFIG_SYS_SDRAM_BASE			PHYS_SDRAM_1
#define CONFIG_SYS_INIT_SP_ADDR			(CONFIG_SYS_SDRAM_BASE + 0x1000 - GENERATED_GBL_DATA_SIZE)

#define CONFIG_BOARD_EARLY_INIT_F

#endif /* __CONFIG_H */
