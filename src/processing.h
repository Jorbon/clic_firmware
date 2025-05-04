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
	
	int i0 = 0;
	int i1 = raw_width * 3;
	int in = i1;
	
	int ir0 = 0;
	int ir1 = ir0 + raw_img.width;
	int ir2 = ir1 + raw_img.width;
	
	for (int y = 0; y < raw_height; y += 2) {
		for (int x = 0; x < raw_width; x += 4) {
			
			data[i0] = raw_img.data[ir0];
			data[i0+6] = raw_img.data[ir0+2];
			data[i0+3] = data[i0]/2 + data[i0+6]/2;
			data[i1] = data[i0]/2 + raw_img.data[ir2]/2;
			data[i1+6] = data[i0+6]/2 + raw_img.data[ir2+2]/2;
			data[i1+3] = data[i1]/2 + data[i1+6]/2;
			
			data[i0+9] = data[i0+6]/2;
			data[i1+9] = data[i1+6]/2;
			
			
			data[i1+5] = raw_img.data[ir1+1];
			data[i1+11] = raw_img.data[ir1+3];
			data[i1+8] = data[i1+5]/2 + data[i1+11]/2;
			data[i0+5] = data[i1+5]/2 + data[in+5]/2;
			data[i0+11] = data[i1+11]/2 + data[in+11]/2;
			data[i0+8] = data[i0+5]/2 + data[i0+11]/2;
			
			data[i1+2] = data[i1+5]/2;
			data[i0+2] = data[i0+5]/2;
			
			
			data[i0+4] = raw_img.data[ir0+1];
			data[i0+10] = raw_img.data[ir0+3];
			data[i1+1] = raw_img.data[ir1];
			data[i1+7] = raw_img.data[ir1+2];
			data[i1+4] = data[i1+1]/4 + data[i1+7]/4 + data[i0+4]/4 + raw_img.data[ir2+1]/4;
			data[i0+7] = data[i0+4]/4 + data[i0+10]/4 + data[i1+7]/4 + data[in+7]/4;
			
			data[i0+1] = data[in+1]/4 + data[i0+4]/4 + data[i1+1]/4;
			data[i1+10] = data[i0+10]/4 + data[i1+7]/4 + raw_img.data[ir2+3]/4;
			
			
			if (x == 0) {
				data[i0+2] += data[i0+5]/2;
				data[i1+2] += data[i1+5]/2;
				data[i0+1] += data[i0+4]/4;
			} else {
				data[i0+2] += data[i0-1]/2;
				data[i1+2] += data[i1-1]/2;
				data[i0+1] += data[i0-2]/4;
			}
			
			if (x + 4 >= raw_width) {
				data[i0+9] += data[i0+6]/2;
				data[i1+9] += data[i1+6]/2;
				data[i1+10] += data[i1+7]/4;
			} else {
				data[i0+9] += raw_img.data[ir0+5]/2;
				data[i1+9] += raw_img.data[ir0+5]/4 + raw_img.data[ir2+5]/4;
				data[i1+10] += raw_img.data[ir1+5]/4;
			}
			
			i0 += 12;
			i1 += 12;
			in += 12;
			ir0 += 5;
			ir1 += 5;
			ir2 += 5;
		}
		
		i0 += raw_width * 3;
		i1 = i0 + raw_width * 3;
		in = i0 - raw_width * 3;
		
		ir0 += raw_img.width;
		ir1 = ir0 + raw_img.width;
		ir2 = ir1 + raw_img.width;
		if (ir2 >= raw_img.width * raw_img.height) 
			ir2 = ir0;
	}
	
	return 0;
}


int rgbshow_from_raw10(Image* rgb_img, Image raw_img) {
	if (raw_img.data == NULL) return -1;;
	
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
