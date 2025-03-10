#include <assert.h>

#include "../include/napi.h"
#include "helpers.h"

int
main() {
  int e;

  napi_env env = napi_setup_env();

  napi_value script;
  e = napi_create_string_utf8(env, "(x) => x + 42", -1, &script);
  assert(e == 0);

  napi_value fn;
  e = napi_run_script(env, script, &fn);
  assert(e == 0);

  napi_value global;
  e = napi_get_global(env, &global);
  assert(e == 0);

  napi_value args[1];
  e = napi_create_uint32(env, 42, &args[0]);
  assert(e == 0);

  napi_value result;
  e = napi_call_function(env, global, fn, 1, (const napi_value *) args, &result);
  assert(e == 0);

  uint32_t value;
  e = napi_get_value_uint32(env, result, &value);
  assert(e == 0);

  assert(value == 84);

  napi_teardown_env();
}
