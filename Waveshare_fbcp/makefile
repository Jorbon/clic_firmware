
spi_divisor=20

dc_pin=18

main:
	cd build; \
	rm -r *; \
	cmake \
		-DSPI_BUS_CLOCK_DIVISOR=$(spi_divisor) \
		-DWAVESHARE_2INCH4_LCD=ON \
		-DSTATISTICS=0 \
		-DGPIO_TFT_DATA_CONTROL=$(dc_pin) \
		..; \
	make -j
