#include <assert.h>

#include "../include/napi.h"
#include "helpers.h"

int
main () {
  int e;

  napi_env env = napi_setup_env();

  napi_value string;
  e = napi_create_string_utf8(env, "hello", -1, &string);
  assert(e == 0);

  char value[6];
  size_t written;
  e = napi_get_value_string_utf8(env, string, value, 6, &written);
  assert(e == 0);

  assert(strcmp(value, "hello") == 0);
  assert(value[5] == '\0');
  assert(written == 5);

  napi_teardown_env();
}
