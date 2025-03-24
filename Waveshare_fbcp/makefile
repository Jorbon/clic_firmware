
spi_divisor=20

dc_pin=16
reset_pin=20
backlight_pin=21

main:
	cd build; \
	rm -r *; \
	cmake \
		-DSPI_BUS_CLOCK_DIVISOR=$(spi_divisor) \
		-DWAVESHARE_2INCH4_LCD=ON \
		-DSTATISTICS=0 \
		-DGPIO_TFT_DATA_CONTROL=$(dc_pin) \
		-DGPIO_TFT_RESET_PIN=$(reset_pin) \
		-DGPIO_TFT_BACKLIGHT=$(backlight_pin) \
		..; \
	make -j
