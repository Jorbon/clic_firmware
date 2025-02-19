#include <stdio.h>
#include <GLFW/glfw3.h>
#include <png.h>
#include <stdlib.h>
//#include <gpiod.h>



void error_callback(int error, const char* description) {
	fprintf(stderr, "GLFW Error: %s\n", description);
}


typedef struct {
	unsigned char* data;
	unsigned int width;
	unsigned int height;
	unsigned int channels;
} Image;


static const unsigned int display_width = 320;
static const unsigned int display_height = 240;
static const float widthf = (float) display_width;
static const float heightf = (float) display_height;
static const float widthf_inv = 1.0 / widthf;
static const float heightf_inv = 1.0 / heightf;
static const float aratio = widthf * heightf_inv;
static const float aratio_inv = heightf * widthf_inv;


Image load_png(const char* path) {
	Image img;
	
	FILE* file_pointer = fopen(path, "rb");
	if (!file_pointer) {
		printf("File not found: %s\n", path);
		return img;
	}
	
	unsigned char sig[8];
	fread(sig, 1, 8, file_pointer);
	if (!png_check_sig(sig, 8)) {
		printf("File at '%s' is not a PNG.\n", path);
		return img;
	}
	
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	
	png_init_io(png_ptr, file_pointer);
	png_set_sig_bytes(png_ptr, 8);
	png_read_info(png_ptr, info_ptr);
	
	int bit_depth, color_type;
	png_get_IHDR(png_ptr, info_ptr, &img.width, &img.height, &bit_depth, &color_type, NULL, NULL, NULL);
	if (bit_depth == 16) png_set_strip_16(png_ptr);
	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
	if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE) png_set_filler(png_ptr, 255, PNG_FILLER_AFTER);
	if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) png_set_gray_to_rgb(png_ptr);
	//png_set_gamma(png_ptr, display_exponent, gamma);
	png_read_update_info(png_ptr, info_ptr);

	unsigned int bytes_per_row = png_get_rowbytes(png_ptr, info_ptr);
	img.channels = png_get_channels(png_ptr, info_ptr);
	
	img.data = (unsigned char*) malloc(bytes_per_row * img.height);
	if (!img.data) {
		printf("Not enough memory for image buffer.\n");
		return img;
	}
	
	unsigned char* row_pointers[img.height];
	for (int i = 0; i < img.height; i++) row_pointers[i] = img.data + i * bytes_per_row;
	png_read_image(png_ptr, row_pointers);
	png_read_end(png_ptr, NULL);
	
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(file_pointer);

	return img;
}


GLuint init_texture(Image img) {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data);

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
	
	
	
	
	GLuint texture = init_texture(img);
	free(img.data);
	
	return texture;
}



void draw_quad(float x, float y, float width, float height, float u_min, float u_max, float v_min, float v_max) {
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
		
		float u = ((float) (c & 0b1111) + (1.0 - char_widthf * char_sizef_inv) * 0.5) * 0.0625;
		float v = (float) ((c >> 4) & 0b111) * 0.125;
		
		draw_quad(x, y, char_widthf * widthf_inv, char_sizef * heightf_inv, u, u + char_widthf * char_sizef_inv * 0.0625, v, v + 0.125);
		x += char_widthf * widthf_inv;
		
		text += 1;
	}
	
	glEnd();
}




int main() {
	
	//struct gpiod_chip* chip = gpiod_chip_open("/dev/gpiochip0");
	//struct gpiod_line* test_line = gpiod_chip_get_line(chip, 21);
	//gpiod_line_request_output(test_line, "test_line", 0);
	
	//gpiod_line_set_value(test_line, 1);
	
	
	
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		printf("GLFW failed to initialize. damn.\n");
		return 1;
	}
	
	GLFWwindow* window = glfwCreateWindow(display_width, display_height, "super cool window name", NULL, NULL);
	if (!window) {
		glfwTerminate();
		printf("Failed to create GLFW window. damn.\n");
		return 1;
	}
	
	glfwMakeContextCurrent(window);
	glEnable(GL_TEXTURE_2D);
	
	
	
	
	GLuint test_tex = load_generic_image("/home/clic/clic_firmware/assets/mc.png");
	if (!test_tex) return 1;
	
	GLuint font = load_font("/home/clic/clic_firmware/assets/consolas16.png");
	if (!font) return 1;

	
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	
	while (!glfwWindowShouldClose(window)) {
		
		
		
		glClear(GL_COLOR_BUFFER_BIT);
		
		//draw_image(test_tex, 0.2, 0.2, 0.6, 0.6);
		//draw_text("Hi test text for rendering (%#*!^&(%)^#@*(&'~`_--+=,<>./?:;[]{}\\|", font, 0.1, 0.1);
		draw_image(test_tex, 0.0, 0.0, 1.0, 1.0);
		
		
		glfwSwapBuffers(window);
		glfwPollEvents();
		
		if (glfwGetTime() > 10.0) break;
	}
	
	
	//gpiod_line_release(test_line);
	//gpiod_chip_close(chip);
	
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
