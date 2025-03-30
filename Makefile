CFLAGS  := -std=gnu99 -Iinclude -Wall -Werror -lEGL

eglintro: main.c
	gcc $< $(CFLAGS) -o $@

run: eglintro
	./eglintro
