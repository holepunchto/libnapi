#ifndef NAPI_MODULE_H
#define NAPI_MODULE_H

#include "../napi.h"

#define NAPI_MODULE_VERSION 1

#ifndef NAPI_MODULE_FILENAME
#define NAPI_MODULE_FILENAME ""
#endif

#define NAPI_MODULE_CONCAT(a, b) a##b

#define NAPI_MODULE_NAME(name) #name

#ifdef __cplusplus
#define NAPI_MODULE_EXTERN_C_START extern "C" {
#define NAPI_MODULE_EXTERN_C_END   }
#else
#define NAPI_MODULE_EXTERN_C_START
#define NAPI_MODULE_EXTERN_C_END
#endif

#define NAPI_MODULE_SYMBOL_HELPER(base, version) NAPI_MODULE_CONCAT(base, version)

#define NAPI_MODULE_SYMBOL_REGISTER_BASE napi_register_module_v

#define NAPI_MODULE_SYMBOL_REGISTER \
  NAPI_MODULE_SYMBOL_HELPER(NAPI_MODULE_SYMBOL_REGISTER_BASE, NAPI_MODULE_VERSION)

// https://stackoverflow.com/a/2390626

#if defined(_MSC_VER)
#pragma section(".CRT$XCU", read)
#define NAPI_MODULE_CONSTRUCTOR_BASE(name, version) \
  __pragma(comment(linker, "/include:napi_register_module_" #name "_" #version "_")); \
  static void napi_register_module_##name(void); \
  __declspec(dllexport, allocate(".CRT$XCU")) void (*napi_register_module_##name##_##version##_)(void) = napi_register_module_##name; \
  static void napi_register_module_##name(void)
#else
#define NAPI_MODULE_CONSTRUCTOR_BASE(name, version) \
  static void napi_register_module_##name(void) __attribute__((constructor)); \
  static void napi_register_module_##name(void)
#endif

#define NAPI_MODULE_CONSTRUCTOR(name, version) NAPI_MODULE_CONSTRUCTOR_BASE(name, version)

#ifdef NAPI_MODULE_REGISTER_CONSTRUCTOR

#define NAPI_MODULE(name, fn) \
  NAPI_MODULE_EXTERN_C_START \
  NAPI_MODULE_CONSTRUCTOR(name, NAPI_MODULE_CONSTRUCTOR_VERSION) { \
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
  } \
  NAPI_MODULE_EXTERN_C_END

#else

#define NAPI_MODULE(name, fn) \
  NAPI_MODULE_EXTERN_C_START \
  napi_value NAPI_MODULE_SYMBOL_REGISTER(napi_env env, napi_value exports) { \
    return fn(env, exports); \
  } \
  NAPI_MODULE_EXTERN_C_END

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
napi_module_register(napi_module *mod);

#endif // NAPI_MODULE_H
