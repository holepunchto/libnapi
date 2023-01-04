#ifndef NAPI_H
#define NAPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <js.h>

#ifdef NAPI_INLINE
#define NAPI_INLINABLE static inline
#else
#define NAPI_INLINABLE
#endif

typedef js_env_t *napi_env;
typedef js_value_t *napi_value;
typedef js_handle_scope_t *napi_handle_scope;
typedef js_escapable_handle_scope_t *napi_escapable_handle_scope;
typedef js_ref_t *napi_ref;
typedef js_deferred_t *napi_deferred;
typedef js_callback_info_t *napi_callback_info;
typedef void *napi_async_context;

typedef napi_value (*napi_callback)(napi_env, napi_callback_info);
typedef void (*napi_finalize)(napi_env env, void *finalize_data, void *finalize_hint);

#include "napi/modules.h"

typedef enum {
  napi_ok,
  napi_invalid_arg,
  napi_object_expected,
  napi_string_expected,
  napi_name_expected,
  napi_function_expected,
  napi_number_expected,
  napi_boolean_expected,
  napi_array_expected,
  napi_generic_failure,
  napi_pending_exception,
  napi_cancelled,
  napi_escape_called_twice,
  napi_handle_scope_mismatch,
  napi_callback_scope_mismatch,
  napi_queue_full,
  napi_closing,
  napi_bigint_expected,
  napi_date_expected,
  napi_arraybuffer_expected,
  napi_detachable_arraybuffer_expected,
  napi_would_deadlock,
  napi_no_external_buffers_allowed
} napi_status;

typedef enum {
  napi_int8_array,
  napi_uint8_array,
  napi_uint8_clamped_array,
  napi_int16_array,
  napi_uint16_array,
  napi_int32_array,
  napi_uint32_array,
  napi_float32_array,
  napi_float64_array,
  napi_bigint64_array,
  napi_biguint64_array,
} napi_typedarray_type;

