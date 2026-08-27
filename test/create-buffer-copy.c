#include <assert.h>
#include <string.h>

#include "../include/napi.h"
#include "helpers.h"

int
main() {
  int e;

  napi_env env = napi_setup_env();

  uint8_t source[64];

  for (size_t i = 0; i < 64; i++) source[i] = (uint8_t) (i + 1);

  uint8_t *data;
  napi_value buffer;
  e = napi_create_buffer_copy(env, 64, source, (void **) &data, &buffer);
  assert(e == 0);

  bool is_buffer;
  e = napi_is_buffer(env, buffer, &is_buffer);
  assert(e == 0);

  assert(is_buffer);

  uint8_t *buffer_data;
  size_t buffer_len;
  e = napi_get_buffer_info(env, buffer, (void **) &buffer_data, &buffer_len);
  assert(e == 0);

  assert(buffer_len == 64);

  // The returned pointer should be the buffer's own storage, not the caller's
  // variable, and it should hold a copy of the source data.
  assert(buffer_data == data);

  assert(memcmp(buffer_data, source, 64) == 0);

  // The source must be left untouched.
  for (size_t i = 0; i < 64; i++) assert(source[i] == (uint8_t) (i + 1));

  // The copy must also happen when the caller doesn't ask for the pointer.
  napi_value other;
  e = napi_create_buffer_copy(env, 64, source, NULL, &other);
  assert(e == 0);

  e = napi_get_buffer_info(env, other, (void **) &buffer_data, &buffer_len);
  assert(e == 0);

  assert(buffer_len == 64);

  assert(memcmp(buffer_data, source, 64) == 0);

  napi_teardown_env();
}
