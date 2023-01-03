# libnapi

Node-API compatibility layer for https://github.com/holepunchto/libjs. It maintains ABI compatibility with a subset of the Node-API symbols, documented at https://nodejs.org/api/n-api.html; support for the entire set of Node-API symbols is, at least for the present, a non-goal.

## Inlining

When compiling for a runtime where the Node-API compatibility layer is unnecessary, the compatibility layer may be inlined by defining `NAPI_INLINE` before including `napi.h`, after which the underlying engine calls will be made directly:

```c
#define NAPI_INLINE

#include <napi.h>
```

## Modules

Node-API module support is provided by the [`include/napi/modules.h`](include/napi/modules.h) header and depends on the runtime providing an implementation of the `napi_module_register()` function:

```c
#include <napi/modules.h>

void
napi_module_register (napi_module *mod) {
  // Do something with `mod`.
}
```

## API

See [`include/napi.h`](include/napi.h) for the public API.

## License

ISC
