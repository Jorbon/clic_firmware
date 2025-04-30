#ifndef CLIC_IMAGE_H
#define CLIC_IMAGE_H

#include <stdlib.h>
#include <png.h>
#include <jpeglib.h>

#include "common.h"


#define CAMERA_WIDTH 3280
#define CAMERA_HEIGHT 2464


typedef struct {
	unsigned char* data;
	unsigned int width;
	unsigned int height;
	unsigned int channels;
} Image;


Image load_png(char* path) {
	Image img = {0};
	
	FILE* file_pointer = fopen(path, "rb");
	if (!file_pointer) {
		printf("File not found: %s\n", path);
		return img;
	}
	
	unsigned char png_signature[8];
	fread(png_signature, 1, 8, file_pointer);
	if (!png_check_sig(png_signature, 8)) {
		printf("File at '%s' is not a PNG.\n", path);
		fclose(file_pointer);
		return img;
	}
	
	png_structp png_ptr = png_create_read_struct(
			PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	
	png_init_io(png_ptr, file_pointer);
	png_set_sig_bytes(png_ptr, 8);
	png_read_info(png_ptr, info_ptr);
	
	int bit_depth, color_type;
	png_get_IHDR(png_ptr, info_ptr, &img.width, &img.height, 
			&bit_depth, &color_type, NULL, NULL, NULL);

	if (bit_depth == 16) png_set_strip_16(png_ptr);
	if (color_type == PNG_COLOR_TYPE_PALETTE) 
		png_set_palette_to_rgb(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) 
		png_set_expand_gray_1_2_4_to_8(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) 
		png_set_tRNS_to_alpha(png_ptr);
	if (color_type == PNG_COLOR_TYPE_RGB 
	 || color_type == PNG_COLOR_TYPE_GRAY 
	 || color_type == PNG_COLOR_TYPE_PALETTE) 
		png_set_filler(png_ptr, 255, PNG_FILLER_AFTER);
	if (color_type == PNG_COLOR_TYPE_GRAY 
	 || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) 
		png_set_gray_to_rgb(png_ptr);
	
	//png_set_gamma(png_ptr, display_exponent, gamma);
	png_read_update_info(png_ptr, info_ptr);
	
	unsigned int bytes_per_row = png_get_rowbytes(png_ptr, info_ptr);
	img.channels = png_get_channels(png_ptr, info_ptr);
	
	img.data = (unsigned char*) malloc(bytes_per_row * img.height);
	if (!img.data) {
		printf("Not enough memory for image buffer.\n");
		fclose(file_pointer);
		return img;
	}
	
	unsigned char* row_pointers[img.height];
	for (int i = 0; i < img.height; i++) 
		row_pointers[i] = img.data + i * bytes_per_row;
	png_read_image(png_ptr, row_pointers);
	png_read_end(png_ptr, NULL);
	
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(file_pointer);

	return img;
}


Image load_jpeg(char* path) {
	Image img = {0};
	
	FILE* file_pointer = fopen(path, "rb");
	if (!file_pointer) {
		printf("File not found: %s\n", path);
		return img;
	}
	
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, file_pointer);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);
	
	img.width = cinfo.output_width;
	img.height = cinfo.output_height;
	img.channels = cinfo.output_components;
	
	img.data = (unsigned char*) malloc(img.height * img.width * img.channels);
	if (!img.data) {
		printf("Not enough memory for image buffer.\n");
		fclose(file_pointer);
		return img;
	}
	
	unsigned char* row_pointer[1];
	for (int i = 0; i < cinfo.output_height; i++) {
		row_pointer[0] = img.data + i * img.width * img.channels;
		jpeg_read_scanlines(&cinfo, row_pointer, 1);
	}
	
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(file_pointer);
	
	return img;
}




#endif
