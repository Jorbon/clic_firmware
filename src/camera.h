#ifndef CLIC_CAMERA_H
#define CLIC_CAMERA_H

#include <linux/videodev2.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "common.h"


int camera_fd;


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
	
	
	
	
	
	clean_exit(0, "Camera test finished without errors.");
}


void cleanup_camera() {
	close(camera_fd);
}



#endif