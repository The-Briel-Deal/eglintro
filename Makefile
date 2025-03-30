CFLAGS  := -std=gnu99 -Iinclude -Wall -Werror -lEGL -lwayland-client

include/xdg_shell.h: /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
	wayland-scanner client-header $< $@

eglintro: main.c include/xdg_shell.h
	gcc $< $(CFLAGS) -o $@

run: eglintro
	./eglintro
