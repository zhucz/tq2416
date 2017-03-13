/*
 * (C) Copyright 2003
 * David Müller ELSOFT AG Switzerland. d.mueller@elsoft.ch
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/************************************************
 * NAME	    : s3c2416.h
 * Version  : 31.3.2003
 *
 * Based on S3C2410X User's manual Rev 1.1
 ************************************************/

#ifndef __S3C2416_H__
#define __S3C2416_H__

#define S3C24XX_UART_CHANNELS	3
#define S3C24XX_SPI_CHANNELS	2

/* S3C2416 only supports 512 Byte HW ECC */
#define S3C2416_ECCSIZE		512
#define S3C2416_ECCBYTES	3

enum s3c24xx_uarts_nr {
	S3C24XX_UART0,
	S3C24XX_UART1,
	S3C24XX_UART2
};

/* S3C2416 device base addresses */
#define S3C24XX_MEMCTL_BASE			0x48000000
#define S3C24XX_USB_HOST_BASE		0x49000000
#define S3C24XX_INTERRUPT_BASE		0x4A000000
#define S3C24XX_DMA_BASE			0x4B000000
#define S3C24XX_CLOCK_POWER_BASE	0x4C000000
#define S3C24XX_LCD_BASE			0x4D000000
#define S3C2416_LCD_BASE			0x4C800000
#define S3C2416_NAND_BASE			0x4E000000
#define S3C2416_EBI					0x4E800000
#define S3C2416_SSMC				0x4F000000
#define S3C24XX_UART_BASE			0x50000000
#define S3C24XX_TIMER_BASE			0x51000000
#define S3C24XX_USB_DEVICE_BASE		0x52000140
#define S3C24XX_WATCHDOG_BASE		0x53000000
#define S3C24XX_I2C_BASE			0x54000000
#define S3C24XX_I2S_BASE			0x55000000
#define S3C24XX_GPIO_BASE			0x56000000
#define S3C24XX_RTC_BASE			0x57000000
#define S3C2416_ADC_BASE			0x58000000
#define S3C24XX_SPI_BASE			0x59000000
#define S3C2416_SPI_BASE			0x52000000
#define S3C2416_SDI_BASE			0x5A000000




/* include common stuff */
#include <asm/arch/s3c24xx.h>

static inline struct s3c24xx_ebi *s3c24xx_get_base_ebi(void)
{
	return (struct s3c24xx_ebi *)S3C2416_EBI;
}

static inline struct s3c24xx_ssmc *s3c24xx_get_base_ssmc(void)
{
	return (struct s3c24xx_ssmc *)S3C2416_SSMC;
}

static inline struct s3c24xx_memctl *s3c24xx_get_base_memctl(void)
{
	return (struct s3c24xx_memctl *)S3C24XX_MEMCTL_BASE;
}

static inline struct s3c24xx_usb_host *s3c24xx_get_base_usb_host(void)
{
	return (struct s3c24xx_usb_host *)S3C24XX_USB_HOST_BASE;
}

static inline struct s3c24xx_interrupt *s3c24xx_get_base_interrupt(void)
{
	return (struct s3c24xx_interrupt *)S3C24XX_INTERRUPT_BASE;
}

static inline struct s3c24xx_dmas *s3c24xx_get_base_dmas(void)
{
	return (struct s3c24xx_dmas *)S3C24XX_DMA_BASE;
}

static inline struct s3c24xx_clock_power *s3c24xx_get_base_clock_power(void)
{
	return (struct s3c24xx_clock_power *)S3C24XX_CLOCK_POWER_BASE;
}

static inline struct s3c24xx_lcd *s3c24xx_get_base_lcd(void)
{
	return (struct s3c24xx_lcd *)S3C2416_LCD_BASE;
}

static inline struct s3c24xx_nand *s3c24xx_get_base_nand(void)
{
	return (struct s3c24xx_nand *)S3C2416_NAND_BASE;
}

static inline struct s3c24xx_uart *s3c24xx_get_base_uart(enum s3c24xx_uarts_nr n)
{
	return (struct s3c24xx_uart *)(S3C24XX_UART_BASE + (n * 0x4000));
}

static inline struct s3c24xx_timers *s3c24xx_get_base_timers(void)
{
	return (struct s3c24xx_timers *)S3C24XX_TIMER_BASE;
}

static inline struct s3c24xx_usb_device *s3c24xx_get_base_usb_device(void)
{
	return (struct s3c24xx_usb_device *)S3C24XX_USB_DEVICE_BASE;
}

static inline struct s3c24xx_watchdog *s3c24xx_get_base_watchdog(void)
{
	return (struct s3c24xx_watchdog *)S3C24XX_WATCHDOG_BASE;
}

static inline struct s3c24xx_i2c *s3c24xx_get_base_i2c(void)
{
	return (struct s3c24xx_i2c *)S3C24XX_I2C_BASE;
}

static inline struct s3c24xx_i2s *s3c24xx_get_base_i2s(void)
{
	return (struct s3c24xx_i2s *)S3C24XX_I2S_BASE;
}

static inline struct s3c24xx_gpio *s3c24xx_get_base_gpio(void)
{
	return (struct s3c24xx_gpio *)S3C24XX_GPIO_BASE;
}

static inline struct s3c24xx_rtc *s3c24xx_get_base_rtc(void)
{
	return (struct s3c24xx_rtc *)S3C24XX_RTC_BASE;
}

static inline struct s3c2416_adc *s3c2416_get_base_adc(void)
{
	return (struct s3c2416_adc *)S3C2416_ADC_BASE;
}

static inline struct s3c24xx_spi *s3c24xx_get_base_spi(void)
{
	return (struct s3c24xx_spi *)S3C2416_SPI_BASE;
}

static inline struct s3c24xx_sdi *s3c24xx_get_base_sdi(void)
{
	return (struct s3c24xx_sdi *)S3C2416_SDI_BASE;
}

#endif /*__S3C2416_H__*/