NAPI_INLINABLE napi_status
napi_get_uv_event_loop (napi_env env, uv_loop_t **loop) {
  int err = js_get_env_loop(env, loop);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_open_handle_scope (napi_env env, napi_handle_scope *result) {
  int err = js_open_handle_scope(env, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_close_handle_scope (napi_env env, napi_handle_scope scope) {
  int err = js_close_handle_scope(env, scope);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_open_escapable_handle_scope (napi_env env, napi_escapable_handle_scope *result) {
  int err = js_open_escapable_handle_scope(env, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_close_escapable_handle_scope (napi_env env, napi_escapable_handle_scope scope) {
  int err = js_close_escapable_handle_scope(env, scope);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_escape_handle (napi_env env, napi_escapable_handle_scope scope, napi_value escapee, napi_value *result) {
  int err = js_escape_handle(env, scope, escapee, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_run_script (napi_env env, napi_value source, napi_value *result) {
  int err = js_run_script(env, source, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_create_reference (napi_env env, napi_value value, uint32_t initial_refcount, napi_ref *result) {
  int err = js_create_reference(env, value, initial_refcount, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_delete_reference (napi_env env, napi_ref ref) {
  int err = js_delete_reference(env, ref);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_reference_ref (napi_env env, napi_ref ref, uint32_t *result) {
  int err = js_reference_ref(env, ref, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_reference_unref (napi_env env, napi_ref ref, uint32_t *result) {
  int err = js_reference_unref(env, ref, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_get_reference_value (napi_env env, napi_ref ref, napi_value *result) {
  int err = js_get_reference_value(env, ref, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_create_int32 (napi_env env, int32_t value, napi_value *result) {
  int err = js_create_int32(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_create_uint32 (napi_env env, uint32_t value, napi_value *result) {
  int err = js_create_uint32(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_create_string_utf8 (napi_env env, const char *str, size_t len, napi_value *result) {
  int err = js_create_string_utf8(env, str, len, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_create_object (napi_env env, napi_value *result) {
  int err = js_create_object(env, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_create_function (napi_env env, const char *name, size_t len, napi_callback cb, void *data, napi_value *result) {
  int err = js_create_function(env, name, len, cb, data, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_create_external (napi_env env, void *data, napi_finalize finalize_cb, void *finalize_hint, napi_value *result) {
  int err = js_create_external(env, data, finalize_cb, finalize_hint, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_create_promise (napi_env env, napi_deferred *deferred, napi_value *promise) {
  int err = js_create_promise(env, deferred, promise);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_resolve_deferred (napi_env env, napi_deferred deferred, napi_value resolution) {
  int err = js_resolve_deferred(env, deferred, resolution);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_reject_deferred (napi_env env, napi_deferred deferred, napi_value resolution) {
  int err = js_reject_deferred(env, deferred, resolution);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_get_global (napi_env env, napi_value *result) {
  int err = js_get_global(env, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_get_null (napi_env env, napi_value *result) {
  int err = js_get_null(env, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_get_undefined (napi_env env, napi_value *result) {
  int err = js_get_undefined(env, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_get_boolean (napi_env env, bool value, napi_value *result) {
  int err = js_get_boolean(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_get_value_int32 (napi_env env, napi_value value, int32_t *result) {
  int err = js_get_value_int32(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_get_value_uint32 (napi_env env, napi_value value, uint32_t *result) {
  int err = js_get_value_uint32(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_get_value_string_utf8 (napi_env env, napi_value value, char *str, size_t len, size_t *result) {
  int err = js_get_value_string_utf8(env, value, str, len, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_get_value_external (napi_env env, napi_value value, void **result) {
  int err = js_get_value_external(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_get_named_property (napi_env env, napi_value object, const char *name, napi_value *result) {
  int err = js_get_named_property(env, object, name, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_set_named_property (napi_env env, napi_value object, const char *name, napi_value value) {
  int err = js_set_named_property(env, object, name, value);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_call_function (napi_env env, napi_value recv, napi_value fn, size_t argc, const napi_value argv[], napi_value *result) {
  int err = js_call_function(env, recv, fn, argc, argv, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_make_callback (napi_env env, napi_async_context async_hook, napi_value recv, napi_value fn, size_t argc, const napi_value argv[], napi_value *result) {
  int err = js_make_callback(env, recv, fn, argc, argv, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_get_cb_info (napi_env env, napi_callback_info info, size_t *argc, napi_value argv[], napi_value *self, void **data) {
  int err = js_get_callback_info(env, info, argc, argv, self, data);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_get_arraybuffer_info (napi_env env, napi_value arraybuffer, void **data, size_t *len) {
  int err = js_get_arraybuffer_info(env, arraybuffer, data, len);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_get_typedarray_info (napi_env env, napi_value typedarray, napi_typedarray_type *type, size_t *len, void **data, napi_value *arraybuffer, size_t *offset) {
  int err = js_get_typedarray_info(env, typedarray, (js_typedarray_type_t *) type, data, len, arraybuffer, offset);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_get_buffer_info (napi_env env, napi_value buffer, void **data, size_t *len) {
  int err = js_get_typedarray_info(env, buffer, NULL, data, len, NULL, NULL);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_get_dataview_info (napi_env env, napi_value dataview, size_t *len, void **data, napi_value *arraybuffer, size_t *offset) {
  int err = js_get_dataview_info(env, dataview, data, len, arraybuffer, offset);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_throw (napi_env env, napi_value error) {
  int err = js_throw(env, error);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_throw_error (napi_env env, const char *code, const char *message) {
  int err = js_throw_error(env, code, message);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_get_and_clear_last_exception (napi_env env, napi_value *result) {
  int err = js_get_and_clear_last_exception(env, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

NAPI_INLINABLE napi_status
napi_fatal_exception (napi_env env, napi_value error) {
  int err = js_fatal_exception(env, error);
  return err == 0 ? napi_ok : napi_pending_exception;
}

#ifdef __cplusplus
}
#endif

#endif // NAPI_H
