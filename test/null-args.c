#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "../include/napi.h"
#include "helpers.h"

static napi_value
on_call(napi_env env, napi_callback_info info) {
  return NULL;
}

int
main() {
  int e;

  napi_env env = napi_setup_env();

  napi_value number;
  e = napi_create_int32(env, 42, &number);
  assert(e == 0);

  napi_value object;
  e = napi_create_object(env, &object);
  assert(e == 0);

  napi_value function;
  e = napi_create_function(env, "fn", NAPI_AUTO_LENGTH, on_call, NULL, &function);
  assert(e == 0);

  napi_value value;

  e = napi_create_int32(NULL, 42, &value);
  assert(e == napi_invalid_arg);

  e = napi_typeof(NULL, number, &(napi_valuetype) {0});
  assert(e == napi_invalid_arg);

  e = napi_create_int32(env, 42, NULL);
  assert(e == napi_invalid_arg);

  e = napi_create_object(env, NULL);
  assert(e == napi_invalid_arg);

  e = napi_get_value_int32(env, number, NULL);
  assert(e == napi_invalid_arg);

  e = napi_typeof(env, NULL, &(napi_valuetype) {0});
  assert(e == napi_invalid_arg);

  e = napi_strict_equals(env, number, NULL, &(bool) {false});
  assert(e == napi_invalid_arg);

  e = napi_get_named_property(env, object, NULL, &value);
  assert(e == napi_invalid_arg);

  e = napi_set_property(env, object, number, NULL);
  assert(e == napi_invalid_arg);

  e = napi_create_function(env, "fn", NAPI_AUTO_LENGTH, NULL, NULL, &value);
  assert(e == napi_invalid_arg);

  e = napi_throw_error(env, NULL, NULL);
  assert(e == napi_invalid_arg);

  e = napi_call_function(env, NULL, function, 0, NULL, &value);
  assert(e == napi_invalid_arg);

  e = napi_call_function(env, object, function, 1, NULL, &value);
  assert(e == napi_invalid_arg);

  e = napi_call_function(env, object, function, 0, NULL, NULL);
  assert(e == napi_ok);

  e = napi_new_instance(env, function, 1, NULL, &value);
  assert(e == napi_invalid_arg);

  napi_value bigint;
  e = napi_create_bigint_int64(env, 42, &bigint);
  assert(e == 0);

  size_t word_count = 1;
  uint64_t word;
  e = napi_get_value_bigint_words(env, bigint, NULL, &word_count, &word);
  assert(e == napi_invalid_arg);

  e = napi_get_value_bigint_words(env, bigint, NULL, &word_count, NULL);
  assert(e == napi_ok);
  assert(word_count == 1);

  e = napi_create_buffer_copy(env, 4, NULL, NULL, &value);
  assert(e == napi_invalid_arg);

  e = napi_get_arraybuffer_info(env, NULL, NULL, NULL);
  assert(e == napi_invalid_arg);

  napi_value arraybuffer;
  e = napi_create_arraybuffer(env, 4, NULL, &arraybuffer);
  assert(e == napi_ok);

  e = napi_get_arraybuffer_info(env, arraybuffer, NULL, NULL);
  assert(e == napi_ok);

  e = napi_delete_reference(env, NULL);
  assert(e == napi_invalid_arg);

  e = napi_close_handle_scope(env, NULL);
  assert(e == napi_invalid_arg);

  e = napi_resolve_deferred(env, NULL, number);
  assert(e == napi_invalid_arg);

  e = napi_queue_async_work(env, NULL);
  assert(e == napi_invalid_arg);

  const napi_extended_error_info *info;
  e = napi_get_last_error_info(env, &info);
  assert(e == napi_ok);
  assert(info->error_code == napi_invalid_arg);

  e = napi_call_threadsafe_function(NULL, NULL, napi_tsfn_nonblocking);
  assert(e == napi_invalid_arg);

  e = napi_release_threadsafe_function(NULL, napi_tsfn_release);
  assert(e == napi_invalid_arg);

  e = napi_remove_async_cleanup_hook(NULL);
  assert(e == napi_invalid_arg);

  napi_teardown_env();
}
