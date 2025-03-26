

build:
	gcc src/main.c -o gui -lglfw -lGL -lpng -ljpeg -lgpiod
run:
	sudo xinit

br: b r
b: build
r: run

