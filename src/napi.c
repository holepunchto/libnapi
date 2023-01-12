#include "../include/napi.h"

extern js_value_type_t
napi_convert_from_valuetype (napi_valuetype type);

extern napi_valuetype
napi_convert_to_valuetype (js_value_type_t type);

extern js_typedarray_type_t
napi_convert_from_typedarray_type (napi_typedarray_type type);

extern napi_typedarray_type
napi_convert_to_typedarray_type (js_typedarray_type_t type);

extern napi_status
napi_get_uv_event_loop (napi_env env, uv_loop_t **loop);

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
napi_wrap (napi_env env, napi_value object, void *data, napi_finalize finalize_cb, void *finalize_hint, napi_ref *result);

extern napi_status
napi_unwrap (napi_env env, napi_value object, void **result);

extern napi_status
napi_remove_wrap (napi_env env, napi_value object, void **result);

extern napi_status
napi_add_finalizer (napi_env env, napi_value object, void *data, napi_finalize finalize_cb, void *finalize_hint, napi_ref *result);

extern napi_status
napi_create_int32 (napi_env env, int32_t value, napi_value *result);

extern napi_status
napi_create_uint32 (napi_env env, uint32_t value, napi_value *result);

extern napi_status
napi_create_int64 (napi_env env, int64_t value, napi_value *result);

extern napi_status
napi_create_double (napi_env env, double value, napi_value *result);

extern napi_status
napi_create_bigint_int64 (napi_env env, int64_t value, napi_value *result);

extern napi_status
napi_create_bigint_uint64 (napi_env env, uint64_t value, napi_value *result);

extern napi_status
napi_create_string_utf8 (napi_env env, const char *str, size_t len, napi_value *result);

extern napi_status
napi_create_symbol (napi_env env, napi_value description, napi_value *result);

extern napi_status
napi_create_object (napi_env env, napi_value *result);

extern napi_status
napi_create_function (napi_env env, const char *name, size_t len, napi_callback cb, void *data, napi_value *result);

extern napi_status
napi_create_array (napi_env env, napi_value *result);

extern napi_status
napi_create_array_with_length (napi_env env, size_t len, napi_value *result);

extern napi_status
napi_create_external (napi_env env, void *data, napi_finalize finalize_cb, void *finalize_hint, napi_value *result);

extern napi_status
napi_create_date (napi_env env, double time, napi_value *result);

extern napi_status
napi_create_error (napi_env env, napi_value code, napi_value message, napi_value *result);

extern napi_status
napi_create_type_error (napi_env env, napi_value code, napi_value message, napi_value *result);

extern napi_status
napi_create_range_error (napi_env env, napi_value code, napi_value message, napi_value *result);

extern napi_status
napi_create_promise (napi_env env, napi_deferred *deferred, napi_value *promise);

extern napi_status
napi_resolve_deferred (napi_env env, napi_deferred deferred, napi_value resolution);

extern napi_status
napi_reject_deferred (napi_env env, napi_deferred deferred, napi_value resolution);

extern napi_status
napi_create_arraybuffer (napi_env env, size_t len, void **data, napi_value *result);

extern napi_status
napi_create_external_arraybuffer (napi_env env, void *data, size_t len, napi_finalize finalize_cb, void *finalize_hint, napi_value *result);

extern napi_status
napi_detach_arraybuffer (napi_env env, napi_value arraybuffer);

extern napi_status
napi_create_typedarray (napi_env env, napi_typedarray_type type, size_t len, napi_value arraybuffer, size_t offset, napi_value *result);

extern napi_status
napi_create_buffer (napi_env env, size_t len, void **data, napi_value *result);

extern napi_status
napi_create_buffer_copy (napi_env env, size_t len, const void *data, void **result_data, napi_value *result);

extern napi_status
napi_create_external_buffer (napi_env env, void *data, size_t len, napi_finalize finalize_cb, void *finalize_hint, napi_value *result);

extern napi_status
napi_create_dataview (napi_env env, size_t len, napi_value arraybuffer, size_t offset, napi_value *result);

extern napi_status
napi_typeof (napi_env env, napi_value value, napi_valuetype *result);

extern napi_status
napi_is_array (napi_env env, napi_value value, bool *result);

