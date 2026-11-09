#include <assert.h>
#include <stdbool.h>

#include "../include/napi.h"
#include "helpers.h"

static bool execute_called = false;

static void
on_execute(napi_env env, void *data) {
  execute_called = true;
}

int
main() {
  int e;

  napi_env env = napi_setup_env();

  napi_async_work work;
  e = napi_create_async_work(env, NULL, NULL, on_execute, NULL, NULL, &work);
  assert(e == napi_ok);

  e = napi_queue_async_work(env, work);
  assert(e == napi_ok);

  napi_run_env();

  assert(execute_called);

  e = napi_delete_async_work(env, work);
  assert(e == napi_ok);

  napi_teardown_env();
}
