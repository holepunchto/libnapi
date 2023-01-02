#ifndef NAPI_MODULES_H
#define NAPI_MODULES_H

#include "../napi.h"

#define NAPI_MODULE_VERSION 1

#define NAPI_MODULE(name, fn) \
  static void module_initializer(void) __attribute__((constructor)); \
  static void module_initializer(void) { \
    napi_module module = { \
      NAPI_MODULE_VERSION, \
      0, \
      __FILE__, \
      fn, \
      #name, \
      NULL, \
      {0}, \
    }; \
    napi_module_register(&module); \
  }

typedef napi_value (*napi_addon_register_func)(napi_env env, napi_value exports);

typedef struct napi_module {
  int nm_version;
  unsigned int nm_flags;
  const char *nm_filename;
  napi_addon_register_func nm_register_func;
  const char *nm_modname;
  void *nm_priv;
  void *reserved[4];
} napi_module;

void
napi_module_register (napi_module *mod);

#endif // NAPI_MODULES_H
