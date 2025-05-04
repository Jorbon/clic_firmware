#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>

#include "common.h"
#include "image.h"
#include "graphics.h"
#include "glfw.h"
#include "gpio.h"
#include "camera.h"
#include "processing.h"
#include "files.h"



int camera_working = 0;


GLuint font;
GLuint error_screen;
GLuint live_image;




void show_error_screen(const char* message) {
	draw_image(error_screen, 0.0, 0.0, 1.0, 1.0);
	draw_text(message, font, 0.4, 0.4);
	glfwSwapBuffers(window);
	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		sleep(0.1);
	}
	
	exit(-1);
}


void draw_live_image() {
	if (camera_working) {
		Image img = {0};
		if (-1 == get_camera_image(&img)) {
			camera_working = 0;
			return;
		}
		
		if (img.data) {
			glDeleteTextures(1, &live_image);
			
			Image rgb_img = {0};
			
			rgbshow_from_raw10(&rgb_img, img);
			live_image = init_texture(rgb_img);
			free(rgb_img.data);
			
			if (-1 == done_with_camera_image()) 
				show_error_screen("camera lost");
		}
		
		draw_image(live_image, 0.0, 0.0, 1.0, 1.0);
		
	} else {
		draw_image(error_screen, 0.0, 0.0, 1.0, 1.0);
		draw_text("no camera feed", font, 0.4, 0.4);
	}
}



int main() {
	if (-1 == setup_glfw()) {
		printf("No glfw setup for error screen.");
		exit(-1);
	}
	
	
	font = load_font("/home/clic/clic_firmware/assets/consolas16.png");
	if (!font) fprintf(stderr, "Asset not loaded: consolas16.png");
	
	error_screen = load_generic_image(
		"/home/clic/clic_firmware/assets/error.png");
	if (!error_screen) fprintf(stderr, "Asset not loaded: error.png");
	
	
	if (-1 == setup_gpio()) 
		show_error_screen("no gpio");
	
	
	//if (-1 == setup_camera(CAMERA_WIDTH, CAMERA_HEIGHT)) 
	if (-1 == setup_camera(640, 480)) 
		camera_working = 0;
	else camera_working = 1;
	
	
	if (-1 == set_reset(1) || -1 == set_backlight(1)) 
		show_error_screen("gpio lost");
	
	
	if (-1 == scan_capture_dir()) 
		printf("Couldn't open capture dir\n");
	register_capture();
	
	set_gain(0.5);
	
	
	while (!glfwWindowShouldClose(window)) {
		
		if (-1 == read_button_states()) 
				show_error_screen("gpio lost");
		
		
		if (-1 == set_led(1)) 
				show_error_screen("gpio lost");
		
		
		
		//glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT);
		
		
		
		
		
		draw_live_image();
		
		
		
		
		//draw_text("applesauce|\\/*^$@?_<>", font, 0.1, 16 * heightf_inv);
		//draw_text("applesauce|\\/*^$@?_<>", font, 0.1, 32 * heightf_inv);
		//draw_text("applesauce|\\/*^$@?_<>", font, 0.1, 48 * heightf_inv);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
		
		
		sleep(0.015);
	}
	
	
	
	glDeleteTextures(1, &live_image);
	glDeleteTextures(1, &font);
	
	cleanup_camera();
	cleanup_gpio();
	cleanup_glfw();
	
	printf("Clic closed without errors.");
	return 0;
}
