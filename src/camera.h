#ifndef CLIC_CAMERA_H
#define CLIC_CAMERA_H

#include <stdio.h>
#include <stdlib.h>
#include <linux/videodev2.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>

#include "common.h"


int xioctl(int fd, int request, void* argument) {
	int result;
	do result = ioctl(fd, request, argument);
	while (-1 == result && errno == EINTR);
	
	return result;
}



#define BUF_INIT(i) struct v4l2_buffer buf = {0}; buf.type = buf_type; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;

#define N_BUFFERS 3

int camera_fd;
unsigned char* camera_buffers[N_BUFFERS];
int camera_buffer_size;

int camera_init = 0;
int camera_output_width;
int camera_output_height;

const enum v4l2_buf_type buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;


// int camera_command(int request, void* argument, char* error_message) {
// 	if (-1 == xioctl(camera_fd, request, argument)) {
// 		printf("Camera error: %s - %d, %s\n", c,
// 			errno, strerror(errno));
// 		return -1;
// 	}
// 	return 0;
// }

#define CAMERA_COMMAND(a, b, c) if (-1 == xioctl(camera_fd, a, b)) { \
	fprintf(stderr, "Camera error: %s - %d, %s\n", \
		c, errno, strerror(errno)); \
	return -1; \
}



void cleanup_camera();


int setup_camera(int width, int height) {
	if (camera_init) cleanup_camera();
	
	camera_fd = open("/dev/video0", O_RDWR | O_NONBLOCK, 0);
	
	
	struct v4l2_capability cap;
	CAMERA_COMMAND(VIDIOC_QUERYCAP, &cap, "Query capabilities");
	
	if (!(cap.capabilities & (V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING))) {
		fprintf(stderr, "Camera got wrong caps");
		return -1;
	}
	
	struct v4l2_format format = {0};
	format.type = buf_type;
	format.fmt.pix.width = width;
	format.fmt.pix.height = height;
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_SRGGB10P;
	format.fmt.pix.field = V4L2_FIELD_NONE;
	
	CAMERA_COMMAND(VIDIOC_S_FMT, &format, "Set format");
	
	camera_output_width = format.fmt.pix.width;
	camera_output_height = format.fmt.pix.height;
	
	
	struct v4l2_requestbuffers request = {0};
	request.count = N_BUFFERS;
	request.type = buf_type;
	request.memory = V4L2_MEMORY_MMAP;
	
	CAMERA_COMMAND(VIDIOC_REQBUFS, &request, "Request buffers");
	if (request.count != N_BUFFERS) {
		fprintf(stderr, "Not enough buffers: Requested %d, driver needs %d\n", 
				N_BUFFERS, request.count);
		return -1;
	}
	
	for (int i = 0; i < N_BUFFERS; i++) {
		BUF_INIT(i);
		CAMERA_COMMAND(VIDIOC_QUERYBUF, &buf, "Query buffer");
		
		camera_buffer_size = buf.length;
		camera_buffers[i] = (unsigned char*) mmap(
				NULL, buf.length, PROT_READ | PROT_WRITE,
				MAP_SHARED, camera_fd, buf.m.offset);
		
		CAMERA_COMMAND(VIDIOC_QBUF, &buf, "Queue buffer");
	}
	
	CAMERA_COMMAND(VIDIOC_STREAMON, &buf_type, "Stream on");

	camera_init = 1;
	
	// printf("Camera setup for %d x %d '", 
	// 		camera_output_width, camera_output_height);
	// fwrite(&format.fmt.pix.pixelformat, 1, 4, stdout);
	// printf("'\n");
	
	return 0;
}


void cleanup_camera() {
	if (!camera_init) return;
	for (int i = 0; i < N_BUFFERS; i++) 
		munmap(camera_buffers[i], camera_buffer_size);
	close(camera_fd);
	camera_init = 0;
}


int current_buffer_index = 0;
int currently_using = 0;

int get_camera_image(Image* img) {
	if (!camera_init) return 0;
	if (currently_using) return 0;
	
	BUF_INIT(0);
	if (-1 == ioctl(camera_fd, VIDIOC_DQBUF, &buf)) {
		return 0;
	} else {
		current_buffer_index = buf.index;
		currently_using = 1;
		img.data = camera_buffers[current_buffer_index];
		img.width = camera_output_width * 5 / 4;
		img.height = camera_output_height;
		img.channels = 1; // cursed ik, it's raw10 eww
		return 0;
	}
}

int done_with_camera_image() {
	if (!camera_init) return 0;
	if (!currently_using) return 0;
	
	BUF_INIT(current_buffer_index);
	CAMERA_COMMAND(VIDIOC_QBUF, &buf, "Re-queue buffer");
	//printf("return buffer\n");
	currently_using = 0;
	return 0;
}



#endif
