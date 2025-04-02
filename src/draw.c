#include <GL/glcorearb.h>
#include <stdbool.h>

GLuint vbo = 0;

bool gf_draw_box(float x, float y, float width, float height) {
  glCreateBuffers(1, &vbo);
  //	glNamedBufferStorage()
	return true;
}
