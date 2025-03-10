#include <assert.h>
#include <string.h>

#include "../include/napi.h"
#include "helpers.h"

bool init_called = false;

static napi_value
init(napi_env env, napi_value exports) {
  init_called = true;

  return exports;
}

NAPI_MODULE(foo, init);

int
main() {
  napi_register_module_v1(NULL, NULL);

  assert(init_called);
}
