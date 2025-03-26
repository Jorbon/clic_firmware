#ifndef CLIC_COMMON_H
#define CLIC_COMMON_H

#include <stdio.h>
#include <stdlib.h>

#include "gpio.h"
#include "glfw.h"


void clean_exit(int code, char* message) {
	cleanup_glfw();
	cleanup_gpio();
	
	printf("%s\n", message);
	exit(code);
}


#endif