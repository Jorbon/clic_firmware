

build:
	gcc main.c -o gui -lglfw -lGL -lpng -lgpiod
run:
	sudo xinit

br: b r
b: build
r: run

