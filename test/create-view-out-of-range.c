#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "../include/napi.h"
#include "helpers.h"

static void
assert_range_error(napi_env env, napi_status status) {
  int e;

  assert(status == napi_pending_exception);

  napi_value exception;
  e = napi_get_and_clear_last_exception(env, &exception);
  assert(e == napi_ok);

  bool is_error;
  e = napi_is_error(env, exception, &is_error);
  assert(e == napi_ok);
  assert(is_error);
}

int
main() {
  int e;

  napi_env env = napi_setup_env();

  napi_value arraybuffer;
  e = napi_create_arraybuffer(env, 8, NULL, &arraybuffer);
  assert(e == 0);

  napi_value sharedarraybuffer;
  e = node_api_create_sharedarraybuffer(env, 8, NULL, &sharedarraybuffer);
  assert(e == 0);

  napi_value buffers[] = {arraybuffer, sharedarraybuffer};

  for (size_t i = 0; i < 2; i++) {
    napi_value buffer = buffers[i];
    napi_value view;

    e = napi_create_typedarray(env, napi_int32_array, 1, buffer, 4, &view);
    assert(e == napi_ok);

    e = napi_create_typedarray(env, napi_int32_array, 1, buffer, 2, &view);
    assert_range_error(env, e);

    e = napi_create_typedarray(env, napi_int32_array, 2, buffer, 4, &view);
    assert_range_error(env, e);

    e = napi_create_typedarray(env, napi_uint8_array, SIZE_MAX, buffer, 1, &view);
    assert_range_error(env, e);

    e = napi_create_typedarray(env, napi_uint8_array, 0, buffer, 9, &view);
    assert_range_error(env, e);

    e = napi_create_dataview(env, 4, buffer, 4, &view);
    assert(e == napi_ok);

    e = napi_create_dataview(env, 4, buffer, 5, &view);
    assert_range_error(env, e);

    e = napi_create_dataview(env, SIZE_MAX, buffer, 1, &view);
    assert_range_error(env, e);

    e = node_api_create_buffer_from_arraybuffer(env, buffer, 4, 4, &view);
    assert(e == napi_ok);

    e = node_api_create_buffer_from_arraybuffer(env, buffer, 4, 5, &view);
    assert_range_error(env, e);

    e = node_api_create_buffer_from_arraybuffer(env, buffer, 9, 0, &view);
    assert_range_error(env, e);
  }

  napi_teardown_env();
}
