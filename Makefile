CFLAGS  := -std=gnu99 -Iinclude -Wall -Werror -lEGL

include/xdg_shell.h: /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
	wayland-scanner client-header $< $@

eglintro: main.c include/xdg_shell.h
	gcc $< $(CFLAGS) -o $@

run: eglintro
	./eglintro
