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

  napi_value object;
  e = napi_create_object(env, &object);
  assert(e == 0);

  napi_value array;
  e = napi_create_array(env, &array);
  assert(e == 0);

  napi_value date;
  e = napi_create_date(env, 0, &date);
  assert(e == 0);

  napi_value arraybuffer;
  e = napi_create_arraybuffer(env, 4, NULL, &arraybuffer);
  assert(e == 0);

  napi_value sharedarraybuffer;
  e = node_api_create_sharedarraybuffer(env, 4, NULL, &sharedarraybuffer);
  assert(e == 0);

  napi_value backing;
  e = napi_create_arraybuffer(env, 4, NULL, &backing);
  assert(e == 0);

  napi_value typedarray;
  e = napi_create_typedarray(env, napi_uint8_array, 4, backing, 0, &typedarray);
  assert(e == 0);

  napi_value dataview;
  e = napi_create_dataview(env, 4, backing, 0, &dataview);
  assert(e == 0);

  enum { NUMBER, OBJECT, ARRAY, DATE, ARRAYBUFFER, SHAREDARRAYBUFFER, TYPEDARRAY, DATAVIEW };

  napi_value values[] = {number, object, array, date, arraybuffer, sharedarraybuffer, typedarray, dataview};

  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
    napi_value value = values[i];

    double d;
    e = napi_get_date_value(env, value, &d);
    assert(e == (i == DATE ? napi_ok : napi_date_expected));

    uint32_t u32;
    e = napi_get_array_length(env, value, &u32);
    assert(e == (i == ARRAY ? napi_ok : napi_array_expected));

    size_t len;
    e = napi_get_arraybuffer_info(env, value, NULL, &len);
    assert(e == (i == ARRAYBUFFER || i == SHAREDARRAYBUFFER ? napi_ok : napi_invalid_arg));
    if (e == napi_ok) assert(len == 4);

    e = napi_get_typedarray_info(env, value, NULL, &len, NULL, NULL, NULL);
    assert(e == (i == TYPEDARRAY ? napi_ok : napi_invalid_arg));

    bool is_buffer;
    e = napi_is_buffer(env, value, &is_buffer);
    assert(e == napi_ok);
    assert(is_buffer == (i == TYPEDARRAY || i == DATAVIEW));

    e = napi_get_buffer_info(env, value, NULL, &len);
    assert(e == (i == TYPEDARRAY || i == DATAVIEW ? napi_ok : napi_invalid_arg));
    if (e == napi_ok) assert(len == 4);

    e = napi_get_dataview_info(env, value, &len, NULL, NULL, NULL);
    assert(e == (i == DATAVIEW ? napi_ok : napi_invalid_arg));

    bool is_arraybuffer = i == ARRAYBUFFER || i == SHAREDARRAYBUFFER;

    napi_value view;
    e = napi_create_typedarray(env, napi_uint8_array, 4, value, 0, &view);
    assert(e == (is_arraybuffer ? napi_ok : napi_invalid_arg));

    e = napi_create_dataview(env, 4, value, 0, &view);
    assert(e == (is_arraybuffer ? napi_ok : napi_invalid_arg));

    e = node_api_create_buffer_from_arraybuffer(env, value, 0, 4, &view);
    assert(e == (is_arraybuffer ? napi_ok : napi_arraybuffer_expected));

    e = napi_detach_arraybuffer(env, value);
    assert(e == (i == ARRAYBUFFER ? napi_ok : napi_arraybuffer_expected));
  }

  napi_value script;
  e = napi_create_string_utf8(env, "new WebAssembly.Memory({ initial: 1 }).buffer", -1, &script);
  assert(e == 0);

  napi_value memory;
  e = napi_run_script(env, script, &memory);
  assert(e == 0);

  e = napi_detach_arraybuffer(env, memory);
  assert(e == napi_detachable_arraybuffer_expected);

  napi_teardown_env();
}
