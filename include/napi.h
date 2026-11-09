#ifndef NAPI_H
#define NAPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <js.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <utf.h>

#ifndef __cplusplus
typedef utf16_t char16_t;
#endif

#define NAPI_AUTO_LENGTH ((size_t) -1)

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
  napi_no_external_buffers_allowed,
  napi_cannot_run_js,
} napi_status;

typedef js_env_t *napi_env;
typedef js_value_t *napi_value;
typedef js_handle_scope_t *napi_handle_scope;
typedef js_escapable_handle_scope_t *napi_escapable_handle_scope;
typedef js_ref_t *napi_ref;
typedef js_deferred_t *napi_deferred;
typedef js_callback_info_t *napi_callback_info;
typedef js_threadsafe_function_t *napi_threadsafe_function;
typedef js_deferred_teardown_t *napi_async_cleanup_hook_handle;
typedef void *napi_async_context;
typedef void *napi_callback_scope;
typedef struct napi_async_work *napi_async_work;

typedef napi_value (*napi_callback)(napi_env, napi_callback_info);
typedef void (*napi_finalize)(napi_env, void *data, void *finalize_hint);
typedef void (*napi_threadsafe_function_call_js)(napi_env, napi_value function, void *context, void *data);
typedef void (*napi_cleanup_hook)(void *data);
typedef void (*napi_async_cleanup_hook)(napi_async_cleanup_hook_handle handle, void *data);
typedef void (*napi_async_execute_callback)(napi_env env, void *data);
typedef void (*napi_async_complete_callback)(napi_env env, napi_status status, void *data);

#include "napi/module.h"

typedef enum {
  napi_undefined,
  napi_null,
  napi_boolean,
  napi_number,
  napi_string,
  napi_symbol,
  napi_object,
  napi_function,
  napi_external,
  napi_bigint,
} napi_valuetype;

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
  napi_float16_array,
} napi_typedarray_type;

typedef enum {
  napi_default = 0,
  napi_writable = 1 << 0,
  napi_enumerable = 1 << 1,
  napi_configurable = 1 << 2,

  napi_static = 1 << 10,

  napi_default_method = napi_writable | napi_configurable,

  napi_default_jsproperty = napi_writable | napi_enumerable | napi_configurable,

} napi_property_attributes;

typedef enum {
  napi_key_include_prototypes,
  napi_key_own_only
} napi_key_collection_mode;

typedef enum {
  napi_key_all_properties = 0,
  napi_key_writable = 1,
  napi_key_enumerable = 1 << 1,
  napi_key_configurable = 1 << 2,
  napi_key_skip_strings = 1 << 3,
  napi_key_skip_symbols = 1 << 4
} napi_key_filter;

typedef enum {
  napi_key_keep_numbers,
  napi_key_numbers_to_strings
} napi_key_conversion;

typedef struct {
  const char *error_message;
  void *engine_reserved;
  uint32_t engine_error_code;
  napi_status error_code;
} napi_extended_error_info;

typedef struct {
  const char *utf8name;
  napi_value name;

  napi_callback method;
  napi_callback getter;
  napi_callback setter;
  napi_value value;

  napi_property_attributes attributes;
  void *data;
} napi_property_descriptor;

typedef struct {
  uint64_t lower;
  uint64_t upper;
} napi_type_tag;

typedef enum {
  napi_tsfn_release,
  napi_tsfn_abort
} napi_threadsafe_function_release_mode;

typedef enum {
  napi_tsfn_nonblocking,
  napi_tsfn_blocking
} napi_threadsafe_function_call_mode;

napi_status
napi_get_last_error_info(napi_env env, const napi_extended_error_info **result);

napi_status
napi_set_instance_data(napi_env env, void *data, napi_finalize finalize_cb, void *finalize_hint);

napi_status
napi_get_instance_data(napi_env env, void **result);

napi_status
napi_get_version(napi_env env, uint32_t *result);

napi_status
napi_get_uv_event_loop(napi_env env, uv_loop_t **loop);

