CFLAGS  := -std=gnu99 -Iinclude -Wall -Werror -lEGL -lwayland-client

include/xdg_shell.h: /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
	wayland-scanner client-header $< $@

xdg_shell.c: /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
	wayland-scanner code $< $@

eglintro: main.c include/xdg_shell.h xdg_shell.c
	gcc $< xdg_shell.c $(CFLAGS) -o $@

run: eglintro
	./eglintro
