#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>

#include "common.h"
#include "image.h"
#include "graphics.h"
#include "glfw.h"
#include "gpio.h"





int main() {
	setup_gpio();
	setup_glfw();
	
	
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
		draw_text("applesauce|\\/*^$@?_<>", font, 0.1, 0.1);
		draw_text("applesauce|\\/*^$@?_<>", font, 0.1, 0.2);
		draw_text("applesauce|\\/*^$@?_<>", font, 0.1, 0.3);
		
		
		glfwSwapBuffers(window);
		glfwPollEvents();
		
		if (glfwGetTime() > 10.0) break;
	}
	
	
	clean_exit(0, "Clic closed without errors.");
	return 0;
}
