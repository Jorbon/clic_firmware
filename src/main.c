#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>

#include "common.h"
#include "image.h"
#include "graphics.h"
#include "glfw.h"
#include "gpio.h"
#include "camera.h"




GLuint font;
GLuint error_screen;
GLuint live_image;




void show_error_screen(const char* message) {
	draw_image(error_screen, 0.0, 0.0, 1.0, 1.0);
	draw_text(message, font, 0.3, 0.4);
	glfwSwapBuffers(window);
	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		sleep(0.1);
	}
	
	exit(-1);
}



int main() {
	if (-1 == setup_glfw()) {
		printf("Couldn't create a window to show a funny error screen.");
		exit(-1);
	}
	
	
	font = load_font("/home/clic/clic_firmware/assets/consolas16.png");
	if (!font) fprintf(stderr, "Asset not loaded: consolas16.png");
	
	error_screen = load_generic_image(
		"/home/clic/clic_firmware/assets/error.png");
	if (!error_screen) fprintf(stderr, "Asset not loaded: error.png");
	
	
	
	if (-1 == setup_camera(CAMERA_WIDTH, CAMERA_HEIGHT)) 
			show_error_screen("no camera");
	
	if (-1 == setup_gpio()) 
			show_error_screen("no gpio");
	
			
	if (-1 == set_reset(1) || -1 == set_backlight(1)) 
			show_error_screen("gpio lost");
	
	
	
	while (!glfwWindowShouldClose(window)) {
		
		if (-1 == read_button_states()) 
				show_error_screen("gpio lost");
		
		
		if (-1 == set_led(1)) 
				show_error_screen("gpio lost");
		
		
		Image img = {0};
		if (-1 == get_camera_image(&img)) 
				show_error_screen("camera lost");
		if (img.data) {
			glDeleteTextures(1, &test_tex);
			test_tex = init_texture(img);
			if (-1 == done_with_camera_image()) 
					show_error_screen("camera lost");
		}
		
		
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		draw_image(test_tex, 0.0, 0.0, 1.0, 1.0);
		
		//draw_text("applesauce|\\/*^$@?_<>", font, 0.1, 16 * heightf_inv);
		//draw_text("applesauce|\\/*^$@?_<>", font, 0.1, 32 * heightf_inv);
		//draw_text("applesauce|\\/*^$@?_<>", font, 0.1, 48 * heightf_inv);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
		

		sleep(0.015);
	}
	
	
	
	glDeleteTextures(1, &test_tex);
	glDeleteTextures(1, &font);
	
	cleanup_camera();
	cleanup_gpio();
	cleanup_glfw();
	
	printf("Clic closed without errors.");
	return 0;
}
