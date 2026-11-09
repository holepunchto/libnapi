#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "../include/napi.h"
#include "helpers.h"

int
main() {
  int e;

  napi_env env = napi_setup_env();

  napi_value number;
  e = napi_create_int32(env, 42, &number);
  assert(e == 0);

  napi_value bigint;
  e = napi_create_bigint_int64(env, 42, &bigint);
  assert(e == 0);

  napi_value string;
  e = napi_create_string_utf8(env, "hello", -1, &string);
  assert(e == 0);

  napi_value boolean;
  e = napi_get_boolean(env, true, &boolean);
  assert(e == 0);

  napi_value object;
  e = napi_create_object(env, &object);
  assert(e == 0);

  napi_value null;
  e = napi_get_null(env, &null);
  assert(e == 0);

  napi_value undefined;
  e = napi_get_undefined(env, &undefined);
  assert(e == 0);

  int data;

  napi_value external;
  e = napi_create_external(env, &data, NULL, NULL, &external);
  assert(e == 0);

  enum { NUMBER, BIGINT, STRING, BOOLEAN, OBJECT, NULL_, UNDEFINED, EXTERNAL };

  napi_value values[] = {number, bigint, string, boolean, object, null, undefined, external};

  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
    napi_value value = values[i];

    bool b;
    e = napi_get_value_bool(env, value, &b);
    assert(e == (i == BOOLEAN ? napi_ok : napi_boolean_expected));

    int32_t i32;
    e = napi_get_value_int32(env, value, &i32);
    assert(e == (i == NUMBER ? napi_ok : napi_number_expected));

    uint32_t u32;
    e = napi_get_value_uint32(env, value, &u32);
    assert(e == (i == NUMBER ? napi_ok : napi_number_expected));

    int64_t i64;
    e = napi_get_value_int64(env, value, &i64);
    assert(e == (i == NUMBER ? napi_ok : napi_number_expected));

    double d;
    e = napi_get_value_double(env, value, &d);
    assert(e == (i == NUMBER ? napi_ok : napi_number_expected));

    bool lossless;
    e = napi_get_value_bigint_int64(env, value, &i64, &lossless);
    assert(e == (i == BIGINT ? napi_ok : napi_bigint_expected));

    uint64_t u64;
    e = napi_get_value_bigint_uint64(env, value, &u64, &lossless);
    assert(e == (i == BIGINT ? napi_ok : napi_bigint_expected));

    size_t word_count = 0;
    e = napi_get_value_bigint_words(env, value, NULL, &word_count, NULL);
    assert(e == (i == BIGINT ? napi_ok : napi_bigint_expected));

    size_t len;
    e = napi_get_value_string_utf8(env, value, NULL, 0, &len);
    assert(e == (i == STRING ? napi_ok : napi_string_expected));

    e = napi_get_value_string_utf16(env, value, NULL, 0, &len);
    assert(e == (i == STRING ? napi_ok : napi_string_expected));

    e = napi_get_value_string_latin1(env, value, NULL, 0, &len);
    assert(e == (i == STRING ? napi_ok : napi_string_expected));

    void *ptr;
    e = napi_get_value_external(env, value, &ptr);
    assert(e == (i == EXTERNAL ? napi_ok : napi_invalid_arg));
    if (i == EXTERNAL) assert(ptr == &data);
  }

  int32_t i32;
  e = napi_get_value_int32(env, NULL, &i32);
  assert(e == napi_invalid_arg);

  napi_teardown_env();
}
