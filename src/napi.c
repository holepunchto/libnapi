#include "../include/napi.h"

extern napi_status
napi_open_handle_scope (napi_env env, napi_handle_scope *result);

extern napi_status
napi_close_handle_scope (napi_env env, napi_handle_scope scope);

extern napi_status
napi_open_escapable_handle_scope (napi_env env, napi_escapable_handle_scope *result);

extern napi_status
napi_close_escapable_handle_scope (napi_env env, napi_escapable_handle_scope scope);

extern napi_status
napi_escape_handle (napi_env env, napi_escapable_handle_scope scope, napi_value escapee, napi_value *result);

extern napi_status
napi_create_reference (napi_env env, napi_value value, uint32_t initial_refcount, napi_ref *result);

extern napi_status
napi_delete_reference (napi_env env, napi_ref ref);

extern napi_status
napi_reference_ref (napi_env env, napi_ref ref, uint32_t *result);

extern napi_status
napi_reference_unref (napi_env env, napi_ref ref, uint32_t *result);

extern napi_status
napi_get_reference_value (napi_env env, napi_ref ref, napi_value *result);

extern napi_status
napi_throw (napi_env env, napi_value error);
