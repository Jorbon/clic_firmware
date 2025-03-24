#include <stdio.h>
#include <GLFW/glfw3.h>
#include <png.h>
#include <stdlib.h>
#include <string.h>
#include <gpiod.h>


const unsigned int GPIO_CLIC   = 13;
const unsigned int GPIO_CENTER = 12;
const unsigned int GPIO_LEFT   =  1;
const unsigned int GPIO_RIGHT  = 18;
const unsigned int GPIO_UP     = 19;
const unsigned int GPIO_DOWN   =  2;
const unsigned int GPIO_LED    =  6;



void error_callback(int error, const char* description) {
	fprintf(stderr, "GLFW Error: %s\n", description);
}


typedef struct {
	unsigned char* data;
	unsigned int width;
	unsigned int height;
	unsigned int channels;
} Image;


const unsigned int display_width = 320;
const unsigned int display_height = 240;
const float widthf = (float) display_width;
const float heightf = (float) display_height;
const float widthf_inv = 1.0 / widthf;
const float heightf_inv = 1.0 / heightf;
const float aratio = widthf * heightf_inv;
const float aratio_inv = heightf * widthf_inv;


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
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 
			0, GL_RGBA, GL_UNSIGNED_BYTE, img.data);

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




struct gpiod_chip* chip;
struct gpiod_line_request_config button_config;
struct gpiod_line_bulk button_lines;
struct gpiod_line_request_config led_config;
struct gpiod_line_bulk led_lines;

const unsigned int LED_PINS = {GPIO_LED};
const unsigned int BUTTON_PINS = {GPIO_CLIC, GPIO_CENTER, 
	GPIO_LEFT, GPIO_RIGHT, GPIO_UP, GPIO_DOWN);

GLFWwindow* window;


enum 



int get_button(enum Button) {
	gpiod_line_get_value_bulk(
}

int set_led(int state) {
	return gpiod_line_set_value_bulk(&led_lines, &state);
}


void clean_exit(int code, char* message) {
	glfwDestroyWindow(window);
	glfwTerminate();
	gpiod_line_release_bulk(&led_lines);
	gpiod_line_release_bulk(&button_lines);
	gpiod_chip_close(chip);
	
	printf("%s\n", message);
	exit(code);
}


int main() {
	int error;

	struct gpiod_chip* chip = gpiod_chip_open("/dev/gpiochip0");
	if (!chip) clean_exit(-1, "No gpio chip open. damn.");
	
	error = gpiod_chip_get_lines(chip, BUTTON_PINS, 6, &button_lines);
	if (error) clean_exit(-1, "No lines. damn.");
	
	error = gpiod_chip_get_lines(chip, LED_PINS, 1, &led_lines);
	if (error) clean_exit(-1, "No lines. damn.");
	
	memset(&button_config, 0, sizeof(button_config));
	button_config.consumer = "clic";
	button_config.request_type = GPIOD_LINE_REQUEST_DIRECTION_INPUT;
	button_config.flags = 0;
	
	memset(&led_config, 0, sizeof(led_config));
	led_config.consumer = "clic";
	led_config.request_type = GPIOD_LINE_REQUEST_DIRECTION_OUTPUT;
	led_config.flags = 0;
	
	error = gpiod_line_request_bulk(&button_lines, &button_config, NULL);
	if (error) clean_exit(-1, "No get lines. damn.");
	
	int value = 0;
	error = gpiod_line_request_bulk(&led_lines, &led_config, &value);
	if (error) clean_exit(-1, "No get lines. damn.");
	
	
	set_led(1);
	
	
	
	
	
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) clean_exit(-1, "GLFW failed to initialize. damn.");
	
	window = glfwCreateWindow(display_width, display_height, 
			"name", NULL, NULL);
	if (!window) clean_exit(-1, "Failed to create GLFW window. damn.");
	
	glfwMakeContextCurrent(window);
	glEnable(GL_TEXTURE_2D);
	
	
	
	
	GLuint test_tex = load_generic_image(
			"/home/clic/clic_firmware/assets/mc.png");
	if (!test_tex) clean_exit(-1, "No load test tex. damn.");
	
	GLuint font = load_font(
			"/home/clic/clic_firmware/assets/consolas16.png");
	if (!font) return 1;

	
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	
	while (!glfwWindowShouldClose(window)) {
		
		
		
		glClear(GL_COLOR_BUFFER_BIT);
		
		draw_image(test_tex, 0.0, 0.0, 1.0, 1.0);
		draw_text("Hi test text for rendering (%#*!^&(%)^#@*(&'~`_--+=,<>./?:;[]{}\\|", 
				font, 0.1, 0.1);
		
		
		glfwSwapBuffers(window);
		glfwPollEvents();
		
		if (glfwGetTime() > 10.0) break;
	}
	
	
	clean_exit(0, "Clic closed without errors.");
	return 0;
}
