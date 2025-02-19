

main: br

build:
	gcc main.c -o gui -lglfw -lGL -lpng #-lgpiod
run:
	xinit

br: b r
b: build
r: run

