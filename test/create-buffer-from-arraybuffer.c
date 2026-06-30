#include <assert.h>

#include "../include/napi.h"
#include "helpers.h"

int
main() {
  int e;

  napi_env env = napi_setup_env();

  uint8_t *data;
  napi_value arraybuffer;
  e = napi_create_arraybuffer(env, 8, (void **) &data, &arraybuffer);
  assert(e == 0);

  for (uint8_t i = 0; i < 8; i++) data[i] = i;

  napi_value buffer;
  e = node_api_create_buffer_from_arraybuffer(env, arraybuffer, 2, 4, &buffer);
  assert(e == 0);

  bool is_buffer;
  e = napi_is_buffer(env, buffer, &is_buffer);
  assert(e == 0);

  assert(is_buffer);

  uint8_t *buffer_data;
  size_t buffer_len;
  e = napi_get_buffer_info(env, buffer, (void **) &buffer_data, &buffer_len);
  assert(e == 0);

  assert(buffer_len == 4);

  // The buffer should view the backing arraybuffer starting at the offset.
  assert(buffer_data == data + 2);

  assert(buffer_data[0] == 2);
  assert(buffer_data[1] == 3);
  assert(buffer_data[2] == 4);
  assert(buffer_data[3] == 5);

  napi_teardown_env();
}
