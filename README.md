# libnapi

Node-API compatibility layer for https://github.com/holepunchto/libjs.

## Inlining

When compiling for a runtime where the Node-API compatibility layer is unnecessary, the compatibility layer may be inlined by defining `NAPI_INLINE` before including `napi.h`, after which the underlying engine calls will be made direcly:

```c
#define NAPI_INLINE

#include <napi.h>
```

## API

See [`includes/napi.h`](include/napi.h) for the public API.

## License

ISC
