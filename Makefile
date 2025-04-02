CFLAGS  := -std=gnu99 -Iinclude -Ibuild/protocols/include -Wall -Werror -lEGL -lwayland-egl -lwayland-client -lGL -DGL_GLEXT_PROTOTYPES
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

build/protocols/include/cursor-shape.h: /usr/share/wayland-protocols/staging/cursor-shape/cursor-shape-v1.xml | build/
	wayland-scanner client-header $< $@

build/protocols/src/cursor-shape.c: /usr/share/wayland-protocols/staging/cursor-shape/cursor-shape-v1.xml | build/
	wayland-scanner private-code $< $@

build/protocols/cursor-shape.o: build/protocols/src/cursor-shape.c
	gcc $< -c $(CFLAGS) -o $@

# For some reason cursor-shape depends on tablet-tool
build/protocols/include/tablet.h: /usr/share/wayland-protocols/stable/tablet/tablet-v2.xml | build/
	wayland-scanner client-header $< $@

build/protocols/src/tablet.c: /usr/share/wayland-protocols/stable/tablet/tablet-v2.xml | build/
	wayland-scanner private-code $< $@

build/protocols/tablet.o: build/protocols/src/tablet.c
	gcc $< -c $(CFLAGS) -o $@

eglintro: $(OBJECTS) build/protocols/xdg-shell.o build/protocols/cursor-shape.o build/protocols/tablet.o
	gcc $^ $(CFLAGS) -o $@

run: eglintro
	./eglintro
