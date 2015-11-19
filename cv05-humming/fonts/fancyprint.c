#include <stdio.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdint.h>
//#include <opencv/cv.h>
//#include <opencv/highgui.h>
#include <time.h>
#include "font8x8-master/font8x8.h"
#include<string.h>

uint8_t pixels[] = {
	0x00,
	0x1F, 0x1F, 0x1F, 0x1F, 0x14, 0x03, 0x00, 0x00,
	0x00, 0x00, 0x03, 0x12, 0x1F, 0x1F, 0x1F, 0x1F,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,

	0x1F, 0x1F, 0x1F, 0x12, 0x03, 0x00, 0x00, 0x00,
	0x00, 0x04, 0x14, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1D,

	0x1F, 0x1F, 0x11, 0x02, 0x00, 0x00, 0x00, 0x00,
	0x05, 0x15, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0B,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x1F, 0x1F,

	0x1F, 0x0F, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x17, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0A, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0A, 0x1F, 0x1F, 0x1F,

	0x0E, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
	0x1F, 0x1F, 0x1F, 0x1F, 0x1D, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x0B, 0x1F, 0x1F, 0x1F, 0x1F,

	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x14,
	0x1F, 0x1F, 0x1F, 0x1B, 0x07, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x0C, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,

	0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x15, 0x1F,
	0x1F, 0x1F, 0x19, 0x06, 0x00, 0x00, 0x00, 0x00,
	0x02, 0x0E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x12,

	0x00, 0x00, 0x00, 0x00, 0x05, 0x17, 0x1F, 0x1F,
	0x1F, 0x17, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0F, 0x02,
};

uint8_t array[192];


int main(int argc, char **argv)
{
	int file;
	const char *filename = "/dev/i2c-2";
	if ((file = open(filename, O_RDWR)) < 0)
	{
		/* ERROR HANDLING: you can check errno to see what went wrong */
		perror("Failed to open the i2c bus");
		exit(1);
	}
	int addr = 0x46;          // The I2C address
	if (ioctl(file, I2C_SLAVE, addr) < 0)
	{
		printf("Failed to acquire bus access and/or talk to slave.\n");
		/* ERROR HANDLING; you can check errno to see what went wrong */
		exit(1);
	}
	#define RED 0
	#define GREEN 8
	#define BLUE  16  

	int textlen=strlen(argv[1]);
	char* text = (char*)malloc((textlen+2)*sizeof(char));

	strncpy(text+1, argv[1], textlen);
	text[0]=0;
	text[textlen+1]=0;
	
	//for each character (even first blank) * its pixels
	for(int l=0; l<(textlen+1)*8; l++)
	{
		
		//pixels of 2 following chars
		//those are arrays of ints representing rows (length 8)
		char *first = font8x8_basic[text[l/8]];
		char *second = font8x8_basic[text[l/8+1]];

		if(l%8==0)
		{
			printf("Going to new chars: '%c' and '%c'!\n", text[l/8], text[l/8+1]);
		}

		char offset[8];
		memcpy(offset, first, 8);
		//do the offset
		for(int i=0; i<8; i++)
		{
			char firstpart=offset[i]>>(l%8);
			char secondpart=second[i]<<(8-(l%8));
			char newval=firstpart|secondpart;
			printf("of=%02x sc=%02x fp=%02x sp=%02x nw=%02x\n", offset[i], second[i], firstpart, secondpart, newval);
			offset[i]=newval;
		}
		printf("------------------\n");
	/*	printf("  Printing first, second and offset:\n");
		for(int i=0; i<8; i++)
		{
			printf("    %2x  %2x %2x\n", first[i], second[i], offset[i]);
		}
		*/
		//clear
		for (int i = 0 ; i<8; i++)
			for(int j = 0 ; j<8; j++)
			{
				pixels[1+i+(j*8*3)+RED  ]=0;
				pixels[1+i+(j*8*3)+GREEN]=0;
				pixels[1+i+(j*8*3)+BLUE ]=0;
			}

		int set=0;
		
		//set colors
		char R=5;
		char G=10;
		char B=60;

		//set color to pixel if necessary
		for (int i = 0 ; i<8; i++)
			for(int j = 0 ; j<8; j++)
			{
				set = offset[i] & 1 << (7-j);
				if(set)
				{
					pixels[1+i+(j*8*3)+RED  ]=R;
					pixels[1+i+(j*8*3)+GREEN]=G;
					pixels[1+i+(j*8*3)+BLUE ]=B;
				}
			}

		//write changes
		if (write(file,pixels,sizeof(pixels)) != sizeof(pixels) )
		{
			printf("Failed to write to the i2c bus.\n");
			exit(1);
		}
		usleep(10000);
		printf("- - First=%c (%02x), Second=%c (%02x)\n", text[l/8], text[l/8], text[l/8+1], text[l/8+1]);
	}
	close(file);
}

