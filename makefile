
libs = -lglfw -lGL -lpng -ljpeg -lgpiod
warnings = -Wno-discarded-qualifiers

build:
	gcc src/main.c -o gui $(libs) $(warnings)
run:
	sudo xinit

br: b r
b: build
r: run

