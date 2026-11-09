#include <assert.h>
#include <stdbool.h>

#include "../include/napi.h"
#include "helpers.h"

static void
on_cleanup(void *data) {}

static void
on_async_cleanup(napi_async_cleanup_hook_handle handle, void *data) {}

static void
assert_pending_exception(napi_env env, napi_status status) {
  int e;

  assert(status == napi_pending_exception);

  napi_value exception;
  e = napi_get_and_clear_last_exception(env, &exception);
  assert(e == napi_ok);

  bool is_error;
  e = napi_is_error(env, exception, &is_error);
  assert(e == napi_ok);
  assert(is_error);
}

int
main() {
  int e;

  napi_env env = napi_setup_env();

  int data;

  e = napi_remove_env_cleanup_hook(env, on_cleanup, &data);
  assert_pending_exception(env, e);

  e = napi_add_env_cleanup_hook(env, on_cleanup, &data);
  assert(e == napi_ok);

  e = napi_add_env_cleanup_hook(env, on_cleanup, &data);
  assert_pending_exception(env, e);

  e = napi_remove_env_cleanup_hook(env, on_cleanup, &data);
  assert(e == napi_ok);

  napi_async_cleanup_hook_handle handle;
  e = napi_add_async_cleanup_hook(env, on_async_cleanup, &data, &handle);
  assert(e == napi_ok);

  napi_async_cleanup_hook_handle duplicate;
  e = napi_add_async_cleanup_hook(env, on_async_cleanup, &data, &duplicate);
  assert_pending_exception(env, e);

  e = napi_remove_async_cleanup_hook(handle);
  assert(e == napi_ok);

  napi_teardown_env();
}
