#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>

#include "common.h"
#include "image.h"
#include "graphics.h"
#include "glfw.h"
#include "gpio.h"
#include "camera.h"
#include "processing.h"
#include "files.h"



int camera_working = 0;


GLuint font0;
GLuint error_screen;
GLuint live_image;

#define MAX_GAIN 20
unsigned char gain = MAX_GAIN / 2;
#define MAX_CAPTURE_TIMER 20
unsigned char capture_timer = 0;
#define MAX_REVIEW_TIME 20
unsigned char review_time = 3;


int capture_mode = 1;
int capturing = 0;
int capture_index = 0;
int menu_pos = 0;
int delete_confirm = 0;

float capture_time = 0.0;
float review_end_time = 0.0;

int previous_button_states[6] = {0};




void show_error_screen(const char* message) {
	draw_image(error_screen, 0.0, 0.0, 1.0, 1.0);
	draw_text(message, font0, 0.4, 0.4);
	glfwSwapBuffers(window);
	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		sleep(0.1);
	}
	
	exit(-1);
}


void draw_live_image() {
	if (glfwGetTime() < review_end_time) {
		draw_image(live_image, 0.0, 0.0, 1.0, 1.0);
		return;
	}
	
	if (!camera_working) {
		draw_image(error_screen, 0.0, 0.0, 1.0, 1.0);
		draw_text("no camera feed", font0, 0.4, 0.4);
		return;
	}
	
	
	Image img = {0};
	if (-1 == get_camera_image(&img)) {
		camera_working = 0;
		return;
	}
	
	if (img.data) {
		glDeleteTextures(1, &live_image);
		Image rgb_img = {0};
		
		if (capturing && glfwGetTime() > capture_time) {
			rgbfull_from_raw10(&rgb_img, img);
			live_image = init_texture(rgb_img);
			
			register_capture_path();
			if (-1 == save_jpeg(capture_path, rgb_img)) {
				printf("Error saving jpeg to %s\n", 
					capture_path);
			}
			
			review_end_time = glfwGetTime() + review_time;
			capturing = 0;
		} else {
			rgbshow_from_raw10(&rgb_img, img);
			live_image = init_texture(rgb_img);
		}
		
		free(rgb_img.data);	
		if (-1 == done_with_camera_image()) 
			show_error_screen("camera lost");
	}
	
	draw_image(live_image, 0.0, 0.0, 1.0, 1.0);
	
}



