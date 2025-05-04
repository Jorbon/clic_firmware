#ifndef CLIC_PROCESSING_H
#define CLIC_PROCESSING_H

#include "common.h"
#include "image.h"
#include "graphics.h"


int rgbfull_from_raw10(Image* rgb_img, Image raw_img) {
	if (raw_img.data == NULL) return -1;
	
	unsigned int raw_width = raw_img.width / 5 * 4;
	unsigned int raw_height = raw_img.height;
	
	rgb_img->width = raw_width;
	rgb_img->height = raw_height;
	rgb_img->channels = 3;
	
	unsigned char* data = 
		(unsigned char*) malloc(raw_width * raw_height * 3);
	rgb_img->data = data;
	
	unsigned int i = 0;
	for (int y = 0; y < raw_height; y++) {
		for (int x = 0; x < raw_width; x++) {
			data[i] = y * 256 / raw_height;
			data[i+1] = x * 256 / raw_width;
			data[i+2] = 127;
			
			i += 3;
		}
	}
	
	return 0;
}


int rgbshow_from_raw10(Image* rgb_img, Image raw_img) {
	if (raw_img.data == NULL) return -1;
	
	//for (int i = 0; i < 50; i++) {
	//	for (int j = 0; j < 10; j++) {
	//		printf("%d\t", raw_img.data[i*10 + j]);
	//	}
	//	printf("\n");
	//}
	//exit(0);
	
	unsigned int raw_width = raw_img.width / 5 * 4;
	unsigned int raw_height = raw_img.height;
	
	rgb_img->width = display_width;
	rgb_img->height = display_height;
	rgb_img->channels = 3;
	
	unsigned char* data = 
		(unsigned char*) malloc(display_width * display_height * 3);
	rgb_img->data = data;
	
	unsigned int i = 0;
	float xf = 0.0;
	float yf = 0.0;
	float xf_inc = widthf_inv;
	float yf_inc = heightf_inv;
	
	for (int y = 0; y < display_height; y++) {
		int raw_y = (int)(yf * raw_height) & (~1);
		int raw_yi0 = raw_y * raw_img.width;
		int raw_yi1 = raw_yi0 + raw_img.width;
		
		for (int x = 0; x < display_width; x++) {
			int raw_xi = ((int)(xf * raw_width) >> 2) * 5;
			
			unsigned char r = raw_img.data[raw_yi0 + raw_xi];
			unsigned char g = raw_img.data[raw_yi1 + raw_xi];
			unsigned char b = raw_img.data[raw_yi1 + raw_xi + 1];
			
			data[i] = r;
			data[i+1] = g;
			data[i+2] = b;
			
			i += 3;
			xf += xf_inc;
		}
		
		xf = 0.0;
		yf += yf_inc;
	}
	
	return 0;
}







#endif
