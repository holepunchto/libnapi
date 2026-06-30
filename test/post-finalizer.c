#include <assert.h>

#include "../include/napi.h"
#include "helpers.h"

static bool finalize_called = false;

static void
on_finalize(napi_env env, void *data, void *finalize_hint) {
  finalize_called = true;

  assert((intptr_t) data == 42);
  assert((intptr_t) finalize_hint == 1337);
}

int
main() {
  int e;

  napi_env env = napi_setup_env();

  e = node_api_post_finalizer(env, on_finalize, (void *) 42, (void *) 1337);
  assert(e == 0);

  napi_run_env();

  assert(finalize_called);

  napi_teardown_env();
}
