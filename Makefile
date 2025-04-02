CFLAGS  := -std=gnu99 -Iinclude -Ibuild/protocols/include -Wall -Werror -lEGL -lwayland-egl -lwayland-client -lGL
HEADERS := $(wildcard include/*.h) build/protocols/include/xdg-shell.h
SOURCES := $(wildcard src/*.c)
OBJECTS := $(addprefix build/, $(notdir $(SOURCES:.c=.o)))

build/:
	mkdir -p build
	mkdir -p build/protocols
	mkdir -p build/protocols/src
	mkdir -p build/protocols/include

# Compile Object Files
build/%.o: src/%.c $(HEADERS) | build/
	gcc $< -c $(CFLAGS) -o $@

build/protocols/include/xdg-shell.h: /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml | build/
	wayland-scanner client-header $< $@

build/protocols/src/xdg-shell.c: /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml | build/
	wayland-scanner private-code $< $@

build/protocols/xdg-shell.o: build/protocols/src/xdg-shell.c
	gcc $< -c $(CFLAGS) -o $@

eglintro: $(OBJECTS) build/protocols/xdg-shell.o
	gcc $^ $(CFLAGS) -o $@

run: eglintro
	./eglintro
