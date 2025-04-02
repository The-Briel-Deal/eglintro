#include "egl.h"
#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <wayland-client-core.h>
#include <wayland-egl-core.h>


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
  case value: return #value;
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
    default: return "Unknown";
  }
#undef CASE_STR
}

bool init_egl(struct gf_egl_state *gf_egl_state, struct wl_display *wl_display,
              struct wl_egl_window *wl_egl_window) {
  gf_egl_state->display = eglGetDisplay(wl_display);
  assert(gf_egl_state->display != EGL_NO_DISPLAY);
  EGLBoolean success;
  EGLint major;
  EGLint minor;
  success = eglInitialize(gf_egl_state->display, &major, &minor);
  assert(success);
  printf("EGL Version - v%i.%i\n", major, minor);

  EGLBoolean ok = eglBindAPI(EGL_OPENGL_API);
  assert(ok);

  /* config selection */
  EGLConfig config;
  EGLint num_config;
  success = eglChooseConfig(gf_egl_state->display, config_attribs, &config, 1,
                            &num_config);
  assert(success && num_config == 1);

  /* create context */
  gf_egl_state->context = eglCreateContext(gf_egl_state->display, config,
                                           EGL_NO_CONTEXT, context_attribs);
  assert(gf_egl_state->context != EGL_NO_CONTEXT);

  /* Init egl_surface */
  gf_egl_state->surface = eglCreateWindowSurface(
      gf_egl_state->display, config, (EGLNativeWindowType)wl_egl_window,
      surface_attribs);
  assert(gf_egl_state->surface != NULL);

  eglMakeCurrent(gf_egl_state->display, gf_egl_state->surface,
                 gf_egl_state->surface, gf_egl_state->context);

  return true;
}
