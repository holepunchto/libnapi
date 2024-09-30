#include <assert.h>

#include "../include/napi.h"
#include "helpers.h"

static napi_async_work work;

static bool execute_called = false;
static bool complete_called = false;

static void
on_execute (napi_env env, void *data) {
  execute_called = true;

  assert((intptr_t) data == 42);
}

static void
on_complete (napi_env env, napi_status status, void *data) {
  int e;

  complete_called = true;

  assert((intptr_t) data == 42);

  e = napi_delete_async_work(env, work);
  assert(e == 0);
}

int
main () {
  int e;

  napi_env env = napi_setup_env();

  e = napi_create_async_work(env, NULL, NULL, on_execute, on_complete, (void *) 42, &work);
  assert(e == 0);

  e = napi_queue_async_work(env, work);
  assert(e == 0);

  napi_run_env();

  assert(execute_called);
  assert(complete_called);

  napi_teardown_env();
}