napi_status
napi_open_handle_scope(napi_env env, napi_handle_scope *result);

napi_status
napi_close_handle_scope(napi_env env, napi_handle_scope scope);

napi_status
napi_open_escapable_handle_scope(napi_env env, napi_escapable_handle_scope *result);

napi_status
napi_close_escapable_handle_scope(napi_env env, napi_escapable_handle_scope scope);

napi_status
napi_escape_handle(napi_env env, napi_escapable_handle_scope scope, napi_value escapee, napi_value *result);

napi_status
napi_run_script(napi_env env, napi_value source, napi_value *result);

napi_status
napi_create_reference(napi_env env, napi_value value, uint32_t initial_refcount, napi_ref *result);

napi_status
napi_delete_reference(napi_env env, napi_ref ref);

napi_status
napi_reference_ref(napi_env env, napi_ref ref, uint32_t *result);

napi_status
napi_reference_unref(napi_env env, napi_ref ref, uint32_t *result);

napi_status
napi_get_reference_value(napi_env env, napi_ref ref, napi_value *result);

napi_status
napi_define_class(napi_env env, const char *name, size_t len, napi_callback constructor, void *data, size_t properties_len, const napi_property_descriptor *properties, napi_value *result);

napi_status
napi_define_properties(napi_env env, napi_value object, size_t len, const napi_property_descriptor *properties);

napi_status
napi_wrap(napi_env env, napi_value object, void *data, napi_finalize finalize_cb, void *finalize_hint, napi_ref *result);

napi_status
napi_unwrap(napi_env env, napi_value object, void **result);

napi_status
napi_remove_wrap(napi_env env, napi_value object, void **result);

napi_status
napi_add_finalizer(napi_env env, napi_value object, void *data, napi_finalize finalize_cb, void *finalize_hint, napi_ref *result);

napi_status
napi_type_tag_object(napi_env env, napi_value object, const napi_type_tag *tag);

napi_status
napi_check_object_type_tag(napi_env env, napi_value object, const napi_type_tag *tag, bool *result);

napi_status
napi_create_int32(napi_env env, int32_t value, napi_value *result);

napi_status
napi_create_uint32(napi_env env, uint32_t value, napi_value *result);

napi_status
napi_create_int64(napi_env env, int64_t value, napi_value *result);

napi_status
napi_create_double(napi_env env, double value, napi_value *result);

napi_status
napi_create_bigint_int64(napi_env env, int64_t value, napi_value *result);

napi_status
napi_create_bigint_uint64(napi_env env, uint64_t value, napi_value *result);

napi_status
napi_create_bigint_words(napi_env env, int sign_bit, size_t word_count, const uint64_t *words, napi_value *result);

napi_status
napi_create_string_utf8(napi_env env, const char *str, size_t len, napi_value *result);

napi_status
napi_create_string_utf16(napi_env env, const char16_t *str, size_t len, napi_value *result);

napi_status
napi_create_string_latin1(napi_env env, const char *str, size_t len, napi_value *result);

napi_status
node_api_create_external_string_latin1(napi_env env, char *str, size_t len, napi_finalize finalize_cb, void *finalize_hint, napi_value *result, bool *copied);

napi_status
node_api_create_external_string_utf16(napi_env env, char16_t *str, size_t len, napi_finalize finalize_cb, void *finalize_hint, napi_value *result, bool *copied);

napi_status
node_api_create_property_key_utf8(napi_env env, const char *str, size_t len, napi_value *result);

napi_status
node_api_create_property_key_utf16(napi_env env, const char16_t *str, size_t len, napi_value *result);

napi_status
node_api_create_property_key_latin1(napi_env env, const char *str, size_t len, napi_value *result);

napi_status
napi_create_symbol(napi_env env, napi_value description, napi_value *result);

napi_status
node_api_symbol_for(napi_env env, const char *description, size_t len, napi_value *result);

napi_status
napi_create_object(napi_env env, napi_value *result);

napi_status
node_api_create_object_with_properties(napi_env env, napi_value prototype, napi_value *property_names, napi_value *property_values, size_t property_count, napi_value *result);

