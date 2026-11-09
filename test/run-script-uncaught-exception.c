#include <assert.h>
#include <stdbool.h>

#include "../include/napi.h"
#include "helpers.h"

static int uncaught_count = 0;

static void
on_uncaught_exception(js_env_t *env, js_value_t *error, void *data) {
  uncaught_count++;
}

int
main() {
  int e;

  napi_env env = napi_setup_env();

  napi_value script;
  e = napi_create_string_utf8(env, "throw new Error('uncaught')", NAPI_AUTO_LENGTH, &script);
  assert(e == napi_ok);

  napi_value result;
  e = napi_run_script(env, script, &result);
  assert(e == napi_pending_exception);

  napi_value exception;
  e = napi_get_and_clear_last_exception(env, &exception);
  assert(e == napi_ok);

  bool is_error;
  e = napi_is_error(env, exception, &is_error);
  assert(e == napi_ok);
  assert(is_error);

  e = js_on_uncaught_exception(env, on_uncaught_exception, NULL);
  assert(e == 0);

  e = napi_run_script(env, script, &result);
  assert(e == napi_generic_failure);
  assert(uncaught_count == 1);

  const napi_extended_error_info *info;
  e = napi_get_last_error_info(env, &info);
  assert(e == napi_ok);
  assert(info->error_code == napi_generic_failure);
  assert(info->engine_error_code == (uint32_t) js_uncaught_exception);

  bool is_pending;
  e = napi_is_exception_pending(env, &is_pending);
  assert(e == napi_ok);
  assert(!is_pending);

  e = napi_get_and_clear_last_exception(env, &exception);
  assert(e == napi_ok);

  napi_valuetype type;
  e = napi_typeof(env, exception, &type);
  assert(e == napi_ok);
  assert(type == napi_undefined);

  napi_teardown_env();
}
