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


void camera_command(int request, void* argument, char* error_message) {
	if (-1 == xioctl(camera_fd, request, argument)) {
		printf("Camera error: %s - %d, %s\n", error_message, 
				errno, strerror(errno));
		clean_exit(-1, "Camera error. damn.");
	}
}

void cleanup_camera();



void setup_camera(int width, int height) {
	if (camera_init) cleanup_camera();
	
	camera_fd = open("/dev/video0", O_RDWR | O_NONBLOCK, 0);
	
	
	struct v4l2_capability cap;
	camera_command(VIDIOC_QUERYCAP, &cap, "Query capabilities");
	
	if (!(cap.capabilities & (V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING)))
		clean_exit(-1, "No cap. damn.");
	
	struct v4l2_format format = {0};
	format.type = buf_type;
	format.fmt.pix.width = width;
	format.fmt.pix.height = height;
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_SRGGB10P;
	format.fmt.pix.field = V4L2_FIELD_NONE;
	
	camera_command(VIDIOC_S_FMT, &format, "Set format");
	
	camera_output_width = format.fmt.pix.width;
	camera_output_height = format.fmt.pix.height;
	
	
	struct v4l2_requestbuffers request = {0};
	request.count = N_BUFFERS;
	request.type = buf_type;
	request.memory = V4L2_MEMORY_MMAP;
	
	camera_command(VIDIOC_REQBUFS, &request, "Request buffers");
	if (request.count != N_BUFFERS) clean_exit(-1, "Not enough buffers.");
	
	for (int i = 0; i < N_BUFFERS; i++) {
		BUF_INIT(i);
		camera_command(VIDIOC_QUERYBUF, &buf, "Query buffer");
		
		camera_buffer_size = buf.length;
		camera_buffers[i] = (unsigned char*) mmap(
				NULL, buf.length, PROT_READ | PROT_WRITE,
				MAP_SHARED, camera_fd, buf.m.offset);
		
		camera_command(VIDIOC_QBUF, &buf, "Queue buffer");
	}
	
	camera_command(VIDIOC_STREAMON, &buf_type, "Stream on");

	camera_init = 1;
	
	printf("Camera setup for %d x %d '", 
			camera_output_width, camera_output_height);
	fwrite(&format.fmt.pix.pixelformat, 1, 4, stdout);
	printf("'\n");
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

Image get_camera_image() {
	Image img = {0};

	if (!camera_init) return img;
	if (currently_using) return img;
	
	BUF_INIT(0);
	if (-1 == ioctl(camera_fd, VIDIOC_DQBUF, &buf)) {
		return img;
	} else {
		current_buffer_index = buf.index;
		currently_using = 1;
		img.data = camera_buffers[current_buffer_index];
		img.width = camera_output_width * 5 / 4;
		img.height = camera_output_height;
		img.channels = 1; // cursed ik, it's raw10
		
		return img;
	}
}

void done_with_camera_image() {
	if (!camera_init) return;
	if (!currently_using) return;
	
	BUF_INIT(current_buffer_index);
	camera_command(VIDIOC_QBUF, &buf, "Re-queue buffer");
	//printf("return buffer\n");
	currently_using = 0;
}



#endif
