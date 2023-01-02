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
napi_create_object (napi_env env, napi_value *result);

extern napi_status
napi_create_function (napi_env env, const char *name, size_t len, napi_callback cb, void *data, napi_value *result);

extern napi_status
napi_create_external (napi_env env, void *data, napi_finalize finalize_cb, void *finalize_hint, napi_value *result);

extern napi_status
napi_create_promise (napi_env env, napi_deferred *deferred, napi_value *promise);

extern napi_status
napi_resolve_deferred (napi_env env, napi_deferred deferred, napi_value resolution);

extern napi_status
napi_reject_deferred (napi_env env, napi_deferred deferred, napi_value resolution);

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
napi_get_value_external (napi_env env, napi_value value, void **result);

extern napi_status
napi_get_named_property (napi_env env, napi_value object, const char *name, napi_value *result);

extern napi_status
napi_set_named_property (napi_env env, napi_value object, const char *name, napi_value value);

extern napi_status
napi_call_function (napi_env env, napi_value recv, napi_value fn, size_t argc, const napi_value argv[], napi_value *result);

extern napi_status
napi_make_callback (napi_env env, napi_async_context async_hook, napi_value recv, napi_value fn, size_t argc, const napi_value argv[], napi_value *result);

extern napi_status
napi_get_cb_info (napi_env env, napi_callback_info info, size_t *argc, napi_value argv[], napi_value *self, void **data);

extern napi_status
napi_get_arraybuffer_info (napi_env env, napi_value arraybuffer, void **data, size_t *len);

extern napi_status
napi_get_typedarray_info (napi_env env, napi_value typedarray, napi_typedarray_type *type, size_t *len, void **data, napi_value *arraybuffer, size_t *offset);

extern napi_status
napi_get_buffer_info (napi_env env, napi_value buffer, void **data, size_t *len);

extern napi_status
napi_get_dataview_info (napi_env env, napi_value dataview, size_t *len, void **data, napi_value *arraybuffer, size_t *offset);

extern napi_status
napi_throw (napi_env env, napi_value error);

extern napi_status
napi_throw_error (napi_env env, const char *code, const char *msg);

extern napi_status
napi_get_uv_event_loop (napi_env env, uv_loop_t **loop);

extern napi_status
napi_get_and_clear_last_exception (napi_env env, napi_value *result);

extern napi_status
napi_fatal_exception (napi_env env, napi_value err);
