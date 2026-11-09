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

  napi_value null;
  e = napi_get_null(env, &null);
  assert(e == 0);

  napi_value undefined;
  e = napi_get_undefined(env, &undefined);
  assert(e == 0);

  napi_value number;
  e = napi_create_int32(env, 42, &number);
  assert(e == 0);

  napi_value string;
  e = napi_create_string_utf8(env, "hello", -1, &string);
  assert(e == 0);

  napi_value object;
  e = napi_create_object(env, &object);
  assert(e == 0);

  napi_value function;
  e = napi_create_function(env, "fn", -1, on_call, NULL, &function);
  assert(e == 0);

  napi_value nullish[] = {null, undefined};

  for (size_t i = 0; i < 2; i++) {
    napi_value result;
    e = napi_get_named_property(env, nullish[i], "length", &result);
    assert(e == napi_object_expected);

    e = napi_set_element(env, nullish[i], 0, number);
    assert(e == napi_object_expected);

    e = napi_type_tag_object(env, nullish[i], &(napi_type_tag) {1, 2});
    assert(e == napi_object_expected);
  }

  napi_value length_key;
  e = napi_create_string_utf8(env, "length", -1, &length_key);
  assert(e == 0);

  napi_value length;
  e = napi_get_named_property(env, string, "length", &length);
  assert(e == napi_ok);

  uint32_t len;
  e = napi_get_value_uint32(env, length, &len);
  assert(e == napi_ok);
  assert(len == 5);

  int data;
  void *result;

  e = napi_wrap(env, number, &data, NULL, NULL, NULL);
  assert(e == napi_invalid_arg);

  e = napi_unwrap(env, number, &result);
  assert(e == napi_invalid_arg);

  e = napi_remove_wrap(env, number, &result);
  assert(e == napi_invalid_arg);

  e = napi_add_finalizer(env, number, &data, NULL, NULL, NULL);
  assert(e == napi_invalid_arg);

  e = napi_wrap(env, function, &data, NULL, NULL, NULL);
  assert(e == napi_ok);

  e = napi_unwrap(env, function, &result);
  assert(e == napi_ok);
  assert(result == &data);

  napi_value value;
  e = napi_call_function(env, undefined, object, 0, NULL, &value);
  assert(e == napi_function_expected);

  e = napi_make_callback(env, NULL, undefined, object, 0, NULL, &value);
  assert(e == napi_function_expected);

  e = napi_new_instance(env, object, 0, NULL, &value);
  assert(e == napi_function_expected);

  e = napi_call_function(env, undefined, function, 0, NULL, &value);
  assert(e == napi_ok);

  napi_threadsafe_function tsfn;
  e = napi_create_threadsafe_function(env, object, NULL, string, 0, 1, NULL, NULL, NULL, NULL, &tsfn);
  assert(e == napi_function_expected);

  e = napi_create_threadsafe_function(env, NULL, NULL, string, 0, 1, NULL, NULL, NULL, NULL, &tsfn);
  assert(e == napi_invalid_arg);

  bool has;
  e = napi_has_own_property(env, object, number, &has);
  assert(e == napi_name_expected);

  e = napi_has_own_property(env, string, length_key, &has);
  assert(e == napi_ok);
  assert(has);

  bool is_instance;
  e = napi_instanceof(env, object, null, &is_instance);
  assert(e == napi_function_expected);

  e = napi_instanceof(env, object, object, &is_instance);
  assert(e == napi_function_expected);

  bool is_pending;
  e = napi_is_exception_pending(env, &is_pending);
  assert(e == napi_ok);
  assert(!is_pending);

  e = napi_instanceof(env, object, function, &is_instance);
  assert(e == napi_ok);
  assert(!is_instance);

  napi_teardown_env();
}
