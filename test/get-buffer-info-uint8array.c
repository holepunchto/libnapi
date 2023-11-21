#include <assert.h>

#include "../include/napi.h"
#include "helpers.h"

int
main () {
  int e;

  napi_env env = napi_setup_env();

  napi_value script;
  e = napi_create_string_utf8(env, "Uint8Array.of(1, 2, 3, 4)", -1, &script);
  assert(e == 0);

  napi_value buffer;
  e = napi_run_script(env, script, &buffer);
  assert(e == 0);

  uint8_t *data;
  size_t len;
  e = napi_get_buffer_info(env, buffer, (void **) &data, &len);
  assert(e == 0);

  assert(len == 4);

  assert(data[0] == 1);
  assert(data[1] == 2);
  assert(data[2] == 3);
  assert(data[3] == 4);

  napi_teardown_env();
}
