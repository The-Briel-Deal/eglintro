#include <GL/gl.h>
#include <GL/glcorearb.h>
#include <GL/glext.h>
#include <stdbool.h>

GLuint vbo          = 0;
GLuint vao          = 0;
GLuint vertexShader = 0;
GLuint fragShader   = 0;

struct vertex {
  float x;
  float y;
};

struct triangle {
  struct vertex v1;
  struct vertex v2;
  struct vertex v3;
};
static char vert_shader[] =
    "#version 450 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "out vec4 vertexColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos, 0.0, 1.0);\n"
    "    vertexColor = vec4(0.5, 0.0, 0.0, 1.0);\n"
    "}\n";

static char frag_shader[] =
    "#version 450 core\n"
    "in vec4 vertexColor;\n"
    "out vec4 FragColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    FragColor = vertexColor;\n"
    "}\n";

bool gf_draw_box(float x, float y, float width, float height) {
  glCreateBuffers(1, &vbo);
  struct triangle triangle = {
      {.x = 0.5,  .y = -0.5},
      {.x = 0.0,  .y = 0.5 },
      {.x = -0.5, .y = -0.5},
  };
  glNamedBufferStorage(vbo, sizeof(triangle), &triangle,
                       GL_DYNAMIC_STORAGE_BIT);

  glCreateVertexArrays(1, &vao);
  glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(struct vertex));
  glEnableVertexArrayAttrib(vao, 0);
  glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribBinding(vao, 0, 0);

  return true;
}
