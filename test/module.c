#include <assert.h>
#include <string.h>

#include "../include/napi.h"
#include "helpers.h"

bool register_called = false;

static napi_value
init (napi_env env, napi_value exports) {
  return exports;
}

NAPI_MODULE(foo, init);

void
napi_module_register (napi_module *mod) {
  register_called = true;

  assert(strcmp(mod->nm_modname, "foo") == 0);
  assert(strcmp(mod->nm_filename, __FILE__) == 0);
  assert(mod->nm_register_func == init);
}

int
main () {
  assert(register_called);
}
