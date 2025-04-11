#ifndef GF_PLAYER_H
#define GF_PLAYER_H

#include <sys/types.h>
struct gf_player;

struct gf_player *gf_player_create();
void gf_player_draw(struct gf_player *player);

void gf_player_input_listener(u_int key, void *data);

#endif
