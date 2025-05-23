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
#define GPIO_DISPLAY_RESET     20
#define GPIO_DISPLAY_BACKLIGHT 21


const unsigned int BUTTON_PINS[6] = {GPIO_CLIC, GPIO_CENTER, 
	GPIO_LEFT, GPIO_RIGHT, GPIO_UP, GPIO_DOWN};
const unsigned int LED_PINS[1] = {GPIO_LED};
const unsigned int RESET_PINS[1] = {GPIO_DISPLAY_RESET};
const unsigned int BACKLIGHT_PINS[1] = {GPIO_DISPLAY_BACKLIGHT};


enum Button {
	Clic   = 0,
	Center = 1,
	Left   = 2,
	Right  = 3,
	Up     = 4,
	Down   = 5,
};


struct gpiod_chip* chip;
struct gpiod_line_request_config input_config;
struct gpiod_line_request_config output_config;
struct gpiod_line_bulk button_lines;
struct gpiod_line_bulk led_lines;
struct gpiod_line_bulk reset_lines;
struct gpiod_line_bulk backlight_lines;

int button_states[6] = {0};


int read_button_states() {
	if (gpiod_line_get_value_bulk(&button_lines, button_states)) return -1;
	button_states[Down] = !button_states[Down];
	return 0;
}

int set_led(int state) {
	return gpiod_line_set_value_bulk(&led_lines, &state);
}

int set_reset(int state) {
	return gpiod_line_set_value_bulk(&reset_lines, &state);
}

int set_backlight(int state) {
	return gpiod_line_set_value_bulk(&backlight_lines, &state);
}




int setup_gpio() {
	int error;
	
	struct gpiod_chip* chip = gpiod_chip_open("/dev/gpiochip0");
	if (!chip) {
		fprintf(stderr, "No gpio chip open. damn.");
		return -1;
	}
	
	error = gpiod_chip_get_lines(chip, BUTTON_PINS, 6, &button_lines);
	if (error) { fprintf(stderr, "No lines. damn."); return -1; }
	error = gpiod_chip_get_lines(chip, LED_PINS, 1, &led_lines);
	if (error) { fprintf(stderr, "No lines. damn."); return -1; }
	error = gpiod_chip_get_lines(chip, RESET_PINS, 1, &reset_lines);
	if (error) { fprintf(stderr, "No lines. damn."); return -1; }
	error = gpiod_chip_get_lines(chip, BACKLIGHT_PINS, 1, &backlight_lines);
	if (error) { fprintf(stderr, "No lines. damn."); return -1; }
	
	memset(&input_config, 0, sizeof(input_config));
	input_config.consumer = "clic";
	input_config.request_type = GPIOD_LINE_REQUEST_DIRECTION_INPUT;
	input_config.flags = 0;
	
	memset(&output_config, 0, sizeof(output_config));
	output_config.consumer = "clic";
	output_config.request_type = GPIOD_LINE_REQUEST_DIRECTION_OUTPUT;
	output_config.flags = 0;
	
	error = gpiod_line_request_bulk(&button_lines, &input_config, NULL);
	if (error) { fprintf(stderr, "No get lines. damn."); return -1; }
	
	int value = 0;
	error = gpiod_line_request_bulk(&led_lines, &output_config, &value);
	if (error) { fprintf(stderr, "No get lines. damn."); return -1; }
	
	value = 1;
	error = gpiod_line_request_bulk(&reset_lines, &output_config, &value);
	if (error) { fprintf(stderr, "No get lines. damn."); return -1; }
	error = gpiod_line_request_bulk(&backlight_lines, &output_config, &value);
	if (error) { fprintf(stderr, "No get lines. damn."); return -1; }
	
}

void cleanup_gpio() {
	gpiod_line_release_bulk(&reset_lines);
	gpiod_line_release_bulk(&backlight_lines);
	gpiod_line_release_bulk(&led_lines);
	gpiod_line_release_bulk(&button_lines);
	//gpiod_chip_close(chip);
}


#endif