napi_status
napi_create_function(napi_env env, const char *name, size_t len, napi_callback cb, void *data, napi_value *result);

napi_status
napi_create_array(napi_env env, napi_value *result);

napi_status
napi_create_array_with_length(napi_env env, size_t len, napi_value *result);

napi_status
napi_create_external(napi_env env, void *data, napi_finalize finalize_cb, void *finalize_hint, napi_value *result);

napi_status
napi_create_date(napi_env env, double time, napi_value *result);

napi_status
napi_create_error(napi_env env, napi_value code, napi_value message, napi_value *result);

napi_status
napi_create_type_error(napi_env env, napi_value code, napi_value message, napi_value *result);

napi_status
napi_create_range_error(napi_env env, napi_value code, napi_value message, napi_value *result);

napi_status
node_api_create_syntax_error(napi_env env, napi_value code, napi_value message, napi_value *result);

napi_status
napi_create_promise(napi_env env, napi_deferred *deferred, napi_value *promise);

napi_status
napi_resolve_deferred(napi_env env, napi_deferred deferred, napi_value resolution);

napi_status
napi_reject_deferred(napi_env env, napi_deferred deferred, napi_value resolution);

napi_status
napi_create_arraybuffer(napi_env env, size_t len, void **data, napi_value *result);

napi_status
napi_create_external_arraybuffer(napi_env env, void *data, size_t len, napi_finalize finalize_cb, void *finalize_hint, napi_value *result);

napi_status
node_api_create_sharedarraybuffer(napi_env env, size_t len, void **data, napi_value *result);

napi_status
node_api_create_external_sharedarraybuffer(napi_env env, void *data, size_t len, napi_finalize finalize_cb, void *finalize_hint, napi_value *result);

napi_status
napi_detach_arraybuffer(napi_env env, napi_value arraybuffer);

napi_status
napi_create_typedarray(napi_env env, napi_typedarray_type type, size_t len, napi_value arraybuffer, size_t offset, napi_value *result);

napi_status
napi_create_buffer(napi_env env, size_t len, void **data, napi_value *result);

napi_status
node_api_create_buffer_from_arraybuffer(napi_env env, napi_value arraybuffer, size_t byte_offset, size_t byte_length, napi_value *result);

napi_status
napi_create_buffer_copy(napi_env env, size_t len, const void *data, void **result_data, napi_value *result);

napi_status
napi_create_external_buffer(napi_env env, size_t len, void *data, napi_finalize finalize_cb, void *finalize_hint, napi_value *result);

napi_status
napi_create_dataview(napi_env env, size_t len, napi_value arraybuffer, size_t offset, napi_value *result);

napi_status
napi_coerce_to_bool(napi_env env, napi_value value, napi_value *result);

napi_status
napi_coerce_to_number(napi_env env, napi_value value, napi_value *result);

napi_status
napi_coerce_to_string(napi_env env, napi_value value, napi_value *result);

napi_status
napi_coerce_to_object(napi_env env, napi_value value, napi_value *result);

napi_status
napi_typeof(napi_env env, napi_value value, napi_valuetype *result);

napi_status
napi_is_array(napi_env env, napi_value value, bool *result);

napi_status
napi_is_date(napi_env env, napi_value value, bool *result);

napi_status
napi_is_error(napi_env env, napi_value value, bool *result);

napi_status
napi_is_promise(napi_env env, napi_value value, bool *result);

napi_status
napi_is_arraybuffer(napi_env env, napi_value value, bool *result);

napi_status
node_api_is_sharedarraybuffer(napi_env env, napi_value value, bool *result);

napi_status
napi_is_detached_arraybuffer(napi_env env, napi_value value, bool *result);

napi_status
napi_is_typedarray(napi_env env, napi_value value, bool *result);

napi_status
napi_is_buffer(napi_env env, napi_value value, bool *result);

napi_status
napi_is_dataview(napi_env env, napi_value value, bool *result);

napi_status
napi_strict_equals(napi_env env, napi_value a, napi_value b, bool *result);

