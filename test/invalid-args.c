#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "../include/napi.h"
#include "helpers.h"

static napi_value
on_call(napi_env env, napi_callback_info info) {
  return NULL;
}

static void
on_finalize(napi_env env, void *data, void *hint) {}

static void
on_execute(napi_env env, void *data) {}

int
main() {
  int e;

  napi_env env = napi_setup_env();

  napi_value number;
  e = napi_create_int32(env, 42, &number);
  assert(e == 0);

  napi_value string;
  e = napi_create_string_utf8(env, "hello", -1, &string);
  assert(e == 0);

  napi_value object;
  e = napi_create_object(env, &object);
  assert(e == 0);

  napi_value value;

  e = napi_create_string_utf8(env, NULL, 1, &value);
  assert(e == napi_invalid_arg);

  e = napi_create_string_utf16(env, NULL, 1, &value);
  assert(e == napi_invalid_arg);

  e = napi_create_string_latin1(env, NULL, 1, &value);
  assert(e == napi_invalid_arg);

  e = node_api_create_property_key_utf8(env, NULL, NAPI_AUTO_LENGTH, &value);
  assert(e == napi_invalid_arg);

  e = node_api_symbol_for(env, NULL, 1, &value);
  assert(e == napi_invalid_arg);

  e = napi_create_string_utf8(env, NULL, 0, &value);
  assert(e == napi_ok);

  e = napi_create_bigint_words(env, 0, 1, NULL, &value);
  assert(e == napi_invalid_arg);

  e = napi_create_symbol(env, number, &value);
  assert(e == napi_string_expected);

  e = napi_create_symbol(env, NULL, &value);
  assert(e == napi_ok);

  e = napi_create_error(env, NULL, number, &value);
  assert(e == napi_string_expected);

  e = napi_create_type_error(env, number, string, &value);
  assert(e == napi_string_expected);

  e = napi_create_range_error(env, NULL, string, &value);
  assert(e == napi_ok);

  e = napi_run_script(env, number, &value);
  assert(e == napi_string_expected);

  e = napi_create_typedarray(env, (napi_typedarray_type) 100, 0, NULL, 0, &value);
  assert(e == napi_invalid_arg);

  napi_value names[] = {number};
  napi_value values[] = {number};

  e = node_api_create_object_with_properties(env, NULL, names, values, 1, &value);
  assert(e == napi_name_expected);

  e = node_api_create_object_with_properties(env, number, NULL, NULL, 0, &value);
  assert(e == napi_object_expected);

  e = node_api_create_object_with_properties(env, NULL, NULL, NULL, 0, &value);
  assert(e == napi_ok);

  napi_value prototype;
  e = napi_get_prototype(env, value, &prototype);
  assert(e == napi_ok);

  napi_valuetype type;
  e = napi_typeof(env, prototype, &type);
  assert(e == napi_ok);
  assert(type == napi_null);

  napi_property_descriptor unnamed = {NULL, NULL, on_call, NULL, NULL, NULL, napi_default, NULL};
  napi_property_descriptor misnamed = {NULL, number, on_call, NULL, NULL, NULL, napi_default, NULL};

  e = napi_define_properties(env, object, 1, &unnamed);
  assert(e == napi_name_expected);

  e = napi_define_properties(env, object, 1, &misnamed);
  assert(e == napi_name_expected);

  e = napi_define_class(env, "Class", NAPI_AUTO_LENGTH, on_call, NULL, 1, &misnamed, &value);
  assert(e == napi_name_expected);

  e = napi_add_finalizer(env, object, NULL, NULL, NULL, NULL);
  assert(e == napi_invalid_arg);

  e = napi_add_finalizer(env, object, NULL, on_finalize, NULL, NULL);
  assert(e == napi_ok);

  napi_value function;
  e = napi_create_function(env, "fn", NAPI_AUTO_LENGTH, on_call, NULL, &function);
  assert(e == 0);

  napi_threadsafe_function tsfn;
  e = napi_create_threadsafe_function(env, function, NULL, string, 0, 0, NULL, NULL, NULL, NULL, &tsfn);
  assert(e == napi_invalid_arg);

  e = napi_fatal_exception(env, NULL);
  assert(e == napi_invalid_arg);

  napi_async_work work;
  e = napi_create_async_work(env, NULL, string, NULL, NULL, NULL, &work);
  assert(e == napi_invalid_arg);

  e = napi_create_async_work(env, NULL, string, on_execute, NULL, NULL, &work);
  assert(e == napi_ok);

  e = napi_delete_async_work(env, work);
  assert(e == napi_ok);

  napi_teardown_env();
}
