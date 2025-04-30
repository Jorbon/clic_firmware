#ifndef CLIC_GLFW_H
#define CLIC_GLFW_H

#include <GLFW/glfw3.h>

#include "common.h"
#include "graphics.h"


GLFWwindow* window;

void glfw_error_callback(int error, const char* description) {
	fprintf(stderr, "GLFW Error: %s\n", description);
}


int setup_glfw() {
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) {
		fprintf(stderr, "GLFW failed to initialize. damn.");
		return -1;
	}
	
	window = glfwCreateWindow(display_width, display_height, 
			"name", NULL, NULL);
	if (!window) {
		fprintf(stderr, "Failed to create GLFW window. damn.");
		return -1;
	}
	
	glfwMakeContextCurrent(window);
	glEnable(GL_TEXTURE_2D);
}

void cleanup_glfw() {
	glfwMakeContextCurrent(NULL);
	glfwDestroyWindow(window);
	glfwTerminate();
}


#endif
