
/* arch/arm/plat-samsung/include/plat/fb-s3c2416.h
 *
 * Copyright (c) 2016 zhuchengzhi <zhuczloveai910@gmail.com>
 *
 * Inspired by pxafb.h
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __ASM_PLAT_FB_S3C2416_H
#define __ASM_PLAT_FB_S3C2416_H __FILE__

struct mys3c2416fb_hw {
	unsigned long	vidcon0;    //0x4c800000
	unsigned long	vidcon1;
	unsigned long	vidtcon0;
	unsigned long	vidtcon1;
	unsigned long	vidtcon2;
	unsigned long   wincon0;
	unsigned long   wincon1;	//0x4c800018
//	unsigned long 	res_1[3];
	unsigned long   vidosd0a;
	unsigned long   vidosd0b;
//	unsigned long   res_2[1];
	unsigned long   vidosd1a;
	unsigned long   vidosd1b;
	unsigned long   vidosd1c;
//	unsigned long 	res_3[9];
	unsigned long   vidw00add0b0;
	unsigned long   vidw00add0b1;
	unsigned long   vidw01add0;
//	unsigned long 	res_4[3];
	unsigned long   vidw00add1b0;
	unsigned long   vidw00add1b1;
	unsigned long   vidw01add1;
//	unsigned long   res_5[3];
	unsigned long   vidw00add2b0;
	unsigned long   vidw00add2b1;
	unsigned long   vidw01add2;
//	unsigned long 	res_6[3];
	unsigned long   vidintcon;
	unsigned long   w1keycon0;
	unsigned long   w1keycon1;
	unsigned long   w2keycon0;
	unsigned long   w2keycon1;
	unsigned long   w3keycon0;
	unsigned long   w3keycon1;
	unsigned long   w4keycon0;
	unsigned long   w4keycon1;
	unsigned long 	win0map;
	unsigned long 	win1map;
//	unsigned long	res_7[3];
	unsigned long   wpalcon;//0x4c8000e4
//	unsigned long 	res_8[19];
	unsigned long 	sysifcon0;
	unsigned long   sysifcon1;
	unsigned long 	dithmode;
	unsigned long 	sifccon0;
	unsigned long   sifccon1;
	unsigned long 	sifccon2;
//	unsigned long 	res_9[7];
	unsigned long 	cputrigcon2;
//	unsigned long 	res_10[168];
	unsigned long   win0_plalette_ram;//0x4c800400-----0x4c8007fc  (0-255 address)
//	unsigned long	res_11[256];
	unsigned long   win1_plalette_ram;//0x4c800800-----0x4c800bfc  (0-255 address)
//	unsigned long   res_12[256];
};

/* LCD description */
struct mys3c2416fb_display {
	const char *name;
	/* LCD type */
	unsigned type;

	/* Screen size */
	unsigned short width;
	unsigned short height;

	/* Screen info */
	unsigned short xres;
	unsigned short yres;
	unsigned short bpp;

	unsigned pixclock;		/* pixclock in picoseconds */
	unsigned short left_margin;  /* value in pixels (TFT) or HCLKs (STN) */
	unsigned short right_margin; /* value in pixels (TFT) or HCLKs (STN) */
	unsigned short hsync_len;    /* value in pixels (TFT) or HCLKs (STN) */
	unsigned short upper_margin;	/* value in lines (TFT) or 0 (STN) */
	unsigned short lower_margin;	/* value in lines (TFT) or 0 (STN) */
	unsigned short vsync_len;	/* value in lines (TFT) or 0 (STN) */

	/* lcd configuration registers */
	struct mys3c2416fb_hw 		*mylcd_hw; /*TODO zhuchengzhi 2016-11-28*/
};

struct mys3c2416fb_mach_info {

	struct mys3c2416fb_display *displays;	/* attached diplays info */
	unsigned num_displays;			/* number of defined displays */
	unsigned default_display;

	/* GPIOs */

	unsigned long	gpcup;
	unsigned long	gpcup_mask;
	unsigned long	gpccon;
	unsigned long	gpccon_mask;
	unsigned long	gpdup;
	unsigned long	gpdup_mask;
	unsigned long	gpdcon;
	unsigned long	gpdcon_mask;

	/* lpc3600 control register */
	unsigned long	lpcsel;
};

extern void __init mys3c2416_fb_set_platdata(struct mys3c2416fb_mach_info *);

#endif /* __ASM_PLAT_FB_S3C2416_H */

