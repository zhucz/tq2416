/*
 * (C) Copyright 2010
 * David Mueller <d.mueller@elsoft.ch>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/s3c24xx_cpu.h>

#define GPBCONF		(*(volatile unsigned long *)0x56000010)
#define GPBDAT		(*(volatile unsigned long *)0x56000014)


typedef ulong (*getfreq)(void);

static const getfreq freq_f[] = {
	get_FCLK,
	get_HCLK,
	get_PCLK,
};

static const char freq_c[] = { 'F', 'H', 'P' };

int print_cpuinfo(void)
{

//	GPBCONF |= (1 << 12);
//	GPBDAT &= ~(1 << 6);
//	while(1);

#if ! defined(CONFIG_SPL_BUILD)
	int i;
	char buf[32];
/* the S3C2416 seems to be lacking a CHIP ID register */
	ulong cpuid;
	struct s3c24xx_gpio * const gpio = s3c24xx_get_base_gpio();

	cpuid = readl(&gpio->gstatus[1]);
	printf("CPUID: %8lX\n", cpuid);

	for (i = 0; i < ARRAY_SIZE(freq_f); i++){
		printf("%cCLK: %8s MHz\n", freq_c[i], strmhz(buf, freq_f[i]()));
	}
#endif

	return 0;
}