napi_status
napi_instanceof(napi_env env, napi_value object, napi_value constructor, bool *result);

napi_status
napi_get_global(napi_env env, napi_value *result);

napi_status
napi_get_undefined(napi_env env, napi_value *result);

napi_status
napi_get_null(napi_env env, napi_value *result);

napi_status
napi_get_boolean(napi_env env, bool value, napi_value *result);

napi_status
napi_get_value_bool(napi_env env, napi_value value, bool *result);

napi_status
napi_get_value_int32(napi_env env, napi_value value, int32_t *result);

napi_status
napi_get_value_uint32(napi_env env, napi_value value, uint32_t *result);

napi_status
napi_get_value_int64(napi_env env, napi_value value, int64_t *result);

napi_status
napi_get_value_double(napi_env env, napi_value value, double *result);

napi_status
napi_get_value_bigint_int64(napi_env env, napi_value value, int64_t *result, bool *lossless);

napi_status
napi_get_value_bigint_uint64(napi_env env, napi_value value, uint64_t *result, bool *lossless);

napi_status
napi_get_value_bigint_words(napi_env env, napi_value value, int *sign_bit, size_t *word_count, uint64_t *words);

napi_status
napi_get_value_string_utf8(napi_env env, napi_value value, char *str, size_t len, size_t *result);

napi_status
napi_get_value_string_utf16(napi_env env, napi_value value, char16_t *str, size_t len, size_t *result);

napi_status
napi_get_value_string_latin1(napi_env env, napi_value value, char *str, size_t len, size_t *result);

napi_status
napi_get_value_external(napi_env env, napi_value value, void **result);

napi_status
napi_get_date_value(napi_env env, napi_value value, double *result);

napi_status
napi_get_array_length(napi_env env, napi_value value, uint32_t *result);

napi_status
napi_get_property_names(napi_env env, napi_value object, napi_value *result);

napi_status
napi_get_all_property_names(napi_env env, napi_value object, napi_key_collection_mode key_mode, napi_key_filter key_filter, napi_key_conversion key_conversion, napi_value *result);

napi_status
napi_get_property(napi_env env, napi_value object, napi_value key, napi_value *result);

napi_status
napi_get_prototype(napi_env env, napi_value object, napi_value *result);

napi_status
node_api_set_prototype(napi_env env, napi_value object, napi_value value);

napi_status
napi_object_freeze(napi_env env, napi_value object);

napi_status
napi_object_seal(napi_env env, napi_value object);

napi_status
napi_has_property(napi_env env, napi_value object, napi_value key, bool *result);

napi_status
napi_has_own_property(napi_env env, napi_value object, napi_value key, bool *result);

napi_status
napi_set_property(napi_env env, napi_value object, napi_value key, napi_value value);

napi_status
napi_delete_property(napi_env env, napi_value object, napi_value key, bool *result);

napi_status
napi_get_named_property(napi_env env, napi_value object, const char *name, napi_value *result);

napi_status
napi_has_named_property(napi_env env, napi_value object, const char *name, bool *result);

napi_status
napi_set_named_property(napi_env env, napi_value object, const char *name, napi_value value);

napi_status
napi_get_element(napi_env env, napi_value object, uint32_t index, napi_value *result);

napi_status
napi_has_element(napi_env env, napi_value object, uint32_t index, bool *result);

napi_status
napi_set_element(napi_env env, napi_value object, uint32_t index, napi_value value);

napi_status
napi_delete_element(napi_env env, napi_value object, uint32_t index, bool *result);

napi_status
napi_get_cb_info(napi_env env, napi_callback_info info, size_t *argc, napi_value argv[], napi_value *receiver, void **data);

napi_status
napi_get_new_target(napi_env env, napi_callback_info info, napi_value *result);

napi_status
napi_get_arraybuffer_info(napi_env env, napi_value arraybuffer, void **data, size_t *len);

napi_status
napi_get_typedarray_info(napi_env env, napi_value typedarray, napi_typedarray_type *type, size_t *len, void **data, napi_value *arraybuffer, size_t *offset);

