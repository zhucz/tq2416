/*
 * (C) Copyright 2001-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, d.mueller@elsoft.ch
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/* This code should work for both the S3C2400 and the S3C2410
 * as they seem to have the same PLL and clock machinery inside.
 * The different address mapping is handled by the s3c24xx.h files below.
 */

#include <common.h>
#ifdef CONFIG_S3C24XX

#include <asm/io.h>
#include <asm/arch/s3c24xx_cpu.h>

#define MPLL 0
#define UPLL 1

/* ------------------------------------------------------------------------- */
/* NOTE: This describes the proper use of this file.
 *
 * CONFIG_SYS_CLK_FREQ should be defined as the input frequency of the PLL.
 *
 * get_FCLK(), get_HCLK(), get_PCLK() and get_UCLK() return the clock of
 * the specified bus in HZ.
 */
/* ------------------------------------------------------------------------- */

static ulong get_PLLCLK(int pllreg)
{
	struct s3c24xx_clock_power *clk_power = s3c24xx_get_base_clock_power();
	ulong r, m, p, s;

	if (pllreg == MPLL){
		r = readl(&clk_power->mpllcon);
		m = ((r >> 14) & 0x3FF);
		p = ((r >> 5)  & 0x3F);
		s = (r & 0x7);

//		printf("m=%d,p=%d,s=%d \n",m,p,s);
	}else if (pllreg == UPLL){
		r = readl(&clk_power->epllcon);
		m = ((r >> 16) & 0xFF);
		p = ((r >> 8)  & 0x3F);
		s = (r & 0x7);
	}else{
		hang();
	}

	return (((CONFIG_SYS_CLK_FREQ / 1000000) * m) / (p << s)) * 1000000;

}

/* return FCLK frequency */
ulong get_FCLK(void)
{
	return get_PLLCLK(MPLL);
}

/* return HCLK frequency */
ulong get_HCLK(void)
{
	struct s3c24xx_clock_power *clk_power = s3c24xx_get_base_clock_power();
	u32 clkdiv0;
	u16 hck1_div,pre_div;

	clkdiv0 = readl(&clk_power->clkdiv0);

	hck1_div = (((clkdiv0 >> 0) & 0x3) + 1);
	pre_div  = (((clkdiv0 >> 4) & 0x3) + 1);

	return get_FCLK()/(hck1_div * pre_div);
}

/* return PCLK frequency */
ulong get_PCLK(void)
{
	struct s3c24xx_clock_power *clk_power = s3c24xx_get_base_clock_power();

	u32 clkdiv0;

	clkdiv0 = readl(&clk_power->clkdiv0);

	return (clkdiv0 & 0x04) ? get_HCLK() /2 :get_HCLK();
}

/* return UCLK frequency */
ulong get_UCLK(void)
{
	return get_PLLCLK(UPLL);
}

#endif /* CONFIG_S3C24XX */
