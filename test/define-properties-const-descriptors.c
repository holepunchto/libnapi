#include <assert.h>
#include <stdbool.h>

#include "../include/napi.h"
#include "helpers.h"

static napi_value
on_call(napi_env env, napi_callback_info info) {
  return NULL;
}

static const napi_property_descriptor properties[] = {
  {"method", NULL, on_call, NULL, NULL, NULL, napi_default, NULL},
};

int
main() {
  int e;

  napi_env env = napi_setup_env();

  for (int i = 0; i < 2; i++) {
    napi_handle_scope scope;
    e = napi_open_handle_scope(env, &scope);
    assert(e == napi_ok);

    napi_value object;
    e = napi_create_object(env, &object);
    assert(e == napi_ok);

    e = napi_define_properties(env, object, 1, properties);
    assert(e == napi_ok);

    bool has;
    e = napi_has_named_property(env, object, "method", &has);
    assert(e == napi_ok);
    assert(has);

    napi_value constructor;
    e = napi_define_class(env, "Class", NAPI_AUTO_LENGTH, on_call, NULL, 1, properties, &constructor);
    assert(e == napi_ok);

    e = napi_close_handle_scope(env, scope);
    assert(e == napi_ok);
  }

  napi_teardown_env();
}
