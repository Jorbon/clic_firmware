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




int main() {
	setup_gpio();
	setup_glfw();
	setup_camera();
	
	
	
	test_tex = load_generic_image("/home/clic/clic_firmware/assets/mc.png");
	if (!test_tex) clean_exit(-1, "No load test tex. damn.");
	
	font = load_font("/home/clic/clic_firmware/assets/consolas16.png");
	if (!font) clean_exit(-1, "No load font. damn.");
	
	
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	
	while (!glfwWindowShouldClose(window)) {
		
		glClear(GL_COLOR_BUFFER_BIT);
		
		draw_image(test_tex, 0.0, 0.0, 1.0, 1.0);
		draw_text("applesauce|\\/*^$@?_<>", font, 0.1, 16 * heightf_inv);
		draw_text("applesauce|\\/*^$@?_<>", font, 0.1, 32 * heightf_inv);
		draw_text("applesauce|\\/*^$@?_<>", font, 0.1, 48 * heightf_inv);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
		
		
		read_button_states();
		
		if (button_states[Left]) {
			glDeleteTextures(1, &test_tex);
			test_tex = load_generic_image(
				"/home/clic/clic_firmware/assets/mc.png");
		}

		if (button_states[Right]) {
			glDeleteTextures(1, &test_tex);
			test_tex = load_generic_jpeg(
				"/home/clic/clic_firmware/test.jpg");
		}
		if (button_states[Clic]) system("libcamera-jpeg -o /home/clic/clic_firmware/test.jpg --width 320 --height 240");

		if (button_states[Center]) break;
	}
	
	
	clean_exit(0, "Clic closed without errors.");
	return 0;
}
