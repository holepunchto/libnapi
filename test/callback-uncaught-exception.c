#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "../include/napi.h"
#include "helpers.h"

static napi_async_work work;

static int uncaught_count = 0;

static char uncaught_message[16];

static void
on_uncaught_exception(js_env_t *env, js_value_t *error, void *data) {
  int e;

  uncaught_count++;

  napi_value message;
  e = napi_get_named_property(env, error, "message", &message);
  assert(e == napi_ok);

  e = napi_get_value_string_utf8(env, message, uncaught_message, sizeof(uncaught_message), NULL);
  assert(e == napi_ok);
}

static void
on_execute(napi_env env, void *data) {}

static void
on_complete(napi_env env, napi_status status, void *data) {
  int e;

  e = napi_delete_async_work(env, work);
  assert(e == napi_ok);

  e = napi_throw_error(env, NULL, "complete");
  assert(e == napi_ok);
}

static void
on_finalize(napi_env env, void *data, void *hint) {
  int e;

  e = napi_throw_error(env, NULL, "finalize");
  assert(e == napi_ok);
}

static void
assert_no_pending_exception(napi_env env) {
  int e;

  bool is_pending;
  e = napi_is_exception_pending(env, &is_pending);
  assert(e == napi_ok);
  assert(!is_pending);
}

int
main() {
  int e;

  napi_env env = napi_setup_env();

  e = js_on_uncaught_exception(env, on_uncaught_exception, NULL);
  assert(e == 0);

  e = napi_create_async_work(env, NULL, NULL, on_execute, on_complete, NULL, &work);
  assert(e == napi_ok);

  e = napi_queue_async_work(env, work);
  assert(e == napi_ok);

  napi_run_env();

  assert(uncaught_count == 1);
  assert(strcmp(uncaught_message, "complete") == 0);

  assert_no_pending_exception(env);

  e = node_api_post_finalizer(env, on_finalize, NULL, NULL);
  assert(e == napi_ok);

  assert(uncaught_count == 2);
  assert(strcmp(uncaught_message, "finalize") == 0);

  assert_no_pending_exception(env);

  napi_teardown_env();
}
