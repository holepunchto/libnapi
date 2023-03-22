#ifndef NAPI_MODULES_H
#define NAPI_MODULES_H

#include "../napi.h"

// https://stackoverflow.com/a/2390626

#if defined(__cplusplus)
#define NAPI_INITIALIZER(f) \
  static void f(void); \
  struct f##_ { \
    f##_(void) { f(); } \
  } f##_; \
  static void f(void)
#elif defined(_MSC_VER)
#pragma section(".CRT$XCU", read)
#define NAPI_INITIALIZER(f) \
  static void f(void); \
  __declspec(dllexport, allocate(".CRT$XCU")) void (*f##_)(void) = f; \
  static void f(void)
#else
#define NAPI_INITIALIZER(f) \
  static void f(void) __attribute__((constructor)); \
  static void f(void)
#endif

#ifndef NAPI_MODULE_VERSION
#define NAPI_MODULE_VERSION 1
#endif

#ifndef NAPI_MODULE_FILENAME
#define NAPI_MODULE_FILENAME ""
#endif

#define NAPI_MODULE_NAME(name) #name

#define NAPI_MODULE(name, fn) \
  NAPI_INITIALIZER(module_initializer) { \
    napi_module module = { \
      NAPI_MODULE_VERSION, \
      0, \
      NAPI_MODULE_FILENAME, \
      fn, \
      NAPI_MODULE_NAME(name), \
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

extern void
napi_module_register (napi_module *mod);

#endif // NAPI_MODULES_H
