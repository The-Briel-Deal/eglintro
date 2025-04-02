CFLAGS  := -std=gnu99 -Iinclude -Wall -Werror -lEGL -lwayland-egl -lwayland-client -lGL

include/xdg_shell.h: /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
	wayland-scanner client-header $< $@

xdg_shell.c: /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
	wayland-scanner private-code $< $@

eglintro: main.c include/xdg_shell.h xdg_shell.c
	gcc $< xdg_shell.c $(CFLAGS) -o $@

run: eglintro
	./eglintro
