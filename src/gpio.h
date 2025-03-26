#ifndef CLIC_GPIO_H
#define CLIC_GPIO_H

#include <string.h>
#include <gpiod.h>

#include "common.h"


#define GPIO_CLIC   13
#define GPIO_CENTER 12
#define GPIO_LEFT    1
#define GPIO_RIGHT  18
#define GPIO_UP     19
#define GPIO_DOWN    2
#define GPIO_LED     6

const unsigned int LED_PINS = {GPIO_LED};
const unsigned int BUTTON_PINS = {GPIO_CLIC, GPIO_CENTER, 
	GPIO_LEFT, GPIO_RIGHT, GPIO_UP, GPIO_DOWN};

enum Button {
	Clic   = 0,
	Center = 1,
	Left   = 2,
	Right  = 3,
	Up     = 4,
	Down   = 5,
}


struct gpiod_chip* chip;
struct gpiod_line_request_config button_config;
struct gpiod_line_bulk button_lines;
struct gpiod_line_request_config led_config;
struct gpiod_line_bulk led_lines;

int button_states[6] = {0};


int read_button_states() {
	gpiod_line_get_value_bulk(&button_lines, &button_states);
}

int set_led(int state) {
	return gpiod_line_set_value_bulk(&led_lines, &state);
}



void setup_gpio() {
	int error;
	
	struct gpiod_chip* chip = gpiod_chip_open("/dev/gpiochip0");
	if (!chip) clean_exit(-1, "No gpio chip open. damn.");
	
	error = gpiod_chip_get_lines(chip, BUTTON_PINS, 6, &button_lines);
	if (error) clean_exit(-1, "No lines. damn.");
	
	error = gpiod_chip_get_lines(chip, LED_PINS, 1, &led_lines);
	if (error) clean_exit(-1, "No lines. damn.");
	
	memset(&button_config, 0, sizeof(button_config));
	button_config.consumer = "clic";
	button_config.request_type = GPIOD_LINE_REQUEST_DIRECTION_INPUT;
	button_config.flags = 0;
	
	memset(&led_config, 0, sizeof(led_config));
	led_config.consumer = "clic";
	led_config.request_type = GPIOD_LINE_REQUEST_DIRECTION_OUTPUT;
	led_config.flags = 0;
	
	error = gpiod_line_request_bulk(&button_lines, &button_config, NULL);
	if (error) clean_exit(-1, "No get lines. damn.");
	
	int value = 0;
	error = gpiod_line_request_bulk(&led_lines, &led_config, &value);
	if (error) clean_exit(-1, "No get lines. damn.");
	
}

cleanup_gpio() {
	gpiod_line_release_bulk(&led_lines);
	gpiod_line_release_bulk(&button_lines);
	gpiod_chip_close(chip);
}


#endif