extern napi_status
napi_is_date (napi_env env, napi_value value, bool *result);

extern napi_status
napi_is_error (napi_env env, napi_value value, bool *result);

extern napi_status
napi_is_promise (napi_env env, napi_value value, bool *result);

extern napi_status
napi_is_arraybuffer (napi_env env, napi_value value, bool *result);

extern napi_status
napi_is_detached_arraybuffer (napi_env env, napi_value value, bool *result);

extern napi_status
napi_is_typedarray (napi_env env, napi_value value, bool *result);

extern napi_status
napi_is_buffer (napi_env env, napi_value value, bool *result);

extern napi_status
napi_is_dataview (napi_env env, napi_value value, bool *result);

extern napi_status
napi_strict_equals (napi_env env, napi_value a, napi_value b, bool *result);

extern napi_status
napi_get_global (napi_env env, napi_value *result);

extern napi_status
napi_get_undefined (napi_env env, napi_value *result);

extern napi_status
napi_get_null (napi_env env, napi_value *result);

extern napi_status
napi_get_boolean (napi_env env, bool value, napi_value *result);

extern napi_status
napi_get_value_bool (napi_env env, napi_value value, bool *result);

extern napi_status
napi_get_value_int32 (napi_env env, napi_value value, int32_t *result);

extern napi_status
napi_get_value_uint32 (napi_env env, napi_value value, uint32_t *result);

extern napi_status
napi_get_value_int64 (napi_env env, napi_value value, int64_t *result);

extern napi_status
napi_get_value_double (napi_env env, napi_value value, double *result);

extern napi_status
napi_get_value_bigint_int64 (napi_env env, napi_value value, int64_t *result);

extern napi_status
napi_get_value_bigint_uint64 (napi_env env, napi_value value, uint64_t *result);

extern napi_status
napi_get_value_string_utf8 (napi_env env, napi_value value, char *str, size_t len, size_t *result);

extern napi_status
napi_get_value_external (napi_env env, napi_value value, void **result);

extern napi_status
napi_get_value_date (napi_env env, napi_value value, double *result);

extern napi_status
napi_get_array_length (napi_env env, napi_value value, uint32_t *result);

extern napi_status
napi_get_property (napi_env env, napi_value object, napi_value key, napi_value *result);

extern napi_status
napi_has_property (napi_env env, napi_value object, napi_value key, bool *result);

extern napi_status
napi_set_property (napi_env env, napi_value object, napi_value key, napi_value value);

extern napi_status
napi_delete_property (napi_env env, napi_value object, napi_value key, bool *result);

extern napi_status
napi_get_named_property (napi_env env, napi_value object, const char *name, napi_value *result);

extern napi_status
napi_has_named_property (napi_env env, napi_value object, const char *name, bool *result);

extern napi_status
napi_set_named_property (napi_env env, napi_value object, const char *name, napi_value value);

extern napi_status
napi_get_element (napi_env env, napi_value object, uint32_t index, napi_value *result);

extern napi_status
napi_has_element (napi_env env, napi_value object, uint32_t index, bool *result);

extern napi_status
napi_set_element (napi_env env, napi_value object, uint32_t index, napi_value value);

extern napi_status
napi_delete_element (napi_env env, napi_value object, uint32_t index, bool *result);

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
napi_call_function (napi_env env, napi_value recv, napi_value fn, size_t argc, const napi_value argv[], napi_value *result);

extern napi_status
napi_make_callback (napi_env env, napi_async_context async_hook, napi_value recv, napi_value fn, size_t argc, const napi_value argv[], napi_value *result);

extern napi_status
napi_throw (napi_env env, napi_value error);

extern napi_status
napi_throw_error (napi_env env, const char *code, const char *msg);

extern napi_status
napi_throw_type_error (napi_env env, const char *code, const char *message);

extern napi_status
napi_throw_range_error (napi_env env, const char *code, const char *message);

extern napi_status
napi_is_exception_pending (napi_env env, bool *result);

extern napi_status
napi_get_and_clear_last_exception (napi_env env, napi_value *result);

extern napi_status
napi_fatal_exception (napi_env env, napi_value err);

extern napi_status
napi_adjust_external_memory (napi_env env, int64_t change_in_bytes, int64_t *result);
