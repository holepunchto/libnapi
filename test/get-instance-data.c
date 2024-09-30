#include <assert.h>

#include "../include/napi.h"
#include "helpers.h"

static bool finalize_called = false;

static void
on_finalize (napi_env env, void *data, void *finalize_hint) {
  finalize_called = true;

  assert((intptr_t) data == 42);
}

int
main () {
  int e;

  napi_env env = napi_setup_env();

  e = napi_set_instance_data(env, (void *) 42, on_finalize, NULL);
  assert(e == 0);

  intptr_t data;
  e = napi_get_instance_data(env, (void **) &data);
  assert(e == 0);

  napi_teardown_env();

  assert(finalize_called);
}
