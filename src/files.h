#ifndef CLIC_FILES_H
#define CLIC_FILES_H

#include <time.h>
#include <dirent.h>

#include "common.h"


#define MAX_CAPTURES 256


DIR* capture_dir;
char capture_path[40] = "/cap/";
char capture_paths[MAX_CAPTURES][32] = {0};
int capture_count = 0;


void get_capture_path(int index) {
	for (int i = 0; i < 32; i++) {
		char c = capture_paths[index][i];
		capture_path[i + 5] = c;
		if (c == 0) break;
	}
}

int scan_capture_dir() {
	capture_dir = opendir("/cap");
	if (!capture_dir) return -1;
	
	capture_count = 0;
	
	while (1) {
		struct dirent* de = readdir(capture_dir);
		if (!de) break;
		
		char* name = de->d_name;
		
		if (name[0] != 'C' ||
		    name[1] != 'L' ||
		    name[2] != 'I' ||
		    name[3] != 'C') 
			continue;
		
		for (int i = 0; i < 32; i++) {
			char c = name[i];
			capture_paths[capture_count][i] = c;
			if (c == 0) break;
		}
		
		capture_count += 1;

		if (capture_count == MAX_CAPTURES) break;
	}
	
	return 0;
}


int register_capture_path() {
	if (capture_count == MAX_CAPTURES) return -1;
	
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf(capture_paths[capture_count], 
		"CLIC_%04d-%02d-%02d_%02d-%02d-%02d\n", 
		tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
		tm.tm_hour, tm.tm_min, tm.tm_sec);
	
	get_capture_path(capture_count);
	capture_count += 1;
}





#endif
