#ifndef NAPI_MODULE_H
#define NAPI_MODULE_H

#include "../napi.h"

#define NAPI_MODULE_VERSION 1

#ifndef NAPI_MODULE_FILENAME
#define NAPI_MODULE_FILENAME ""
#endif

#define NAPI_MODULE_CONCAT(a, b) a##b

#define NAPI_MODULE_NAME(name) #name

#define NAPI_MODULE_SYMBOL_HELPER(base, version) NAPI_MODULE_CONCAT(base, version)

#define NAPI_MODULE_SYMBOL_REGISTER_BASE napi_register_module_v

#define NAPI_MODULE_SYMBOL_REGISTER \
  NAPI_MODULE_SYMBOL_HELPER(NAPI_MODULE_SYMBOL_REGISTER_BASE, NAPI_MODULE_VERSION)

// https://stackoverflow.com/a/2390626

#if defined(__cplusplus)
#define NAPI_MODULE_CONSTRUCTOR(name) \
  static void napi_register_module_##name(void); \
  struct napi_register_module_##name##_ { \
    napi_register_module_##name##_(void) { napi_register_module_##name(); } \
  } napi_register_module_##name##_; \
  static void napi_register_module_##name(void)
#elif defined(_MSC_VER)
#pragma section(".CRT$XCU", read)
#define NAPI_MODULE_CONSTRUCTOR(name) \
  static void napi_register_module_##name(void); \
  __declspec(dllexport, allocate(".CRT$XCU")) void (*napi_register_module_##name##_)(void) = napi_register_module_##name; \
  __pragma(comment(linker, "/include:napi_register_module_" #name "_")) static void napi_register_module_##name(void)
#else
#define NAPI_MODULE_CONSTRUCTOR(name) \
  static void napi_register_module_##name(void) __attribute__((constructor)); \
  static void napi_register_module_##name(void)
#endif

#ifdef NAPI_MODULE_REGISTER_CONSTRUCTOR

#define NAPI_MODULE(name, fn) \
  NAPI_MODULE_CONSTRUCTOR(name) { \
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

#else

#define NAPI_MODULE(name, fn) \
  napi_value NAPI_MODULE_SYMBOL_REGISTER(napi_env env, napi_value exports) { \
    return fn(env, exports); \
  }

#endif

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

#endif // NAPI_MODULE_H
