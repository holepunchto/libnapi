#include <assert.h>

#include "../include/napi.h"
#include "helpers.h"

int
main() {
  int e;

  napi_env env = napi_setup_env();

  napi_value object;
  e = napi_create_object(env, &object);
  assert(e == 0);

  napi_value one;
  e = napi_create_int32(env, 1, &one);
  assert(e == 0);

  e = napi_set_named_property(env, object, "foo", one);
  assert(e == 0);

  e = napi_object_freeze(env, object);
  assert(e == 0);

  // A frozen object's existing properties are not writable.
  napi_value two;
  e = napi_create_int32(env, 2, &two);
  assert(e == 0);

  e = napi_set_named_property(env, object, "foo", two);
  assert(e == 0);

  napi_value foo;
  e = napi_get_named_property(env, object, "foo", &foo);
  assert(e == 0);

  int32_t value;
  e = napi_get_value_int32(env, foo, &value);
  assert(e == 0);

  assert(value == 1);

  // A frozen object cannot be extended with new properties.
  e = napi_set_named_property(env, object, "bar", one);
  assert(e == 0);

  bool has;
  e = napi_has_named_property(env, object, "bar", &has);
  assert(e == 0);

  assert(!has);

  napi_teardown_env();
}