napi_status
napi_get_buffer_info(napi_env env, napi_value buffer, void **data, size_t *len);

napi_status
napi_get_dataview_info(napi_env env, napi_value dataview, size_t *len, void **data, napi_value *arraybuffer, size_t *offset);

napi_status
napi_call_function(napi_env env, napi_value receiver, napi_value function, size_t argc, const napi_value argv[], napi_value *result);

napi_status
napi_make_callback(napi_env env, napi_async_context async_context, napi_value receiver, napi_value function, size_t argc, const napi_value argv[], napi_value *result);

napi_status
napi_new_instance(napi_env env, napi_value constructor, size_t argc, napi_value *argv, napi_value *result);

napi_status
napi_throw(napi_env env, napi_value error);

napi_status
napi_throw_error(napi_env env, const char *code, const char *message);

napi_status
napi_throw_type_error(napi_env env, const char *code, const char *message);

napi_status
napi_throw_range_error(napi_env env, const char *code, const char *message);

napi_status
node_api_throw_syntax_error(napi_env env, const char *code, const char *message);

napi_status
napi_is_exception_pending(napi_env env, bool *result);

napi_status
napi_get_and_clear_last_exception(napi_env env, napi_value *result);

napi_status
napi_fatal_exception(napi_env env, napi_value error);

void
napi_fatal_error(const char *location, size_t location_len, const char *message, size_t message_len);

napi_status
napi_adjust_external_memory(napi_env env, int64_t change_in_bytes, int64_t *result);

napi_status
napi_create_threadsafe_function(napi_env env, napi_value function, napi_value async_resource, napi_value async_resource_name, size_t max_queue_size, size_t initial_thread_count, void *thread_finalize_data, napi_finalize thread_finalize_cb, void *context, napi_threadsafe_function_call_js cb, napi_threadsafe_function *result);

napi_status
napi_get_threadsafe_function_context(napi_threadsafe_function function, void **result);

napi_status
napi_call_threadsafe_function(napi_threadsafe_function function, void *data, napi_threadsafe_function_call_mode mode);

napi_status
napi_acquire_threadsafe_function(napi_threadsafe_function function);

napi_status
napi_release_threadsafe_function(napi_threadsafe_function function, napi_threadsafe_function_release_mode mode);

napi_status
napi_ref_threadsafe_function(napi_env env, napi_threadsafe_function function);

napi_status
napi_unref_threadsafe_function(napi_env env, napi_threadsafe_function function);

napi_status
napi_add_env_cleanup_hook(napi_env env, napi_cleanup_hook callback, void *data);

napi_status
napi_remove_env_cleanup_hook(napi_env env, napi_cleanup_hook callback, void *data);

napi_status
napi_add_async_cleanup_hook(napi_env env, napi_async_cleanup_hook callback, void *data, napi_async_cleanup_hook_handle *result);

napi_status
napi_remove_async_cleanup_hook(napi_async_cleanup_hook_handle handle);

napi_status
napi_async_init(napi_env env, napi_value async_resource, napi_value async_resource_name, napi_async_context *result);

napi_status
napi_async_destroy(napi_env env, napi_async_context async_context);

napi_status
napi_open_callback_scope(napi_env env, napi_value resource, napi_async_context context, napi_callback_scope *result);

napi_status
napi_close_callback_scope(napi_env env, napi_callback_scope scope);

napi_status
napi_create_async_work(napi_env env, napi_value async_resource, napi_value async_resource_name, napi_async_execute_callback execute, napi_async_complete_callback complete, void *data, napi_async_work *result);

napi_status
napi_delete_async_work(napi_env env, napi_async_work work);

napi_status
napi_queue_async_work(napi_env env, napi_async_work work);

napi_status
napi_cancel_async_work(napi_env env, napi_async_work work);

napi_status
node_api_post_finalizer(napi_env env, napi_finalize finalize_cb, void *finalize_data, void *finalize_hint);

#ifdef __cplusplus
}
#endif

#endif // NAPI_H
