#ifndef CLIC_GRAPHICS_H
#define CLIC_GRAPHICS_H

#include <GLFW/glfw3.h>

#include "common.h"
#include "image.h"

const unsigned int display_width = 320;
const unsigned int display_height = 240;
const float widthf = (float) display_width;
const float heightf = (float) display_height;
const float widthf_inv = 1.0 / widthf;
const float heightf_inv = 1.0 / heightf;
const float aratio = widthf * heightf_inv;
const float aratio_inv = heightf * widthf_inv;


GLuint init_texture(Image img) {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	GLint format = GL_RGBA;
	if (img.channels == 3) format = GL_RGB;
	
	glTexImage2D(GL_TEXTURE_2D, 0, format, img.width, img.height, 
			0, format, GL_UNSIGNED_BYTE, img.data);

	return texture;
}


GLuint load_generic_image(const char* path) {
	Image img = load_png(path);
	GLuint texture = init_texture(img);
	free(img.data);
	
	return texture;
}


GLuint load_font(const char* path) {
	Image img = load_png(path);
	
	// todo: color processing
	
	GLuint texture = init_texture(img);
	free(img.data);
	
	return texture;
}



void draw_quad(float x, float y, float width, float height, 
		float u_min, float u_max, float v_min, float v_max) {
	float x1 = x * 2.0 - 1.0;
	float x2 = (x + width) * 2.0 - 1.0;
	float y1 = 1.0 - y * 2.0;
	float y2 = 1.0 - (y + height) * 2.0;
	glTexCoord2f(u_min, v_max); glVertex3f(x1, y2, 0.0f);
	glTexCoord2f(u_max, v_max); glVertex3f(x2, y2, 0.0f);
	glTexCoord2f(u_max, v_min); glVertex3f(x2, y1, 0.0f);
	glTexCoord2f(u_min, v_min); glVertex3f(x1, y1, 0.0f);
}

void draw_image(GLuint texture, float x, float y, float width, float height) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);
	draw_quad(x, y, width, height, 0.0, 1.0, 0.0, 1.0);
	glEnd();
}

void draw_text(char* text, GLuint font, float x, float y) {
	unsigned int char_size = 16;
	unsigned int char_width = 9;
	float char_sizef = (float) char_size;
	float char_widthf = (float) char_width;
	float char_sizef_inv = 1.0 / char_sizef;
	float char_widthf_inv = 1.0 / char_widthf;
	
	glBindTexture(GL_TEXTURE_2D, font);
	glBegin(GL_QUADS);
	
	char c;
	while (1) {
		c = *text;
		if (c == 0) break;
		
		float u = ((float) (c & 0b1111) + 
				(1.0 - char_widthf * char_sizef_inv) * 0.5) 
				* 0.0625;
		float v = (float) ((c >> 4) & 0b111) * 0.125;
		
		draw_quad(x, y, char_widthf * widthf_inv, 
				char_sizef * heightf_inv, 
				u, u + char_widthf * char_sizef_inv * 0.0625, 
				v, v + 0.125);
		x += char_widthf * widthf_inv;
		
		text += 1;
	}
	
	glEnd();
}


#endif