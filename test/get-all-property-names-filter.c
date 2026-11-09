#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../include/napi.h"
#include "helpers.h"

int
main() {
  int e;

  napi_env env = napi_setup_env();

  napi_value object;
  e = napi_create_object(env, &object);
  assert(e == 0);

  napi_value value;
  e = napi_create_int32(env, 42, &value);
  assert(e == 0);

  e = napi_set_named_property(env, object, "enumerable", value);
  assert(e == 0);

  napi_value symbol;
  e = napi_create_symbol(env, NULL, &symbol);
  assert(e == 0);

  e = napi_set_property(env, object, symbol, value);
  assert(e == 0);

  napi_property_descriptor hidden = {"hidden", NULL, NULL, NULL, NULL, value, napi_default, NULL};
  e = napi_define_properties(env, object, 1, &hidden);
  assert(e == 0);

  napi_value names;
  e = napi_get_all_property_names(env, object, napi_key_own_only, napi_key_enumerable | napi_key_skip_symbols, napi_key_keep_numbers, &names);
  assert(e == napi_ok);

  uint32_t len;
  e = napi_get_array_length(env, names, &len);
  assert(e == napi_ok);
  assert(len == 1);

  napi_value name;
  e = napi_get_element(env, names, 0, &name);
  assert(e == napi_ok);

  char buf[16];
  e = napi_get_value_string_utf8(env, name, buf, sizeof(buf), NULL);
  assert(e == napi_ok);
  assert(strcmp(buf, "enumerable") == 0);

  e = napi_get_all_property_names(env, object, (napi_key_collection_mode) 2, napi_key_all_properties, napi_key_keep_numbers, &names);
  assert(e == napi_invalid_arg);

  e = napi_get_all_property_names(env, object, napi_key_own_only, (napi_key_filter) (1 << 5), napi_key_keep_numbers, &names);
  assert(e == napi_invalid_arg);

  e = napi_get_all_property_names(env, object, napi_key_own_only, napi_key_all_properties, (napi_key_conversion) 2, &names);
  assert(e == napi_invalid_arg);

  napi_teardown_env();
}
