#ifndef CLIC_CAMERA_H
#define CLIC_CAMERA_H

#include <stdio.h>
#include <stdlib.h>
#include <linux/videodev2.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "common.h"


#define N_BUFFERS 3;

int camera_fd;
unsigned char* camera_buffers[N_BUFFERS];
int camera_buffer_size;

void setup_camera() {
	camera_fd = open("/dev/video0", O_RDWR, 0); // | O_NONBLOCK
	
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
	
	if (-1 == ioctl(camera_fd, VIDIOC_S_FMT, &format)) 
		clean_exit(-1, "No format. damn.");
	
	struct v4l2_requestbuffers request = {0};
	request.count = 2;
	request.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	request.memory = V4L2_MEMORY_MMAP;
	
	if (-1 == ioctl(camera_fd, VIDIOC_REQBUFS, &request)) 
		clean_exit(-1, "No buf. damn.");
	
	printf("\ncount: %d\n", request.count);
	
	
	struct v4l2_buffer buf = {0};
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = 0;
	
	if (-1 == ioctl(camera_fd, VIDIOC_QUERYBUF, &buf)) 
		clean_exit(-1, "No buf query. damn.");
	
	unsigned char* buffer = (unsigned char*) mmap(
			NULL, buf.length, PROT_READ | PROT_WRITE, 
			MAP_SHARED, camera_fd, buf.m.offset);
	
	if (-1 == ioctl(camera_fd, VIDIOC_QBUF, &buf)) 
		clean_exit(-1, "No queue. damn.");
	
	
	printf("%d\n", buffer[200]);
	
	
	
	
	
	
	
	
	
	
	
	
	
	clean_exit(0, "Camera test finished without errors.");
}


void cleanup_camera() {
	close(camera_fd);
}



#endif
