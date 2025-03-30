#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#include <GL/gl.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  assert(display != EGL_NO_DISPLAY);

  EGLint major;
  EGLint minor;
  EGLBoolean initialized = eglInitialize(display, &major, &minor);
  assert(initialized);
  printf("EGL Version - v%i.%i\n", major, minor);

  EGLint config_size = 100;
  EGLConfig configs[config_size];
  EGLint num_config;
  EGLBoolean get_config_success =
      eglGetConfigs(display, configs, config_size, &num_config);
  assert(get_config_success);
	printf("Number of Configs: %i\n",num_config);

  for (int i = 0; i < num_config; i++) {
    EGLint samples;
    EGLBoolean get_config_attr_success =
        eglGetConfigAttrib(display, configs[i], EGL_SAMPLES, &samples);
    assert(get_config_attr_success);
    printf("Config %i - Num Samples: %i\n", i, samples);
  }
}
