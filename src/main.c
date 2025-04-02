
// Enabling for Debug logging.
#include "window.h"
#define GL_GLEXT_PROTOTYPES

#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#include <GL/gl.h>
#include <GL/glcorearb.h>
#include <GL/glext.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-egl.h>
#include <wayland-util.h>

EGLDisplay egl_display = EGL_NO_DISPLAY;
EGLSurface egl_surface = EGL_NO_SURFACE;
EGLContext egl_context = EGL_NO_CONTEXT;

static const EGLint config_attribs[] = {
    EGL_SURFACE_TYPE,
    EGL_WINDOW_BIT,
    EGL_CONFORMANT,
    EGL_OPENGL_BIT,
    EGL_RENDERABLE_TYPE,
    EGL_OPENGL_BIT,
    EGL_COLOR_BUFFER_TYPE,
    EGL_RGB_BUFFER,

    EGL_RED_SIZE,
    8,
    EGL_GREEN_SIZE,
    8,
    EGL_BLUE_SIZE,
    8,
    EGL_DEPTH_SIZE,
    24,
    EGL_STENCIL_SIZE,
    8,

    EGL_NONE,
};

static const EGLint surface_attribs[] = {
    EGL_GL_COLORSPACE, EGL_GL_COLORSPACE_LINEAR,
    EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
    EGL_NONE,
};

static const EGLint context_attribs[] = {
    EGL_CONTEXT_MAJOR_VERSION,
    4,
    EGL_CONTEXT_MINOR_VERSION,
    5,
    EGL_CONTEXT_OPENGL_PROFILE_MASK,
    EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
#ifndef NDEBUG

    EGL_CONTEXT_OPENGL_DEBUG,
    EGL_TRUE,
#endif
    EGL_NONE,
};

const char *eglGetErrorString(EGLint error) {
#define CASE_STR(value)                                                        \
  case value:                                                                  \
    return #value;
  switch (error) {
    CASE_STR(EGL_SUCCESS)
    CASE_STR(EGL_NOT_INITIALIZED)
    CASE_STR(EGL_BAD_ACCESS)
    CASE_STR(EGL_BAD_ALLOC)
    CASE_STR(EGL_BAD_ATTRIBUTE)
    CASE_STR(EGL_BAD_CONTEXT)
    CASE_STR(EGL_BAD_CONFIG)
    CASE_STR(EGL_BAD_CURRENT_SURFACE)
    CASE_STR(EGL_BAD_DISPLAY)
    CASE_STR(EGL_BAD_SURFACE)
    CASE_STR(EGL_BAD_MATCH)
    CASE_STR(EGL_BAD_PARAMETER)
    CASE_STR(EGL_BAD_NATIVE_PIXMAP)
    CASE_STR(EGL_BAD_NATIVE_WINDOW)
    CASE_STR(EGL_CONTEXT_LOST)
  default:
    return "Unknown";
  }
#undef CASE_STR
}

void initEGL(struct wl_display *wl_display,
             struct wl_egl_window *wl_egl_window) {
  egl_display = eglGetDisplay(wl_display);
  assert(egl_display != EGL_NO_DISPLAY);
  EGLBoolean success;
  EGLint major;
  EGLint minor;
  success = eglInitialize(egl_display, &major, &minor);
  assert(success);
  printf("EGL Version - v%i.%i\n", major, minor);

  EGLBoolean ok = eglBindAPI(EGL_OPENGL_API);
  assert(ok);

  /* config selection */
  EGLConfig config;
  EGLint num_config;
  success =
      eglChooseConfig(egl_display, config_attribs, &config, 1, &num_config);
  assert(success && num_config == 1);

  /* create context */
  egl_context =
      eglCreateContext(egl_display, config, EGL_NO_CONTEXT, context_attribs);
  assert(egl_context != EGL_NO_CONTEXT);

  /* Init egl_surface */
  egl_surface = eglCreateWindowSurface(
      egl_display, config, (EGLNativeWindowType)wl_egl_window, surface_attribs);
  assert(egl_surface != NULL);

  eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);
}

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar *message, const void *userParam) {
  fprintf(stderr,
          "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
          (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity,
          message);
}

int main() {
  struct gf_window window;
  init_gf_window(&window);
  initEGL(window.display, window.wl_egl_window);

  // During init, enable debug output
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);
  printf("\n\nVersion %s\n\n", glGetString(GL_VERSION));
  while (true) {
    wl_display_dispatch_pending(window.display);

    glClearColor(1.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();

    bool success = eglSwapBuffers(egl_display, egl_surface);
    printf("\neglSwapBuffers error: \n%s\n\n",
           eglGetErrorString(eglGetError()));
    assert(success);
  }
}
