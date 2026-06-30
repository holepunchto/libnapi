#include <assert.h>

#include "../include/napi.h"
#include "helpers.h"

int
main() {
  int e;

  napi_env env = napi_setup_env();

  uint32_t version;
  e = napi_get_version(env, &version);
  assert(e == 0);

  // The default version reported when libnapi is built without an override.
  assert(version == 10);

  napi_teardown_env();
}
