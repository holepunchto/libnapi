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
napi_run_script (napi_env env, napi_value source, napi_value *result);

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
napi_create_int32 (napi_env env, int32_t value, napi_value *result);

extern napi_status
napi_create_uint32 (napi_env env, uint32_t value, napi_value *result);

extern napi_status
napi_create_string_utf8 (napi_env env, const char *str, size_t len, napi_value *result);

extern napi_status
napi_create_function (napi_env env, const char *name, size_t len, napi_callback cb, void *data, napi_value *result);

extern napi_status
napi_get_global (napi_env env, napi_value *result);

extern napi_status
napi_get_null (napi_env env, napi_value *result);

extern napi_status
napi_get_undefined (napi_env env, napi_value *result);

extern napi_status
napi_get_boolean (napi_env env, bool value, napi_value *result);

extern napi_status
napi_get_value_int32 (napi_env env, napi_value value, int32_t *result);

extern napi_status
napi_get_value_uint32 (napi_env env, napi_value value, uint32_t *result);

extern napi_status
napi_get_value_string_utf8 (napi_env env, napi_value value, char *str, size_t len, size_t *result);

extern napi_status
napi_call_function (napi_env env, napi_value recv, napi_value fn, size_t argc, const napi_value argv[], napi_value *result);

extern napi_status
napi_throw (napi_env env, napi_value error);
