#ifndef NAPI_TEST_HELPERS_H
#define NAPI_TEST_HELPERS_H

#include <assert.h>
#include <js.h>
#include <uv.h>

#include "../include/napi.h"

static js_env_t *env;
static js_platform_t *platform;
static js_handle_scope_t *scope;

static inline napi_env
napi_setup_env() {
  int e;

  uv_loop_t *loop = uv_default_loop();

  e = js_create_platform(loop, NULL, &platform);
  assert(e == 0);

  e = js_create_env(loop, platform, NULL, &env);
  assert(e == 0);

  e = js_open_handle_scope(env, &scope);
  assert(e == 0);

  return env;
}

static inline void
napi_teardown_env() {
  int e;

  e = js_close_handle_scope(env, scope);
  assert(e == 0);

  e = js_destroy_env(env);
  assert(e == 0);

  e = js_destroy_platform(platform);
  assert(e == 0);
}

static inline int
napi_run_env() {
  int e;

  uv_loop_t *loop = uv_default_loop();

  e = uv_run(loop, UV_RUN_DEFAULT);

  return e;
}

#endif // NAPI_TEST_HELPERS_H
