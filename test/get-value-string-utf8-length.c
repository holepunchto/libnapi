#include <assert.h>

#include "../include/napi.h"
#include "helpers.h"

int
main() {
  int e;

  napi_env env = napi_setup_env();

  napi_value string;
  e = napi_create_string_utf8(env, "hello", -1, &string);
  assert(e == 0);

  size_t len;
  e = napi_get_value_string_utf8(env, string, NULL, 0, &len);
  assert(e == 0);

  assert(len == 5);

  napi_teardown_env();
}
