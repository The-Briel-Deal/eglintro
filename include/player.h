#ifndef GF_PLAYER_H
#define GF_PLAYER_H

#include <stdbool.h>
#include <sys/types.h>
#include <xkbcommon/xkbcommon.h>
struct gf_player;

struct gf_player *gf_player_create();
void gf_player_draw(struct gf_player *player);

void gf_player_input_listener(xkb_keysym_t key, bool pressed, void *data);

#endif
