#ifndef CLIC_GLFW_H
#define CLIC_GLFW_H

#include <GLFW/glfw3.h>

#include "common.h"


GLFWwindow* window;

void glfw_error_callback(int error, const char* description) {
	fprintf(stderr, "GLFW Error: %s\n", description);
}


setup_glfw() {
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) clean_exit(-1, "GLFW failed to initialize. damn.");
	
	window = glfwCreateWindow(display_width, display_height, 
			"name", NULL, NULL);
	if (!window) clean_exit(-1, "Failed to create GLFW window. damn.");
	
	glfwMakeContextCurrent(window);
	glEnable(GL_TEXTURE_2D);
}

cleanup_glfw() {
	glfwDestroyWindow(window);
	glfwTerminate();
}


#endif