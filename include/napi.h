#ifndef NAPI_H
#define NAPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <js.h>

typedef js_env_t *napi_env;
typedef js_value_t *napi_value;
typedef js_handle_scope_t *napi_handle_scope;
typedef js_escapable_handle_scope_t *napi_escapable_handle_scope;
typedef js_ref_t *napi_ref;
typedef js_callback_info_t *napi_callback_info;

typedef napi_value (*napi_callback)(napi_env, napi_callback_info);

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

napi_status
napi_open_handle_scope (napi_env env, napi_handle_scope *result);

napi_status
napi_close_handle_scope (napi_env env, napi_handle_scope scope);

napi_status
napi_open_escapable_handle_scope (napi_env env, napi_escapable_handle_scope *result);

napi_status
napi_close_escapable_handle_scope (napi_env env, napi_escapable_handle_scope scope);

napi_status
napi_escape_handle (napi_env env, napi_escapable_handle_scope scope, napi_value escapee, napi_value *result);

napi_status
napi_create_reference (napi_env env, napi_value value, uint32_t initial_refcount, napi_ref *result);

napi_status
napi_delete_reference (napi_env env, napi_ref ref);

napi_status
napi_reference_ref (napi_env env, napi_ref ref, uint32_t *result);

napi_status
napi_reference_unref (napi_env env, napi_ref ref, uint32_t *result);

napi_status
napi_get_reference_value (napi_env env, napi_ref ref, napi_value *result);

napi_status
napi_throw (napi_env env, napi_value error);

#ifdef __cplusplus
}
#endif

#endif // NAPI_H
