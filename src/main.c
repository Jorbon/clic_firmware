#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>

#include "common.h"
#include "image.h"
#include "graphics.h"
#include "glfw.h"
#include "gpio.h"
#include "camera.h"






GLuint test_tex;
GLuint font;


void clean_exit(int code, char* message) {
	glDeleteTextures(1, &test_tex);
	glDeleteTextures(1, &font);
	
	cleanup_camera();
	cleanup_glfw();
	cleanup_gpio();
	
	printf("\n%s\n\n", message);
	exit(code);
}



int live_preview = 0;


int main() {
	setup_gpio();
	setup_glfw();
	setup_camera(display_width, display_height);
	
	
	test_tex = load_generic_image("/home/clic/clic_firmware/assets/mc.png");
	if (!test_tex) clean_exit(-1, "No load test tex. damn.");
	
	font = load_font("/home/clic/clic_firmware/assets/consolas16.png");
	if (!font) clean_exit(-1, "No load font. damn.");
	
	
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	
	while (!glfwWindowShouldClose(window)) {
		
		read_button_states();
		
		if (button_states[Left]) {
			live_preview = 0;
			glDeleteTextures(1, &test_tex);
			test_tex = load_generic_image(
				"/home/clic/clic_firmware/assets/mc.png");
		}
		
		if (button_states[Right]) {
			live_preview = 0;
			glDeleteTextures(1, &test_tex);
			test_tex = load_generic_jpeg(
				"/home/clic/clic_firmware/test.jpg");
		}
		
		if (button_states[Clic]) {
			live_preview = 1;
			glDeleteTextures(1, &test_tex);
		}
		
		set_led(live_preview);
		
		if (button_states[Center]) break;
		
		
		if (live_preview) {
			Image img = get_camera_image();
			if (img.data) {
				glDeleteTextures(1, &test_tex);
				test_tex = init_texture(img);
				done_with_camera_image();
			}
		}
		
		
		glClear(GL_COLOR_BUFFER_BIT);
		
		draw_image(test_tex, 0.0, 0.0, 1.0, 1.0);
		
		//draw_text("applesauce|\\/*^$@?_<>", font, 0.1, 16 * heightf_inv);
		//draw_text("applesauce|\\/*^$@?_<>", font, 0.1, 32 * heightf_inv);
		//draw_text("applesauce|\\/*^$@?_<>", font, 0.1, 48 * heightf_inv);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
		

		sleep(0.015);
	}
	
	
	clean_exit(0, "Clic closed without errors.");
	return 0;
}
