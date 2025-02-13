#include <stdio.h>
#include <GLFW/glfw3.h>
#include <png.h>
#include <stdlib.h>




void error_callback(int error, const char* description) {
	fprintf(stderr, "GLFW Error: %s\n", description);
}


typedef struct {
	unsigned char* data;
	unsigned int width;
	unsigned int height;
	unsigned int channels;
} Image;

typedef struct {
	GLuint font_texture;
} Font;


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


GLuint load_generic_image(const char* path) {
	
	Image img = load_png(path);
	
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data);
	
	free(img.data);
	
	return texture;
}


GLuint load_font(const char* path) {
	
	Image img = load_png(path);
	
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data);
	
	
	free(img.data);
	
	return texture;
}





int main() {
	
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		printf("GLFW failed to initialize. damn.\n");
		return 1;
	}
	
	int display_width = 1920;
	int display_height = 1080;
	
	GLFWwindow* window = glfwCreateWindow(display_width, display_height, "super cool window name", NULL, NULL);
	if (!window) {
		glfwTerminate();
		printf("Failed to create GLFW window. damn.\n");
		return 1;
	}
	
	printf("window exists!\n");
	
	glfwMakeContextCurrent(window);
	glEnable(GL_TEXTURE_2D);
	
	
	
	
	GLuint test_tex = load_font("/root/clic_firmware/assets/consolas16.png");
	if (!test_tex) return 1;
	
	
	
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	
	while (!glfwWindowShouldClose(window)) {
		
		
		
		glClear(GL_COLOR_BUFFER_BIT);
		
		glBindTexture(GL_TEXTURE_2D, test_tex);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, -1.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f, 0.0f);
		glEnd();
		
		glfwSwapBuffers(window);
		glfwPollEvents();
		
		if (glfwGetTime() > 10.0) break;
	}
	
	
	
	glfwDestroyWindow(window); printf("destroy did not segfault\n");
	glfwTerminate(); printf("terminate did not segfault\n");
	return 0;
}
