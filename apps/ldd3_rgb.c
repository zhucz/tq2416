#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>


int main(int argc,char *argv[])
{
	int fbfb = 0;
	struct fb_var_screeninfo vinfo;
	unsigned long screensize = 0;
	char *fbp = 0;
	int x = 0,y = 0;
	int i = 0;

	fbfb = open("/dev/fb0",O_RDWR);
	if(!fbfb){
		printf("Error : cannot open framebuffer device. \n");
		exit(1);
	}
	printf(" open framebuffer device successfully. \n");

	if(ioctl(fbfb, FBIOGET_VSCREENINFO, &vinfo)){
		printf("Error : reading variable information. \n");
		exit(1);
	}
	printf("%dx%d, %dbpp\n",vinfo.xres,vinfo.yres,vinfo.bits_per_pixel);

	if(vinfo.bits_per_pixel != 16){
		printf("Error:not supported bits_per_pixel,it only supports 16bit color %d \n",vinfo.bits_per_pixel);
		exit(1);
	}
	
	screensize = vinfo.xres * vinfo.yres * 2;
	printf("Error:not supported bits_per_pixel,it only supports 16bit color %ld \n",screensize);
	
	fbp = (char *)mmap(0,screensize,PROT_READ|PROT_WRITE,MAP_SHARED,fbfb,0);
	if((int)fbp  == -1){
		printf("Error: failed to map framebuffer device to memory \n");
		exit(1);
	}
	printf("map framebuffer device to memory successfully\n");
	
	for(i = 0; i < 3; i++){
		for(y = i * (vinfo.yres / 3); y < (i + 1) * (vinfo.yres / 3); y++){
			for(x = 0; x < vinfo.xres; x++){
				long location = x * 2 + y * vinfo.xres * 2;
				int r = 0, g = 0, b = 0;
				unsigned short rgb;

				if(i == 0)
					r = ((x * 1.0) / vinfo.xres) * 32;
				if(i == 1)
					g = ((x * 1.0) / vinfo.xres) * 64;
				if(i == 2)
					b = ((x * 1.0) / vinfo.xres) * 32;

				rgb = (r<<11)|(g<<5)|(b<<0);

				*((unsigned short *)(fbp + location)) = rgb;
			//	printf("%#x \n",rgb);
			}
		}
	}
	munmap(fbp, screensize);
	close(fbfb);
	return 0 ;

}
