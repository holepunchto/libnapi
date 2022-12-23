#ifndef NAPI_TEST_HELPERS_H
#define NAPI_TEST_HELPERS_H

#include <assert.h>
#include <js.h>
#include <uv.h>

#include "../include/napi.h"

static js_env_t *env;
static js_platform_t *platform;

static inline napi_env
napi_setup_env () {
  int e;

  uv_loop_t *loop = uv_default_loop();

  e = js_create_platform(loop, NULL, &platform);
  assert(e == 0);

  e = js_create_env(loop, platform, &env);
  assert(e == 0);

  return env;
}

static inline void
napi_teardown_env () {
  int e;

  e = js_destroy_env(env);
  assert(e == 0);

  e = js_destroy_platform(platform);
  assert(e == 0);
}

#endif // NAPI_TEST_HELPERS_H
