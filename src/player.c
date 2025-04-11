
#include "player.h"
#include "common.h"
#include "log.h"
#include "render.h"

#define PLAYER_LIST_MAX 128

enum gf_player_input_state {
  GF_PLAYER_INPUT_UP    = 0b0001,
  GF_PLAYER_INPUT_DOWN  = 0b0010,
  GF_PLAYER_INPUT_RIGHT = 0b0100,
  GF_PLAYER_INPUT_LEFT  = 0b1000,
};

struct gf_player {
  struct gf_obj *obj;
  enum gf_player_input_state input_state;
};

STATIC_LIST(gf_player_list, struct gf_player, 128)

const char *vert_shader_src =
    "#version 450 core\n"
    "layout (location = " TO_STR(GF_ATTRIB_VERT_LOCATION) ") in vec2 aPos;\n"
		"layout (location = " TO_STR(GF_UNIFORM_TRANSFORM_MAT_LOCATION) ") uniform mat4 model;"
		"layout (location = " TO_STR(GF_UNIFORM_PROJECTION_MAT_LOCATION) ") uniform mat4 projection;"
		"\n"
    "out vec4 vertexColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position = projection * model * vec4(aPos, 0.0, 1.0);\n"
    "    vertexColor = vec4(0.5, 0.0, 0.0, 1.0);\n"
    "}\n";

const char *frag_shader_src =
    "#version 450 core\n"
    "in vec4 vertexColor;\n"
    "out vec4 FragColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    FragColor = vertexColor;\n"
    "}\n";

#ifdef GF_DEBUG_PLAYER_INPUT

static void gf_debug_log_player_input(enum gf_player_input_state input_state) {
  char log_str[128] = "Player input state = [ ";
  if (input_state & GF_PLAYER_INPUT_UP) {
    strcat(log_str, "Up, ");
  }
  if (input_state & GF_PLAYER_INPUT_DOWN) {
    strcat(log_str, "Down, ");
  }
  if (input_state & GF_PLAYER_INPUT_RIGHT) {
    strcat(log_str, "Right, ");
  }
  if (input_state & GF_PLAYER_INPUT_LEFT) {
    strcat(log_str, "Left, ");
  }

  strcat(log_str, "]");

  gf_log(DEBUG_LOG, log_str);
}

#endif

struct gf_player *gf_player_create() {
  if (gf_player_list.count + 1 >= gf_player_list.capacity) {
    gf_log(DEBUG_LOG,
           "`gf_player_list` has a count of '%i', which is greater than "
           "it's capacity of '%i'.",
           gf_player_list.count, gf_player_list.capacity);
    return NULL;
  }

  struct gf_player *player = &gf_player_list.items[gf_player_list.count++];
  player->input_state      = 0b0000;

  player->obj = gf_obj_create_box();
  struct gf_shader *shader =
      gf_compile_shaders(vert_shader_src, frag_shader_src);
  gf_obj_set_shader(player->obj, shader);
  gf_obj_commit_state(player->obj);

  return player;
}

void gf_player_draw(struct gf_player *player) {
#ifdef GF_DEBUG_PLAYER_INPUT
  gf_debug_log_player_input(player->input_state);
#endif
  gf_obj_commit_state(player->obj);
  gf_obj_draw(player->obj);
}
