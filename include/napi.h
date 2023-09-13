#ifndef NAPI_H
#define NAPI_H

#ifdef __cplusplus
extern "C" {
#endif

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

#include "napi/module.h"

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

inline js_value_type_t
napi_convert_from_valuetype (napi_valuetype type) {
  switch (type) {
  case napi_undefined:
    return js_undefined;
  case napi_null:
    return js_null;
  case napi_boolean:
    return js_boolean;
  case napi_number:
    return js_number;
  case napi_string:
    return js_string;
  case napi_symbol:
    return js_symbol;
  case napi_object:
    return js_object;
  case napi_function:
    return js_function;
  case napi_external:
    return js_external;
  case napi_bigint:
    return js_bigint;
  }
}

inline napi_valuetype
napi_convert_to_valuetype (js_value_type_t type) {
  switch (type) {
  case js_undefined:
    return napi_undefined;
  case js_null:
    return napi_null;
  case js_boolean:
    return napi_boolean;
  case js_number:
    return napi_number;
  case js_string:
    return napi_string;
  case js_symbol:
    return napi_symbol;
  case js_object:
    return napi_object;
  case js_function:
    return napi_function;
  case js_external:
    return napi_external;
  case js_bigint:
    return napi_bigint;
  }
}

inline js_typedarray_type_t
napi_convert_from_typedarray_type (napi_typedarray_type type) {
  switch (type) {
  case napi_int8_array:
    return js_int8_array;
  case napi_uint8_array:
    return js_uint8_array;
  case napi_uint8_clamped_array:
    return js_uint8_clamped_array;
  case napi_int16_array:
    return js_int16_array;
  case napi_uint16_array:
    return js_uint16_array;
  case napi_int32_array:
    return js_int32_array;
  case napi_uint32_array:
    return js_uint32_array;
  case napi_float32_array:
    return js_float32_array;
  case napi_float64_array:
    return js_float64_array;
  case napi_bigint64_array:
    return js_bigint64_array;
  case napi_biguint64_array:
    return js_biguint64_array;
  }
}

inline napi_typedarray_type
napi_convert_to_typedarray_type (js_typedarray_type_t type) {
  switch (type) {
  case js_int8_array:
    return napi_int8_array;
  case js_uint8_array:
    return napi_uint8_array;
  case js_uint8_clamped_array:
    return napi_uint8_clamped_array;
  case js_int16_array:
    return napi_int16_array;
  case js_uint16_array:
    return napi_uint16_array;
  case js_int32_array:
    return napi_int32_array;
  case js_uint32_array:
    return napi_uint32_array;
  case js_float32_array:
    return napi_float32_array;
  case js_float64_array:
    return napi_float64_array;
  case js_bigint64_array:
    return napi_bigint64_array;
  case js_biguint64_array:
    return napi_biguint64_array;
  }
}

inline napi_status
napi_get_uv_event_loop (napi_env env, uv_loop_t **loop) {
  int err = js_get_env_loop(env, loop);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_open_handle_scope (napi_env env, napi_handle_scope *result) {
  int err = js_open_handle_scope(env, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_close_handle_scope (napi_env env, napi_handle_scope scope) {
  int err = js_close_handle_scope(env, scope);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_open_escapable_handle_scope (napi_env env, napi_escapable_handle_scope *result) {
  int err = js_open_escapable_handle_scope(env, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_close_escapable_handle_scope (napi_env env, napi_escapable_handle_scope scope) {
  int err = js_close_escapable_handle_scope(env, scope);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_escape_handle (napi_env env, napi_escapable_handle_scope scope, napi_value escapee, napi_value *result) {
  int err = js_escape_handle(env, scope, escapee, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_run_script (napi_env env, napi_value source, napi_value *result) {
  int err = js_run_script(env, "<anonymous>", -1, 0, source, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_reference (napi_env env, napi_value value, uint32_t initial_refcount, napi_ref *result) {
  int err = js_create_reference(env, value, initial_refcount, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_delete_reference (napi_env env, napi_ref ref) {
  int err = js_delete_reference(env, ref);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_reference_ref (napi_env env, napi_ref ref, uint32_t *result) {
  int err = js_reference_ref(env, ref, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_reference_unref (napi_env env, napi_ref ref, uint32_t *result) {
  int err = js_reference_unref(env, ref, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_reference_value (napi_env env, napi_ref ref, napi_value *result) {
  int err = js_get_reference_value(env, ref, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_define_class (napi_env env, const char *name, size_t len, napi_callback constructor, void *data, size_t properties_len, const napi_property_descriptor *properties, napi_value *result) {
  js_property_descriptor_t *js_properties = malloc(properties_len * sizeof(js_property_descriptor_t));

  for (size_t i = 0; i < properties_len; i++) {
    if (properties[i].name) {
      js_throw_errorf(env, NULL, "Only string literal property names are supported");

      goto err;
    }

    js_properties[i] = (js_property_descriptor_t){
      .name = properties[i].utf8name,
      .data = properties[i].data,
      .attributes = properties[i].attributes,
      .method = properties[i].method,
      .getter = properties[i].getter,
      .setter = properties[i].setter,
      .value = properties[i].value,
    };
  }

  int err = js_define_class(env, name, len, constructor, data, js_properties, properties_len, result);
  if (err < 0) goto err;

  return napi_ok;

err:
  free(js_properties);

  return napi_pending_exception;
}

inline napi_status
napi_define_properties (napi_env env, napi_value object, size_t len, const napi_property_descriptor *properties) {
  js_property_descriptor_t *js_properties = malloc(len * sizeof(js_property_descriptor_t));

  for (size_t i = 0; i < len; i++) {
    if (properties[i].name) {
      js_throw_errorf(env, NULL, "Only string literal property names are supported");

      goto err;
    }

    js_properties[i] = (js_property_descriptor_t){
      .name = properties[i].utf8name,
      .data = properties[i].data,
      .attributes = properties[i].attributes,
      .method = properties[i].method,
      .getter = properties[i].getter,
      .setter = properties[i].setter,
      .value = properties[i].value,
    };
  }

  int err = js_define_properties(env, object, js_properties, len);
  if (err < 0) goto err;

  return napi_ok;

err:
  free(js_properties);

  return napi_pending_exception;
}

inline napi_status
napi_wrap (napi_env env, napi_value object, void *data, napi_finalize finalize_cb, void *finalize_hint, napi_ref *result) {
  int err = js_wrap(env, object, data, finalize_cb, finalize_hint, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_unwrap (napi_env env, napi_value object, void **result) {
  int err = js_unwrap(env, object, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_remove_wrap (napi_env env, napi_value object, void **result) {
  int err = js_remove_wrap(env, object, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_add_finalizer (napi_env env, napi_value object, void *data, napi_finalize finalize_cb, void *finalize_hint, napi_ref *result) {
  int err = js_add_finalizer(env, object, data, finalize_cb, finalize_hint, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_type_tag_object (napi_env env, napi_value object, const napi_type_tag *tag) {
  int err = js_add_type_tag(env, object, &(js_type_tag_t){tag->lower, tag->upper});
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_check_object_type_tag (napi_env env, napi_value object, const napi_type_tag *tag, bool *result) {
  int err = js_check_type_tag(env, object, &(js_type_tag_t){tag->lower, tag->upper}, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_int32 (napi_env env, int32_t value, napi_value *result) {
  int err = js_create_int32(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_uint32 (napi_env env, uint32_t value, napi_value *result) {
  int err = js_create_uint32(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_int64 (napi_env env, int64_t value, napi_value *result) {
  int err = js_create_int64(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_double (napi_env env, double value, napi_value *result) {
  int err = js_create_double(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_bigint_int64 (napi_env env, int64_t value, napi_value *result) {
  int err = js_create_bigint_int64(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_bigint_uint64 (napi_env env, uint64_t value, napi_value *result) {
  int err = js_create_bigint_uint64(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_string_utf8 (napi_env env, const char *str, size_t len, napi_value *result) {
  int err = js_create_string_utf8(env, (const utf8_t *) str, len, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_string_utf16 (napi_env env, const char16_t *str, size_t len, napi_value *result) {
  int err = js_create_string_utf16le(env, str, len, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_symbol (napi_env env, napi_value description, napi_value *result) {
  int err = js_create_symbol(env, description, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_object (napi_env env, napi_value *result) {
  int err = js_create_object(env, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_function (napi_env env, const char *name, size_t len, napi_callback cb, void *data, napi_value *result) {
  int err = js_create_function(env, name, len, cb, data, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_array (napi_env env, napi_value *result) {
  int err = js_create_array(env, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_array_with_length (napi_env env, size_t len, napi_value *result) {
  int err = js_create_array_with_length(env, len, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_external (napi_env env, void *data, napi_finalize finalize_cb, void *finalize_hint, napi_value *result) {
  int err = js_create_external(env, data, finalize_cb, finalize_hint, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_date (napi_env env, double time, napi_value *result) {
  int err = js_create_date(env, time, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_error (napi_env env, napi_value code, napi_value message, napi_value *result) {
  int err = js_create_error(env, code, message, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_type_error (napi_env env, napi_value code, napi_value message, napi_value *result) {
  int err = js_create_type_error(env, code, message, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_range_error (napi_env env, napi_value code, napi_value message, napi_value *result) {
  int err = js_create_range_error(env, code, message, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_promise (napi_env env, napi_deferred *deferred, napi_value *promise) {
  int err = js_create_promise(env, deferred, promise);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_resolve_deferred (napi_env env, napi_deferred deferred, napi_value resolution) {
  int err = js_resolve_deferred(env, deferred, resolution);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_reject_deferred (napi_env env, napi_deferred deferred, napi_value resolution) {
  int err = js_reject_deferred(env, deferred, resolution);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_arraybuffer (napi_env env, size_t len, void **data, napi_value *result) {
  int err = js_create_arraybuffer(env, len, data, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_external_arraybuffer (napi_env env, void *data, size_t len, napi_finalize finalize_cb, void *finalize_hint, napi_value *result) {
  int err = js_create_external_arraybuffer(env, data, len, finalize_cb, finalize_hint, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_detach_arraybuffer (napi_env env, napi_value arraybuffer) {
  int err = js_detach_arraybuffer(env, arraybuffer);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_typedarray (napi_env env, napi_typedarray_type type, size_t len, napi_value arraybuffer, size_t offset, napi_value *result) {
  js_typedarray_type_t js_type = napi_convert_from_typedarray_type(type);

  int err = js_create_typedarray(env, js_type, len, arraybuffer, offset, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_buffer (napi_env env, size_t len, void **data, napi_value *result) {
  js_value_t *arraybuffer;

  int err = js_create_arraybuffer(env, len, data, &arraybuffer);
  if (err < 0) return napi_pending_exception;

  err = js_create_typedarray(env, js_uint8_array, len, arraybuffer, 0, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_buffer_copy (napi_env env, size_t len, const void *data, void **result_data, napi_value *result) {
  js_value_t *arraybuffer;

  int err = js_create_arraybuffer(env, len, result_data, &arraybuffer);
  if (err < 0) return napi_pending_exception;

  if (result_data) memcpy(result_data, data, len);

  err = js_create_typedarray(env, js_uint8_array, len, arraybuffer, 0, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_external_buffer (napi_env env, size_t len, void *data, napi_finalize finalize_cb, void *finalize_hint, napi_value *result) {
  js_value_t *arraybuffer;

  int err = js_create_external_arraybuffer(env, data, len, finalize_cb, finalize_hint, &arraybuffer);
  if (err < 0) return napi_pending_exception;

  err = js_create_typedarray(env, js_uint8_array, len, arraybuffer, 0, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_create_dataview (napi_env env, size_t len, napi_value arraybuffer, size_t offset, napi_value *result) {
  int err = js_create_dataview(env, len, arraybuffer, offset, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_typeof (napi_env env, napi_value value, napi_valuetype *result) {
  js_value_type_t js_type;

  int err = js_typeof(env, value, &js_type);
  if (err < 0) return napi_pending_exception;

  *result = napi_convert_to_valuetype(js_type);

  return napi_ok;
}

inline napi_status
napi_is_array (napi_env env, napi_value value, bool *result) {
  int err = js_is_array(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_is_date (napi_env env, napi_value value, bool *result) {
  int err = js_is_date(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_is_error (napi_env env, napi_value value, bool *result) {
  int err = js_is_error(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_is_promise (napi_env env, napi_value value, bool *result) {
  int err = js_is_promise(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_is_arraybuffer (napi_env env, napi_value value, bool *result) {
  int err = js_is_arraybuffer(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_is_detached_arraybuffer (napi_env env, napi_value value, bool *result) {
  int err = js_is_detached_arraybuffer(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_is_typedarray (napi_env env, napi_value value, bool *result) {
  int err = js_is_typedarray(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_is_buffer (napi_env env, napi_value value, bool *result) {
  int err = js_is_typedarray(env, value, result);
  if (err < 0) return napi_pending_exception;

  if (!*result) return napi_ok;

  js_typedarray_type_t type;
  err = js_get_typedarray_info(env, value, &type, NULL, NULL, NULL, NULL);
  if (err < 0) return napi_pending_exception;

  *result = type == js_uint8_array;

  return napi_ok;
}

inline napi_status
napi_is_dataview (napi_env env, napi_value value, bool *result) {
  int err = js_is_dataview(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_strict_equals (napi_env env, napi_value a, napi_value b, bool *result) {
  int err = js_strict_equals(env, a, b, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_global (napi_env env, napi_value *result) {
  int err = js_get_global(env, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_undefined (napi_env env, napi_value *result) {
  int err = js_get_undefined(env, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_null (napi_env env, napi_value *result) {
  int err = js_get_null(env, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_boolean (napi_env env, bool value, napi_value *result) {
  int err = js_get_boolean(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_value_bool (napi_env env, napi_value value, bool *result) {
  int err = js_get_value_bool(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_value_int32 (napi_env env, napi_value value, int32_t *result) {
  int err = js_get_value_int32(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_value_uint32 (napi_env env, napi_value value, uint32_t *result) {
  int err = js_get_value_uint32(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_value_int64 (napi_env env, napi_value value, int64_t *result) {
  int err = js_get_value_int64(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_value_double (napi_env env, napi_value value, double *result) {
  int err = js_get_value_double(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_value_bigint_int64 (napi_env env, napi_value value, int64_t *result, bool *lossless) {
  int err = js_get_value_bigint_int64(env, value, result, lossless);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_value_bigint_uint64 (napi_env env, napi_value value, uint64_t *result, bool *lossless) {
  int err = js_get_value_bigint_uint64(env, value, result, lossless);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_value_string_utf8 (napi_env env, napi_value value, char *str, size_t len, size_t *result) {
  int err = js_get_value_string_utf8(env, value, (utf8_t *) str, len > 0 ? len - 1 : 0, result);
  if (err < 0) return napi_pending_exception;

  if (len > 0) str[len - 1] = '\0'; // Always NULL terminate

  return napi_ok;
}

inline napi_status
napi_get_value_string_utf16 (napi_env env, napi_value value, char16_t *str, size_t len, size_t *result) {
  int err = js_get_value_string_utf16le(env, value, str, len > 0 ? len - 1 : 0, result);
  if (err < 0) return napi_pending_exception;

  if (len > 0) str[len - 1] = L'\0'; // Always NULL terminate

  return napi_ok;
}

inline napi_status
napi_get_value_external (napi_env env, napi_value value, void **result) {
  int err = js_get_value_external(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_value_date (napi_env env, napi_value value, double *result) {
  int err = js_get_value_date(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_array_length (napi_env env, napi_value value, uint32_t *result) {
  int err = js_get_array_length(env, value, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_property_names (napi_env env, napi_value object, napi_value *result) {
  int err = js_get_property_names(env, object, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_property (napi_env env, napi_value object, napi_value key, napi_value *result) {
  int err = js_get_property(env, object, key, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_has_property (napi_env env, napi_value object, napi_value key, bool *result) {
  int err = js_has_property(env, object, key, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_set_property (napi_env env, napi_value object, napi_value key, napi_value value) {
  int err = js_set_property(env, object, key, value);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_delete_property (napi_env env, napi_value object, napi_value key, bool *result) {
  int err = js_delete_property(env, object, key, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_named_property (napi_env env, napi_value object, const char *name, napi_value *result) {
  int err = js_get_named_property(env, object, name, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_has_named_property (napi_env env, napi_value object, const char *name, bool *result) {
  int err = js_has_named_property(env, object, name, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_set_named_property (napi_env env, napi_value object, const char *name, napi_value value) {
  int err = js_set_named_property(env, object, name, value);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_element (napi_env env, napi_value object, uint32_t index, napi_value *result) {
  int err = js_get_element(env, object, index, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_has_element (napi_env env, napi_value object, uint32_t index, bool *result) {
  int err = js_has_element(env, object, index, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_set_element (napi_env env, napi_value object, uint32_t index, napi_value value) {
  int err = js_set_element(env, object, index, value);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_delete_element (napi_env env, napi_value object, uint32_t index, bool *result) {
  int err = js_delete_element(env, object, index, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_cb_info (napi_env env, napi_callback_info info, size_t *argc, napi_value argv[], napi_value *receiver, void **data) {
  int err = js_get_callback_info(env, info, argc, argv, receiver, data);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_arraybuffer_info (napi_env env, napi_value arraybuffer, void **data, size_t *len) {
  int err = js_get_arraybuffer_info(env, arraybuffer, data, len);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_typedarray_info (napi_env env, napi_value typedarray, napi_typedarray_type *type, size_t *len, void **data, napi_value *arraybuffer, size_t *offset) {
  js_typedarray_type_t js_type;

  int err = js_get_typedarray_info(env, typedarray, type == NULL ? NULL : &js_type, data, len, arraybuffer, offset);
  if (err < 0) return napi_pending_exception;

  if (type != NULL) {
    *type = napi_convert_to_typedarray_type(js_type);
  }

  return napi_ok;
}

inline napi_status
napi_get_buffer_info (napi_env env, napi_value buffer, void **data, size_t *len) {
  int err = js_get_typedarray_info(env, buffer, NULL, data, len, NULL, NULL);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_dataview_info (napi_env env, napi_value dataview, size_t *len, void **data, napi_value *arraybuffer, size_t *offset) {
  int err = js_get_dataview_info(env, dataview, data, len, arraybuffer, offset);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_call_function (napi_env env, napi_value receiver, napi_value function, size_t argc, const napi_value argv[], napi_value *result) {
  int err = js_call_function(env, receiver, function, argc, argv, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_make_callback (napi_env env, napi_async_context async_context, napi_value receiver, napi_value function, size_t argc, const napi_value argv[], napi_value *result) {
  int err = js_call_function_with_checkpoint(env, receiver, function, argc, argv, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_throw (napi_env env, napi_value error) {
  int err = js_throw(env, error);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_throw_error (napi_env env, const char *code, const char *message) {
  int err = js_throw_error(env, code, message);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_throw_type_error (napi_env env, const char *code, const char *message) {
  int err = js_throw_type_error(env, code, message);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_throw_range_error (napi_env env, const char *code, const char *message) {
  int err = js_throw_range_error(env, code, message);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_is_exception_pending (napi_env env, bool *result) {
  int err = js_is_exception_pending(env, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_get_and_clear_last_exception (napi_env env, napi_value *result) {
  int err = js_get_and_clear_last_exception(env, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_fatal_exception (napi_env env, napi_value error) {
  int err = js_fatal_exception(env, error);
  return err == 0 ? napi_ok : napi_pending_exception;
}

inline napi_status
napi_adjust_external_memory (napi_env env, int64_t change_in_bytes, int64_t *result) {
  int err = js_adjust_external_memory(env, change_in_bytes, result);
  return err == 0 ? napi_ok : napi_pending_exception;
}

#ifdef __cplusplus
}
#endif

#endif // NAPI_H