int main() {
	if (-1 == setup_glfw()) {
		printf("No glfw setup for error screen.");
		exit(-1);
	}
	
	
	font0 = load_font("/home/clic/clic_firmware/assets/consolas16.png");
	if (!font0) fprintf(stderr, "Asset not loaded: consolas16.png");
	
	error_screen = load_generic_image(
		"/home/clic/clic_firmware/assets/error.png");
	if (!error_screen) fprintf(stderr, "Asset not loaded: error.png");
	
	
	if (-1 == setup_gpio()) 
		show_error_screen("no gpio");
	
	
	//if (-1 == setup_camera(CAMERA_WIDTH, CAMERA_HEIGHT)) 
	if (-1 == setup_camera(640, 480)) 
		camera_working = 0;
	else camera_working = 1;
	
	
	if (-1 == set_reset(1) || -1 == set_backlight(1)) 
		show_error_screen("gpio lost");
	
	
	if (-1 == scan_capture_dir()) 
		printf("Couldn't open capture dir\n");
	
	
	set_gain((float) gain / (float) MAX_GAIN);
	
	print_font = font0;
	
	
	
	while (!glfwWindowShouldClose(window)) {
		
		if (-1 == read_button_states()) 
			show_error_screen("gpio lost");
		
		
		if (button_states[Clic] && !previous_button_states[Clic]) {
			if (capture_mode && !menu_pos && camera_working 
				&& glfwGetTime() > review_end_time) {
				capturing = 1;
				capture_time = 
					glfwGetTime() + capture_timer;
			} else {
				capture_mode = 1;
				menu_pos = 0;
				delete_confirm = 0;
			}
		}
		
		if (button_states[Left] && !previous_button_states[Left]) {
			if (capture_mode) {
				switch (menu_pos) {
				case 0:
					if (capture_count == 0) break;
					capture_mode = 0;
					capturing = 0;
					capture_index = 0;
					glDeleteTextures(1, &live_image);
					get_capture_path(capture_index);
					live_image = load_generic_jpeg(
						capture_path);
					break;
				case 1:
					if (gain <= 1) break;
					gain -= 1;
					set_gain((float) gain / MAX_GAIN);
					break;
				case 2:
					if (capture_timer <= 0) break;
					capture_timer -= 1;
					break;
				case 3:
					if (review_time <= 0) break;
					review_time -= 1;
					break;
				}
			} else if (!delete_confirm) {
				capture_index += 1;
				if (capture_index >= capture_count) {
					capture_mode = 1;
				} else {
					glDeleteTextures(1, &live_image);
					get_capture_path(capture_index);
					live_image = load_generic_jpeg(
						capture_path);
				}
				menu_pos = 0;
			}
		}
		
		if (button_states[Right] && !previous_button_states[Right]) {
			if (capture_mode) {
				switch(menu_pos) {
				case 0:
					if (capture_count == 0) break;
					capture_mode = 0;
					capturing = 0;
					capture_index = capture_count - 1;
					glDeleteTextures(1, &live_image);
					get_capture_path(capture_index);
					live_image = load_generic_jpeg(
						capture_path);
					break;
				case 1:
					if (gain >= MAX_GAIN) break;
					gain += 1;
					set_gain((float) gain / MAX_GAIN);
					break;
				case 2:
					if (capture_timer >= MAX_CAPTURE_TIMER)
						break;
					capture_timer += 1;
					break;
				case 3:
					if (review_time >= MAX_REVIEW_TIME)
						break;
					review_time += 1;
					break;
				}
			} else if (!delete_confirm) {
				capture_index -= 1;
				if (capture_index < 0) {
					capture_mode = 1;
				} else {
					glDeleteTextures(1, &live_image);
					get_capture_path(capture_index);
					live_image = load_generic_jpeg(
						capture_path);
				}
				menu_pos = 0;
			}
		}
		
		if (button_states[Up] && !previous_button_states[Up]) {
			if (capture_mode) {
				menu_pos -= 1;
				if (menu_pos < 0) menu_pos = 3;
			} else if (!delete_confirm) {
				menu_pos -= 1;
				if (menu_pos < 0) menu_pos = 1;
			}
		}
		
		if (button_states[Down] && !previous_button_states[Down]) {
			if (capture_mode) {
				menu_pos += 1;
				if (menu_pos > 3) menu_pos = 0;
			} else if (!delete_confirm) {
				menu_pos += 1;
				if (menu_pos > 1) menu_pos = 0;
			}
		}
		
		if (button_states[Center] && !previous_button_states[Center]) {
			if (!capture_mode && menu_pos == 1) {
				if (delete_confirm) {
					//todo
					
					delete_confirm = false;
					menu_pos = 0;
					if (capture_index > 0) 
						capture_index -= 1;
				} else {
					delete_confirm = true;
				}
			}
		}
		
		
		
		
		for (int i = 0; i < 6; i++) 
			previous_button_states[i] = button_states[i];
		
		if (-1 == set_led(capture_mode)) 
			show_error_screen("gpio lost");
		
		
		
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		
		if (capture_mode) {
			draw_live_image();
			if (menu_pos) {
				print(6, 5.0, " Gain             %%   ");
				print(6, 6.5, " Capture Timer    %%s  ");
				print(6, 8.0, " Review Time      %%s  ");
				print(22, 3.5 + 1.5*menu_pos, ">");
				print(27, 3.5 + 1.5*menu_pos, "<");
				
				char buf[3] = "  ";
				buf[1] = '0' + gain % 10;
				if (gain >= 10) buf[0] = '0' + gain / 10;
				print(24, 5.0, buf);
				
				buf[1] = '0' + capture_timer % 10;
				if (capture_timer >= 10) 
					buf[0] = '0' + capture_timer / 10;
				else buf[0] = ' ';
				print(24, 6.5, buf);
				
				buf[1] = '0' + review_time % 10;
				if (review_time >= 10) 
					buf[0] = '0' + review_time / 10;
				else buf[0] = ' ';
				print(24, 8.0, buf);
			}
		} else {
			draw_image(live_image, 0.0, 0.0, 1.0, 1.0);
			print(0, 12.0, capture_names[capture_index]);
			if (menu_pos) {
				print(6, 7.0, "      > Delete <      ");
			}

			if (delete_confirm) {
				print(6, 7.0, "                      ");
				print(6, 8.0, "  > Confirm Delete <  ");
				print(6, 9.0, "                      ");
			}
		}
		
		
		
		
		//draw_text("Exposure:", font0, 0.1, 0.0);
		
			
		glfwSwapBuffers(window);
		glfwPollEvents();
		
		
		sleep(0.015);
	}
	
	
	
	glDeleteTextures(1, &live_image);
	glDeleteTextures(1, &font0);
	
	cleanup_camera();
	cleanup_gpio();
	cleanup_glfw();
	
	printf("Clic closed without errors.");
	return 0;
}
