#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <linux/videodev2.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "common.h"
#include "image.h"
#include "graphics.h"
#include "glfw.h"
#include "gpio.h"







GLuint test_tex;
GLuint font;


void clean_exit(int code, char* message) {
	glDeleteTextures(1, &test_tex);
	glDeleteTextures(1, &font);
	
	cleanup_glfw();
	cleanup_gpio();
	
	printf("\n%s\n\n", message);
	exit(code);
}




int main() {
	setup_gpio();
	setup_glfw();
	setup_camera();
	
	int camera_fd = open("/dev/video0", O_RDWR, 0); // | O_NONBLOCK
	
	struct v4l2_capability cap;
	if (-1 == ioctl(camera_fd, VIDIOC_QUERYCAP, &cap)) 
		clean_exit(-1, "No query cap. damn.");
	
	if (!(cap.capabilities & (V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING)))
		clean_exit(-1, "No cap. damn.");
	
	struct v4l2_format format = {0};
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format.fmt.pix.width = 320;
	format.fmt.pix.height = 240;
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
	format.fmt.pix.field = V4L2_FIELD_NONE;
	
	if (-1 == ioctl(camera_fd, VIDIOC_S_FMT, &format)) clean_exit(-1, "no fmt");
	
	
	
	
	clean_exit(0, "test process done");
	
	
	
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
