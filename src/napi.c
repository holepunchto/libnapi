#include <js.h>
#include <utf.h>
#include <uv.h>

#include "../include/napi.h"

#ifndef thread_local
#ifdef _WIN32
#define thread_local __declspec(thread)
#else
#define thread_local _Thread_local
#endif
#endif

// The Node-API version reported by `napi_get_version()`. The runtime embedding
// us owns this value and may override it at build time.
#ifndef NAPI_VERSION
#define NAPI_VERSION 10
#endif

typedef struct {
  napi_env env;
  void *data;
  napi_finalize finalize_cb;
  void *finalize_hint;
} napi_instance_data;

static const char *napi_error_messages[] = {
  NULL,
  "Invalid argument",
  "An object was expected",
  "A string was expected",
  "A string or symbol was expected",
  "A function was expected",
  "A number was expected",
  "A boolean was expected",
  "An array was expected",
  "Unknown failure",
  "An exception is pending",
  "The async work item was cancelled",
  "napi_escape_handle already called on scope",
  "Invalid handle scope usage",
  "Invalid callback scope usage",
  "Thread-safe function queue is full",
  "Thread-safe function handle is closing",
  "A bigint was expected",
  "A date was expected",
  "An arraybuffer was expected",
  "A detachable arraybuffer was expected",
  "Main thread would deadlock",
  "External buffers are not allowed",
  "Cannot run JavaScript",
};

static thread_local napi_extended_error_info napi__extended_error_info = {
  .error_code = 0,
  .error_message = NULL,
  .engine_error_code = 0,
  .engine_reserved = NULL,
};

static napi_status
napi_set_last_error_info(napi_env env, napi_status error_code, uint32_t engine_error_code, void *engine_reserved) {
  napi__extended_error_info.error_code = error_code;
  napi__extended_error_info.engine_error_code = engine_error_code;
  napi__extended_error_info.engine_reserved = engine_reserved;

  return error_code;
}

static napi_status
napi_clear_last_error_info(napi_env env) {
  napi__extended_error_info.error_code = napi_ok;
  napi__extended_error_info.error_message = NULL;
  napi__extended_error_info.engine_error_code = 0;
  napi__extended_error_info.engine_reserved = NULL;

  return napi_ok;
}

napi_status
napi_get_last_error_info(napi_env env, const napi_extended_error_info **result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi__extended_error_info.error_message = napi_error_messages[napi__extended_error_info.error_code];

  if (napi__extended_error_info.error_code == napi_ok) {
    napi_clear_last_error_info(env);
  }

  *result = &napi__extended_error_info;

  return napi_ok;
}

static thread_local napi_instance_data napi__instance_data = {
  .data = NULL,
  .finalize_cb = NULL,
  .finalize_hint = NULL,
};

static void
napi__on_instance_data_finalize(void *data) {
  napi__instance_data.finalize_cb(napi__instance_data.env, napi__instance_data.data, napi__instance_data.finalize_hint);
}

napi_status
napi_set_instance_data(napi_env env, void *data, napi_finalize finalize_cb, void *finalize_hint) {
  if (env == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err;

  if (napi__instance_data.finalize_cb) {
    err = js_remove_teardown_callback(env, napi__on_instance_data_finalize, NULL);

    if (err < 0) return napi_set_last_error_info(env, napi_pending_exception, 0, NULL);
  }

  if (finalize_cb) {
    err = js_add_teardown_callback(env, napi__on_instance_data_finalize, NULL);

    if (err < 0) return napi_set_last_error_info(env, napi_pending_exception, 0, NULL);
  }

  napi__instance_data.env = env;
  napi__instance_data.data = data;
  napi__instance_data.finalize_cb = finalize_cb;
  napi__instance_data.finalize_hint = finalize_hint;

  return napi_clear_last_error_info(env);
}

napi_status
napi_get_instance_data(napi_env env, void **result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  *result = napi__instance_data.data;

  return napi_clear_last_error_info(env);
}

napi_status
napi_get_version(napi_env env, uint32_t *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  *result = NAPI_VERSION;

  return napi_clear_last_error_info(env);
}

static napi_status
napi_convert_to_status(int err) {
  switch (err) {
  case 0:
    return napi_ok;
  case js_pending_exception:
  case js_uncaught_exception:
    return napi_pending_exception;
  default:
    return napi_generic_failure;
  }
}

static napi_valuetype
napi_convert_to_valuetype(js_value_type_t type) {
  switch (type) {
  case js_undefined:
    return napi_undefined;
  case js_null:
  default:
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

static napi_status
napi_get_value_type(napi_env env, napi_value value, js_value_type_t *result) {
  if (value == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_typeof(env, value, result);

  if (err < 0) return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);

  return napi_ok;
}

static napi_status
napi_check_value_type(napi_env env, napi_value value, js_value_type_t type, napi_status error) {
  js_value_type_t actual;

  napi_status status = napi_get_value_type(env, value, &actual);

  if (status != napi_ok) return status;

  if (actual != type) return napi_set_last_error_info(env, error, 0, NULL);

  return napi_ok;
}

static napi_status
napi_check_object(napi_env env, napi_value value, napi_status error) {
  js_value_type_t type;

  napi_status status = napi_get_value_type(env, value, &type);

  if (status != napi_ok) return status;

  if (type != js_object && type != js_function) return napi_set_last_error_info(env, error, 0, NULL);

  return napi_ok;
}

static napi_status
napi_get_object_type(napi_env env, napi_value value, js_object_type_t *result, napi_status error) {
  napi_status status = napi_check_value_type(env, value, js_object, error);

  if (status != napi_ok) return status;

  int err = js_get_object_type(env, value, result);

  if (err < 0) return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);

  return napi_ok;
}

static napi_status
napi_check_object_type(napi_env env, napi_value value, js_object_type_t type, napi_status error) {
  js_object_type_t actual;

  napi_status status = napi_get_object_type(env, value, &actual, error);

  if (status != napi_ok) return status;

  if (actual != type) return napi_set_last_error_info(env, error, 0, NULL);

  return napi_ok;
}

static napi_status
napi_check_name(napi_env env, napi_value value) {
  js_value_type_t type;

  napi_status status = napi_get_value_type(env, value, &type);

  if (status != napi_ok) return status;

  if (type != js_string && type != js_symbol) return napi_set_last_error_info(env, napi_name_expected, 0, NULL);

  return napi_ok;
}

static napi_status
napi_check_error_args(napi_env env, napi_value code, napi_value message) {
  if (code != NULL) {
    napi_status status = napi_check_value_type(env, code, js_string, napi_string_expected);

    if (status != napi_ok) return status;
  }

  return napi_check_value_type(env, message, js_string, napi_string_expected);
}

static napi_status
napi_get_arraybuffer_length(napi_env env, napi_value value, size_t *result, napi_status error) {
  js_object_type_t type;

  napi_status status = napi_get_object_type(env, value, &type, error);

  if (status != napi_ok) return status;

  int err;

  switch (type) {
  case js_arraybuffer:
    err = js_get_arraybuffer_info(env, value, NULL, result);
    break;
  case js_sharedarraybuffer:
    err = js_get_sharedarraybuffer_info(env, value, NULL, result);
    break;
  default:
    return napi_set_last_error_info(env, error, 0, NULL);
  }

  if (err < 0) return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);

  return napi_ok;
}

static napi_status
napi_throw_range_error_pending(napi_env env, const char *code, const char *message) {
  int err = js_throw_range_error(env, code, message);

  if (err < 0) return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);

  return napi_set_last_error_info(env, napi_pending_exception, 0, NULL);
}

static napi_status
napi_to_object(napi_env env, napi_value value, napi_value *result) {
  js_value_type_t type;

  napi_status status = napi_get_value_type(env, value, &type);

  if (status != napi_ok) return status;

  switch (type) {
  case js_undefined:
  case js_null:
    return napi_set_last_error_info(env, napi_object_expected, 0, NULL);

  case js_object:
  case js_function:
    *result = value;
    return napi_ok;

  default: {
    int err = js_coerce_to_object(env, value, result);

    if (err < 0) return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);

    return napi_ok;
  }
  }
}

static js_typedarray_type_t
napi_convert_from_typedarray_type(napi_typedarray_type type) {
  switch (type) {
  case napi_int8_array:
    return js_int8array;
  case napi_uint8_array:
  default:
    return js_uint8array;
  case napi_uint8_clamped_array:
    return js_uint8clampedarray;
  case napi_int16_array:
    return js_int16array;
  case napi_uint16_array:
    return js_uint16array;
  case napi_int32_array:
    return js_int32array;
  case napi_uint32_array:
    return js_uint32array;
  case napi_float32_array:
    return js_float32array;
  case napi_float16_array:
    return js_float16array;
  case napi_float64_array:
    return js_float64array;
  case napi_bigint64_array:
    return js_bigint64array;
  case napi_biguint64_array:
    return js_biguint64array;
  }
}

static size_t
napi_typedarray_element_size(js_typedarray_type_t type) {
  switch (type) {
  case js_int8array:
  case js_uint8array:
  case js_uint8clampedarray:
  default:
    return 1;
  case js_int16array:
  case js_uint16array:
  case js_float16array:
    return 2;
  case js_int32array:
  case js_uint32array:
  case js_float32array:
    return 4;
  case js_float64array:
  case js_bigint64array:
  case js_biguint64array:
    return 8;
  }
}

static napi_typedarray_type
napi_convert_to_typedarray_type(js_typedarray_type_t type) {
  switch (type) {
  case js_int8array:
    return napi_int8_array;
  case js_uint8array:
  default:
    return napi_uint8_array;
  case js_uint8clampedarray:
    return napi_uint8_clamped_array;
  case js_int16array:
    return napi_int16_array;
  case js_uint16array:
    return napi_uint16_array;
  case js_int32array:
    return napi_int32_array;
  case js_uint32array:
    return napi_uint32_array;
  case js_float32array:
    return napi_float32_array;
  case js_float16array:
    return napi_float16_array;
  case js_float64array:
    return napi_float64_array;
  case js_bigint64array:
    return napi_bigint64_array;
  case js_biguint64array:
    return napi_biguint64_array;
  }
}

static js_key_collection_mode_t
napi_convert_from_key_collection_mode(napi_key_collection_mode mode) {
  switch (mode) {
  case napi_key_include_prototypes:
  default:
    return js_key_include_prototypes;
  case napi_key_own_only:
    return js_key_own_only;
  }
}

static js_property_filter_t
napi_convert_from_key_filter(napi_key_filter filter) {
  int result = js_property_all_properties;

  if (filter & napi_key_writable) result |= js_property_only_writable;
  if (filter & napi_key_enumerable) result |= js_property_only_enumerable;
  if (filter & napi_key_configurable) result |= js_property_only_configurable;
  if (filter & napi_key_skip_strings) result |= js_property_skip_strings;
  if (filter & napi_key_skip_symbols) result |= js_property_skip_symbols;

  return (js_property_filter_t) result;
}

static js_key_conversion_mode_t
napi_convert_from_key_conversion(napi_key_conversion mode) {
  switch (mode) {
  case napi_key_keep_numbers:
  default:
    return js_key_keep_numbers;
  case napi_key_numbers_to_strings:
    return js_key_convert_to_string;
  }
}

static js_threadsafe_function_release_mode_t
napi_convert_from_threadsafe_function_release_mode(napi_threadsafe_function_release_mode mode) {
  switch (mode) {
  case napi_tsfn_release:
  default:
    return js_threadsafe_function_release;
  case napi_tsfn_abort:
    return js_threadsafe_function_abort;
  }
}

static js_threadsafe_function_call_mode_t
napi_convert_from_threadsafe_function_call_mode(napi_threadsafe_function_call_mode mode) {
  switch (mode) {
  case napi_tsfn_nonblocking:
  default:
    return js_threadsafe_function_nonblocking;
  case napi_tsfn_blocking:
    return js_threadsafe_function_blocking;
  }
}

napi_status
napi_get_uv_event_loop(napi_env env, uv_loop_t **loop) {
  if (env == NULL || loop == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_get_env_loop(env, loop);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_open_handle_scope(napi_env env, napi_handle_scope *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_open_handle_scope(env, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_close_handle_scope(napi_env env, napi_handle_scope scope) {
  if (env == NULL || scope == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_close_handle_scope(env, scope);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_open_escapable_handle_scope(napi_env env, napi_escapable_handle_scope *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_open_escapable_handle_scope(env, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_close_escapable_handle_scope(napi_env env, napi_escapable_handle_scope scope) {
  if (env == NULL || scope == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_close_escapable_handle_scope(env, scope);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_escape_handle(napi_env env, napi_escapable_handle_scope scope, napi_value escapee, napi_value *result) {
  if (env == NULL || scope == NULL || escapee == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_escape_handle(env, scope, escapee, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_run_script(napi_env env, napi_value source, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_value_type(env, source, js_string, napi_string_expected);

  if (status != napi_ok) return status;

  int err = js_run_script(env, "<anonymous>", (size_t) -1, 0, source, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_reference(napi_env env, napi_value value, uint32_t initial_refcount, napi_ref *result) {
  if (env == NULL || value == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_reference(env, value, initial_refcount, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_delete_reference(napi_env env, napi_ref ref) {
  if (env == NULL || ref == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_delete_reference(env, ref);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_reference_ref(napi_env env, napi_ref ref, uint32_t *result) {
  if (env == NULL || ref == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_reference_ref(env, ref, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_reference_unref(napi_env env, napi_ref ref, uint32_t *result) {
  if (env == NULL || ref == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_reference_unref(env, ref, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_reference_value(napi_env env, napi_ref ref, napi_value *result) {
  if (env == NULL || ref == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_get_reference_value(env, ref, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

static napi_status
napi_convert_from_property_descriptor(napi_env env, const napi_property_descriptor *property, js_property_descriptor_t *result) {
  js_value_t *name = property->name;

  if (name == NULL) {
    if (property->utf8name == NULL) return napi_set_last_error_info(env, napi_name_expected, 0, NULL);

    int err = js_create_string_utf8(env, (const utf8_t *) property->utf8name, (size_t) -1, &name);

    if (err < 0) return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
  } else {
    napi_status status = napi_check_name(env, name);

    if (status != napi_ok) return status;
  }

  *result = (js_property_descriptor_t) {
    .name = name,
    .data = property->data,
    .attributes = (int) property->attributes,
    .method = property->method,
    .getter = property->getter,
    .setter = property->setter,
    .value = property->value,
  };

  return napi_ok;
}

napi_status
napi_define_class(napi_env env, const char *name, size_t len, napi_callback constructor, void *data, size_t properties_len, const napi_property_descriptor *properties, napi_value *result) {
  if (env == NULL || name == NULL || constructor == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if (properties == NULL && properties_len > 0) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_ok;

  js_property_descriptor_t *js_properties = malloc(properties_len * sizeof(js_property_descriptor_t));

  for (size_t i = 0; i < properties_len && status == napi_ok; i++) {
    status = napi_convert_from_property_descriptor(env, &properties[i], &js_properties[i]);
  }

  if (status == napi_ok) {
    int err = js_define_class(env, name, len, constructor, data, js_properties, properties_len, result);

    status = napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
  }

  free(js_properties);

  return status;
}

napi_status
napi_define_properties(napi_env env, napi_value object, size_t len, const napi_property_descriptor *properties) {
  if (env == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if (properties == NULL && len > 0) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_to_object(env, object, &object);

  if (status != napi_ok) return status;

  js_property_descriptor_t *js_properties = malloc(len * sizeof(js_property_descriptor_t));

  for (size_t i = 0; i < len && status == napi_ok; i++) {
    status = napi_convert_from_property_descriptor(env, &properties[i], &js_properties[i]);
  }

  if (status == napi_ok) {
    int err = js_define_properties(env, object, js_properties, len);

    status = napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
  }

  free(js_properties);

  return status;
}

napi_status
napi_wrap(napi_env env, napi_value object, void *data, napi_finalize finalize_cb, void *finalize_hint, napi_ref *result) {
  if (env == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_object(env, object, napi_invalid_arg);

  if (status != napi_ok) return status;

  int err = js_wrap(env, object, data, finalize_cb, finalize_hint, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_unwrap(napi_env env, napi_value object, void **result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_object(env, object, napi_invalid_arg);

  if (status != napi_ok) return status;

  int err = js_unwrap(env, object, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_remove_wrap(napi_env env, napi_value object, void **result) {
  if (env == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_object(env, object, napi_invalid_arg);

  if (status != napi_ok) return status;

  int err = js_remove_wrap(env, object, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_add_finalizer(napi_env env, napi_value object, void *data, napi_finalize finalize_cb, void *finalize_hint, napi_ref *result) {
  if (env == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if (finalize_cb == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_object(env, object, napi_invalid_arg);

  if (status != napi_ok) return status;

  int err = js_add_finalizer(env, object, data, finalize_cb, finalize_hint, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_type_tag_object(napi_env env, napi_value object, const napi_type_tag *tag) {
  if (env == NULL || tag == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_to_object(env, object, &object);

  if (status != napi_ok) return status;

  int err = js_add_type_tag(env, object, &(js_type_tag_t) {tag->lower, tag->upper});

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_check_object_type_tag(napi_env env, napi_value object, const napi_type_tag *tag, bool *result) {
  if (env == NULL || tag == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_to_object(env, object, &object);

  if (status != napi_ok) return status;

  int err = js_check_type_tag(env, object, &(js_type_tag_t) {tag->lower, tag->upper}, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_int32(napi_env env, int32_t value, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_int32(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_uint32(napi_env env, uint32_t value, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_uint32(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_int64(napi_env env, int64_t value, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_int64(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_double(napi_env env, double value, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_double(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_bigint_int64(napi_env env, int64_t value, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_bigint_int64(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_bigint_uint64(napi_env env, uint64_t value, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_bigint_uint64(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_bigint_words(napi_env env, int sign_bit, size_t word_count, const uint64_t *words, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if (words == NULL && word_count != 0) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_bigint_words(env, sign_bit, words, word_count, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_string_utf8(napi_env env, const char *str, size_t len, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if (str == NULL && len != 0) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_string_utf8(env, (const utf8_t *) str, len, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_string_utf16(napi_env env, const char16_t *str, size_t len, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if (str == NULL && len != 0) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_string_utf16le(env, (const utf16_t *) str, len, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_string_latin1(napi_env env, const char *str, size_t len, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if (str == NULL && len != 0) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_string_latin1(env, (const latin1_t *) str, len, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
node_api_create_external_string_latin1(napi_env env, char *str, size_t len, napi_finalize finalize_cb, void *finalize_hint, napi_value *result, bool *copied) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if (str == NULL && len != 0) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_external_string_latin1(env, (latin1_t *) str, len, finalize_cb, finalize_hint, result, copied);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
node_api_create_external_string_utf16(napi_env env, char16_t *str, size_t len, napi_finalize finalize_cb, void *finalize_hint, napi_value *result, bool *copied) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if (str == NULL && len != 0) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_external_string_utf16le(env, (utf16_t *) str, len, finalize_cb, finalize_hint, result, copied);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
node_api_create_property_key_utf8(napi_env env, const char *str, size_t len, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if (str == NULL && len != 0) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_property_key_utf8(env, (const utf8_t *) str, len, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
node_api_create_property_key_utf16(napi_env env, const char16_t *str, size_t len, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if (str == NULL && len != 0) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_property_key_utf16le(env, (const utf16_t *) str, len, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
node_api_create_property_key_latin1(napi_env env, const char *str, size_t len, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if (str == NULL && len != 0) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_property_key_latin1(env, (const latin1_t *) str, len, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_symbol(napi_env env, napi_value description, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if (description != NULL) {
    napi_status status = napi_check_value_type(env, description, js_string, napi_string_expected);

    if (status != napi_ok) return status;
  }

  int err = js_create_symbol(env, description, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
node_api_symbol_for(napi_env env, const char *description, size_t len, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if (description == NULL && len != 0) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_symbol_for(env, description, len, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_object(napi_env env, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_object(env, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
node_api_create_object_with_properties(napi_env env, napi_value prototype, napi_value *property_names, napi_value *property_values, size_t property_count, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if ((property_names == NULL || property_values == NULL) && property_count > 0) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status;

  if (prototype == NULL) {
    int err = js_get_null(env, &prototype);

    if (err < 0) return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
  } else {
    js_value_type_t type;

    status = napi_get_value_type(env, prototype, &type);

    if (status != napi_ok) return status;

    if (type != js_object && type != js_function && type != js_null) return napi_set_last_error_info(env, napi_object_expected, 0, NULL);
  }

  for (size_t i = 0; i < property_count; i++) {
    status = napi_check_name(env, property_names[i]);

    if (status != napi_ok) return status;
  }

  int err = js_create_object_with_properties(env, prototype, (js_value_t *const *) property_names, (js_value_t *const *) property_values, property_count, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_function(napi_env env, const char *name, size_t len, napi_callback cb, void *data, napi_value *result) {
  if (env == NULL || cb == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_function(env, name, len, cb, data, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_array(napi_env env, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_array(env, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_array_with_length(napi_env env, size_t len, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_array_with_length(env, len, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_external(napi_env env, void *data, napi_finalize finalize_cb, void *finalize_hint, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_external(env, data, finalize_cb, finalize_hint, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_date(napi_env env, double time, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_date(env, time, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_error(napi_env env, napi_value code, napi_value message, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_error_args(env, code, message);

  if (status != napi_ok) return status;

  int err = js_create_error(env, code, message, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_type_error(napi_env env, napi_value code, napi_value message, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_error_args(env, code, message);

  if (status != napi_ok) return status;

  int err = js_create_type_error(env, code, message, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_range_error(napi_env env, napi_value code, napi_value message, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_error_args(env, code, message);

  if (status != napi_ok) return status;

  int err = js_create_range_error(env, code, message, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
node_api_create_syntax_error(napi_env env, napi_value code, napi_value message, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_error_args(env, code, message);

  if (status != napi_ok) return status;

  int err = js_create_syntax_error(env, code, message, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_promise(napi_env env, napi_deferred *deferred, napi_value *promise) {
  if (env == NULL || deferred == NULL || promise == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_promise(env, deferred, promise);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_resolve_deferred(napi_env env, napi_deferred deferred, napi_value resolution) {
  if (env == NULL || deferred == NULL || resolution == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_resolve_deferred(env, deferred, resolution);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_reject_deferred(napi_env env, napi_deferred deferred, napi_value resolution) {
  if (env == NULL || deferred == NULL || resolution == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_reject_deferred(env, deferred, resolution);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_arraybuffer(napi_env env, size_t len, void **data, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_arraybuffer(env, len, data, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_external_arraybuffer(napi_env env, void *data, size_t len, napi_finalize finalize_cb, void *finalize_hint, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_external_arraybuffer(env, data, len, finalize_cb, finalize_hint, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
node_api_create_sharedarraybuffer(napi_env env, size_t len, void **data, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_sharedarraybuffer(env, len, data, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
node_api_create_external_sharedarraybuffer(napi_env env, void *data, size_t len, napi_finalize finalize_cb, void *finalize_hint, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_create_external_sharedarraybuffer(env, data, len, finalize_cb, finalize_hint, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_detach_arraybuffer(napi_env env, napi_value arraybuffer) {
  if (env == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_object_type(env, arraybuffer, js_arraybuffer, napi_arraybuffer_expected);

  if (status != napi_ok) return status;

  bool detachable;

  int err = js_is_detachable_arraybuffer(env, arraybuffer, &detachable);

  if (err < 0) return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);

  if (!detachable) return napi_set_last_error_info(env, napi_detachable_arraybuffer_expected, 0, NULL);

  err = js_detach_arraybuffer(env, arraybuffer);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_typedarray(napi_env env, napi_typedarray_type type, size_t len, napi_value arraybuffer, size_t offset, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if ((unsigned) type > napi_float16_array) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  size_t byte_length;

  napi_status status = napi_get_arraybuffer_length(env, arraybuffer, &byte_length, napi_invalid_arg);

  if (status != napi_ok) return status;

  js_typedarray_type_t js_type = napi_convert_from_typedarray_type(type);

  size_t element_size = napi_typedarray_element_size(js_type);

  if (offset % element_size != 0) {
    return napi_throw_range_error_pending(env, "ERR_NAPI_INVALID_TYPEDARRAY_ALIGNMENT", "Start offset of typed array must be a multiple of its element size");
  }

  if (offset > byte_length || len > (byte_length - offset) / element_size) {
    return napi_throw_range_error_pending(env, "ERR_NAPI_INVALID_TYPEDARRAY_LENGTH", "Invalid typed array length");
  }

  int err = js_create_typedarray(env, js_type, len, arraybuffer, offset, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_buffer(napi_env env, size_t len, void **data, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  js_value_t *arraybuffer;

  int err = js_create_arraybuffer(env, len, data, &arraybuffer);

  if (err < 0) return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);

  err = js_create_typedarray(env, js_uint8array, len, arraybuffer, 0, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
node_api_create_buffer_from_arraybuffer(napi_env env, napi_value arraybuffer, size_t byte_offset, size_t byte_length, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  size_t arraybuffer_length;

  napi_status status = napi_get_arraybuffer_length(env, arraybuffer, &arraybuffer_length, napi_arraybuffer_expected);

  if (status != napi_ok) return status;

  if (byte_offset > arraybuffer_length || byte_length > arraybuffer_length - byte_offset) {
    return napi_throw_range_error_pending(env, "ERR_OUT_OF_RANGE", "The byte offset + length is out of range");
  }

  int err = js_create_typedarray(env, js_uint8array, byte_length, arraybuffer, byte_offset, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_buffer_copy(napi_env env, size_t len, const void *data, void **result_data, napi_value *result) {
  if (env == NULL || result == NULL || (data == NULL && len > 0)) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  js_value_t *arraybuffer;

  void *buffer_data;

  int err = js_create_arraybuffer(env, len, &buffer_data, &arraybuffer);

  if (err < 0) return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);

  memcpy(buffer_data, data, len);

  if (result_data) *result_data = buffer_data;

  err = js_create_typedarray(env, js_uint8array, len, arraybuffer, 0, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_external_buffer(napi_env env, size_t len, void *data, napi_finalize finalize_cb, void *finalize_hint, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  js_value_t *arraybuffer;

  int err = js_create_external_arraybuffer(env, data, len, finalize_cb, finalize_hint, &arraybuffer);

  if (err < 0) return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);

  err = js_create_typedarray(env, js_uint8array, len, arraybuffer, 0, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_dataview(napi_env env, size_t len, napi_value arraybuffer, size_t offset, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  size_t byte_length;

  napi_status status = napi_get_arraybuffer_length(env, arraybuffer, &byte_length, napi_invalid_arg);

  if (status != napi_ok) return status;

  if (offset > byte_length || len > byte_length - offset) {
    return napi_throw_range_error_pending(env, "ERR_NAPI_INVALID_DATAVIEW_ARGS", "Byte offset + length must not exceed the length of the array buffer");
  }

  int err = js_create_dataview(env, len, arraybuffer, offset, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_coerce_to_bool(napi_env env, napi_value value, napi_value *result) {
  if (env == NULL || value == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_coerce_to_boolean(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_coerce_to_number(napi_env env, napi_value value, napi_value *result) {
  if (env == NULL || value == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_coerce_to_number(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_coerce_to_string(napi_env env, napi_value value, napi_value *result) {
  if (env == NULL || value == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_coerce_to_string(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_coerce_to_object(napi_env env, napi_value value, napi_value *result) {
  if (env == NULL || value == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_coerce_to_object(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_typeof(napi_env env, napi_value value, napi_valuetype *result) {
  if (env == NULL || value == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  js_value_type_t js_type;

  int err = js_typeof(env, value, &js_type);

  if (err < 0) return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);

  *result = napi_convert_to_valuetype(js_type);

  return napi_clear_last_error_info(env);
}

napi_status
napi_is_array(napi_env env, napi_value value, bool *result) {
  if (env == NULL || value == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_is_array(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_is_date(napi_env env, napi_value value, bool *result) {
  if (env == NULL || value == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_is_date(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_is_error(napi_env env, napi_value value, bool *result) {
  if (env == NULL || value == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_is_error(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_is_promise(napi_env env, napi_value value, bool *result) {
  if (env == NULL || value == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_is_promise(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_is_arraybuffer(napi_env env, napi_value value, bool *result) {
  if (env == NULL || value == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_is_arraybuffer(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
node_api_is_sharedarraybuffer(napi_env env, napi_value value, bool *result) {
  if (env == NULL || value == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_is_sharedarraybuffer(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_is_detached_arraybuffer(napi_env env, napi_value value, bool *result) {
  if (env == NULL || value == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_is_detached_arraybuffer(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_is_typedarray(napi_env env, napi_value value, bool *result) {
  if (env == NULL || value == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_is_typedarray(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_is_buffer(napi_env env, napi_value value, bool *result) {
  if (env == NULL || value == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_is_typedarray(env, value, result);

  if (err < 0) return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);

  if (!*result) return napi_clear_last_error_info(env);

  js_typedarray_type_t type;
  err = js_get_typedarray_info(env, value, &type, NULL, NULL, NULL, NULL);

  if (err < 0) return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);

  *result = type == js_uint8array;

  return napi_clear_last_error_info(env);
}

napi_status
napi_is_dataview(napi_env env, napi_value value, bool *result) {
  if (env == NULL || value == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_is_dataview(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_strict_equals(napi_env env, napi_value a, napi_value b, bool *result) {
  if (env == NULL || a == NULL || b == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_strict_equals(env, a, b, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_instanceof(napi_env env, napi_value object, napi_value constructor, bool *result) {
  if (env == NULL || object == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_value_type(env, constructor, js_function, napi_function_expected);

  if (status != napi_ok) return status;

  int err = js_instanceof(env, object, constructor, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_global(napi_env env, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_get_global(env, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_undefined(napi_env env, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_get_undefined(env, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_null(napi_env env, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_get_null(env, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_boolean(napi_env env, bool value, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_get_boolean(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_value_bool(napi_env env, napi_value value, bool *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_value_type(env, value, js_boolean, napi_boolean_expected);

  if (status != napi_ok) return status;

  int err = js_get_value_bool(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_value_int32(napi_env env, napi_value value, int32_t *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_value_type(env, value, js_number, napi_number_expected);

  if (status != napi_ok) return status;

  int err = js_get_value_int32(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_value_uint32(napi_env env, napi_value value, uint32_t *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_value_type(env, value, js_number, napi_number_expected);

  if (status != napi_ok) return status;

  int err = js_get_value_uint32(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_value_int64(napi_env env, napi_value value, int64_t *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_value_type(env, value, js_number, napi_number_expected);

  if (status != napi_ok) return status;

  int err = js_get_value_int64(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_value_double(napi_env env, napi_value value, double *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_value_type(env, value, js_number, napi_number_expected);

  if (status != napi_ok) return status;

  int err = js_get_value_double(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_value_bigint_int64(napi_env env, napi_value value, int64_t *result, bool *lossless) {
  if (env == NULL || result == NULL || lossless == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_value_type(env, value, js_bigint, napi_bigint_expected);

  if (status != napi_ok) return status;

  int err = js_get_value_bigint_int64(env, value, result, lossless);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_value_bigint_uint64(napi_env env, napi_value value, uint64_t *result, bool *lossless) {
  if (env == NULL || result == NULL || lossless == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_value_type(env, value, js_bigint, napi_bigint_expected);

  if (status != napi_ok) return status;

  int err = js_get_value_bigint_uint64(env, value, result, lossless);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_value_bigint_words(napi_env env, napi_value value, int *sign_bit, size_t *word_count, uint64_t *words) {
  if (env == NULL || word_count == NULL || ((sign_bit == NULL) != (words == NULL))) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_value_type(env, value, js_bigint, napi_bigint_expected);

  if (status != napi_ok) return status;

  int err = js_get_value_bigint_words(env, value, sign_bit, words, *word_count, word_count);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_value_string_utf8(napi_env env, napi_value value, char *str, size_t len, size_t *result) {
  if (env == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_value_type(env, value, js_string, napi_string_expected);

  if (status != napi_ok) return status;

  if (str == NULL && result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_get_value_string_utf8(env, value, (utf8_t *) str, len > 0 ? len - 1 : 0, result);

  if (err < 0) return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);

  if (str != NULL && len > 0) str[len - 1] = '\0'; // Always NULL terminate

  return napi_clear_last_error_info(env);
}

napi_status
napi_get_value_string_utf16(napi_env env, napi_value value, char16_t *str, size_t len, size_t *result) {
  if (env == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_value_type(env, value, js_string, napi_string_expected);

  if (status != napi_ok) return status;

  if (str == NULL && result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_get_value_string_utf16le(env, value, str, len > 0 ? len - 1 : 0, result);

  if (err < 0) return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);

  if (str != NULL && len > 0) str[len - 1] = L'\0'; // Always NULL terminate

  return napi_clear_last_error_info(env);
}

napi_status
napi_get_value_string_latin1(napi_env env, napi_value value, char *str, size_t len, size_t *result) {
  if (env == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_value_type(env, value, js_string, napi_string_expected);

  if (status != napi_ok) return status;

  if (str == NULL && result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_get_value_string_latin1(env, value, (latin1_t *) str, len > 0 ? len - 1 : 0, result);

  if (err < 0) return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);

  if (str != NULL && len > 0) str[len - 1] = '\0'; // Always NULL terminate

  return napi_clear_last_error_info(env);
}

napi_status
napi_get_value_external(napi_env env, napi_value value, void **result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_value_type(env, value, js_external, napi_invalid_arg);

  if (status != napi_ok) return status;

  int err = js_get_value_external(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_date_value(napi_env env, napi_value value, double *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_object_type(env, value, js_date, napi_date_expected);

  if (status != napi_ok) return status;

  int err = js_get_value_date(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_array_length(napi_env env, napi_value value, uint32_t *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_object_type(env, value, js_array, napi_array_expected);

  if (status != napi_ok) return status;

  int err = js_get_array_length(env, value, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_property_names(napi_env env, napi_value object, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_to_object(env, object, &object);

  if (status != napi_ok) return status;

  int err = js_get_property_names(env, object, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_all_property_names(napi_env env, napi_value object, napi_key_collection_mode key_mode, napi_key_filter key_filter, napi_key_conversion key_conversion, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if (key_mode != napi_key_include_prototypes && key_mode != napi_key_own_only) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if (key_conversion != napi_key_keep_numbers && key_conversion != napi_key_numbers_to_strings) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if ((unsigned) key_filter & ~(unsigned) (napi_key_writable | napi_key_enumerable | napi_key_configurable | napi_key_skip_strings | napi_key_skip_symbols)) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_to_object(env, object, &object);

  if (status != napi_ok) return status;

  int err = js_get_filtered_property_names(env, object, napi_convert_from_key_collection_mode(key_mode), napi_convert_from_key_filter(key_filter), js_index_include_indices, napi_convert_from_key_conversion(key_conversion), result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_property(napi_env env, napi_value object, napi_value key, napi_value *result) {
  if (env == NULL || key == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_to_object(env, object, &object);

  if (status != napi_ok) return status;

  int err = js_get_property(env, object, key, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_prototype(napi_env env, napi_value object, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_to_object(env, object, &object);

  if (status != napi_ok) return status;

  int err = js_get_prototype(env, object, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
node_api_set_prototype(napi_env env, napi_value object, napi_value value) {
  if (env == NULL || value == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_to_object(env, object, &object);

  if (status != napi_ok) return status;

  int err = js_set_prototype(env, object, value);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_object_freeze(napi_env env, napi_value object) {
  if (env == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_to_object(env, object, &object);

  if (status != napi_ok) return status;

  int err = js_freeze(env, object);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_object_seal(napi_env env, napi_value object) {
  if (env == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_to_object(env, object, &object);

  if (status != napi_ok) return status;

  int err = js_seal(env, object);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_has_property(napi_env env, napi_value object, napi_value key, bool *result) {
  if (env == NULL || key == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_to_object(env, object, &object);

  if (status != napi_ok) return status;

  int err = js_has_property(env, object, key, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_has_own_property(napi_env env, napi_value object, napi_value key, bool *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_to_object(env, object, &object);

  if (status != napi_ok) return status;

  status = napi_check_name(env, key);

  if (status != napi_ok) return status;

  int err = js_has_own_property(env, object, key, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_set_property(napi_env env, napi_value object, napi_value key, napi_value value) {
  if (env == NULL || key == NULL || value == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_to_object(env, object, &object);

  if (status != napi_ok) return status;

  int err = js_set_property(env, object, key, value);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_delete_property(napi_env env, napi_value object, napi_value key, bool *result) {
  if (env == NULL || key == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_to_object(env, object, &object);

  if (status != napi_ok) return status;

  int err = js_delete_property(env, object, key, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_named_property(napi_env env, napi_value object, const char *name, napi_value *result) {
  if (env == NULL || name == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_to_object(env, object, &object);

  if (status != napi_ok) return status;

  int err = js_get_named_property(env, object, name, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_has_named_property(napi_env env, napi_value object, const char *name, bool *result) {
  if (env == NULL || name == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_to_object(env, object, &object);

  if (status != napi_ok) return status;

  int err = js_has_named_property(env, object, name, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_set_named_property(napi_env env, napi_value object, const char *name, napi_value value) {
  if (env == NULL || name == NULL || value == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_to_object(env, object, &object);

  if (status != napi_ok) return status;

  int err = js_set_named_property(env, object, name, value);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_element(napi_env env, napi_value object, uint32_t index, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_to_object(env, object, &object);

  if (status != napi_ok) return status;

  int err = js_get_element(env, object, index, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_has_element(napi_env env, napi_value object, uint32_t index, bool *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_to_object(env, object, &object);

  if (status != napi_ok) return status;

  int err = js_has_element(env, object, index, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_set_element(napi_env env, napi_value object, uint32_t index, napi_value value) {
  if (env == NULL || value == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_to_object(env, object, &object);

  if (status != napi_ok) return status;

  int err = js_set_element(env, object, index, value);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_delete_element(napi_env env, napi_value object, uint32_t index, bool *result) {
  if (env == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_to_object(env, object, &object);

  if (status != napi_ok) return status;

  int err = js_delete_element(env, object, index, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_cb_info(napi_env env, napi_callback_info info, size_t *argc, napi_value argv[], napi_value *receiver, void **data) {
  if (env == NULL || info == NULL || (argv != NULL && argc == NULL)) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_get_callback_info(env, info, argc, argv, receiver, data);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_new_target(napi_env env, napi_callback_info info, napi_value *result) {
  if (env == NULL || info == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_get_new_target(env, info, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_arraybuffer_info(napi_env env, napi_value arraybuffer, void **data, size_t *len) {
  if (env == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  js_object_type_t type;

  napi_status status = napi_get_object_type(env, arraybuffer, &type, napi_invalid_arg);

  if (status != napi_ok) return status;

  int err;

  switch (type) {
  case js_arraybuffer:
    err = js_get_arraybuffer_info(env, arraybuffer, data, len);
    break;
  case js_sharedarraybuffer:
    err = js_get_sharedarraybuffer_info(env, arraybuffer, data, len);
    break;
  default:
    return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);
  }

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_typedarray_info(napi_env env, napi_value typedarray, napi_typedarray_type *type, size_t *len, void **data, napi_value *arraybuffer, size_t *offset) {
  if (env == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_object_type(env, typedarray, js_typedarray, napi_invalid_arg);

  if (status != napi_ok) return status;

  js_typedarray_type_t js_type;

  int err = js_get_typedarray_info(env, typedarray, type == NULL ? NULL : &js_type, data, len, arraybuffer, offset);

  if (err < 0) return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);

  if (type != NULL) {
    *type = napi_convert_to_typedarray_type(js_type);
  }

  return napi_clear_last_error_info(env);
}

napi_status
napi_get_buffer_info(napi_env env, napi_value buffer, void **data, size_t *len) {
  if (env == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  js_object_type_t type;

  napi_status status = napi_get_object_type(env, buffer, &type, napi_invalid_arg);

  if (status != napi_ok) return status;

  if (type == js_dataview) {
    int err = js_get_dataview_info(env, buffer, data, len, NULL, NULL);

    return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
  }

  if (type != js_typedarray) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  js_typedarray_type_t js_type;

  int err = js_get_typedarray_info(env, buffer, &js_type, data, len, NULL, NULL);

  if (err < 0) return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);

  if (len) *len *= napi_typedarray_element_size(js_type);

  return napi_clear_last_error_info(env);
}

napi_status
napi_get_dataview_info(napi_env env, napi_value dataview, size_t *len, void **data, napi_value *arraybuffer, size_t *offset) {
  if (env == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_object_type(env, dataview, js_dataview, napi_invalid_arg);

  if (status != napi_ok) return status;

  int err = js_get_dataview_info(env, dataview, data, len, arraybuffer, offset);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_call_function(napi_env env, napi_value receiver, napi_value function, size_t argc, const napi_value argv[], napi_value *result) {
  if (env == NULL || receiver == NULL || (argc > 0 && argv == NULL)) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_value_type(env, function, js_function, napi_function_expected);

  if (status != napi_ok) return status;

  int err = js_call_function(env, receiver, function, argc, argv, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_make_callback(napi_env env, napi_async_context async_context, napi_value receiver, napi_value function, size_t argc, const napi_value argv[], napi_value *result) {
  if (env == NULL || receiver == NULL || (argc > 0 && argv == NULL)) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_value_type(env, function, js_function, napi_function_expected);

  if (status != napi_ok) return status;

  int err = js_call_function_with_checkpoint(env, receiver, function, argc, argv, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_new_instance(napi_env env, napi_value constructor, size_t argc, napi_value *argv, napi_value *result) {
  if (env == NULL || result == NULL || (argc > 0 && argv == NULL)) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_status status = napi_check_value_type(env, constructor, js_function, napi_function_expected);

  if (status != napi_ok) return status;

  int err = js_new_instance(env, constructor, argc, argv, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_throw(napi_env env, napi_value error) {
  if (env == NULL || error == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_throw(env, error);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_throw_error(napi_env env, const char *code, const char *message) {
  if (env == NULL || message == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_throw_error(env, code, message);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_throw_type_error(napi_env env, const char *code, const char *message) {
  if (env == NULL || message == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_throw_type_error(env, code, message);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_throw_range_error(napi_env env, const char *code, const char *message) {
  if (env == NULL || message == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_throw_range_error(env, code, message);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
node_api_throw_syntax_error(napi_env env, const char *code, const char *message) {
  if (env == NULL || message == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_throw_syntax_error(env, code, message);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_is_exception_pending(napi_env env, bool *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_is_exception_pending(env, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_and_clear_last_exception(napi_env env, napi_value *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  const napi_extended_error_info *info;

  napi_get_last_error_info(env, &info);

  if (info->engine_error_code == (uint32_t) js_uncaught_exception) {
    *result = NULL;

    return napi_clear_last_error_info(env);
  }

  int err = js_get_and_clear_last_exception(env, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_fatal_exception(napi_env env, napi_value error) {
  if (env == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if (error == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_fatal_exception(env, error);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

void
napi_fatal_error(const char *location, size_t location_len, const char *message, size_t message_len) {
  if (location_len == (size_t) -1) location_len = strlen(location);

  if (message_len == (size_t) -1) message_len = strlen(message);

  if (location) {
    fprintf(stderr, "FATAL ERROR: %.*s %.*s\n", (int) location_len, location, (int) message_len, message);
  } else {
    fprintf(stderr, "FATAL ERROR: %.*s\n", (int) message_len, message);
  }

  fflush(stderr);

  abort();
}

napi_status
napi_adjust_external_memory(napi_env env, int64_t change_in_bytes, int64_t *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_adjust_external_memory(env, change_in_bytes, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_create_threadsafe_function(napi_env env, napi_value function, napi_value async_resource, napi_value async_resource_name, size_t max_queue_size, size_t initial_thread_count, void *thread_finalize_data, napi_finalize thread_finalize_cb, void *context, napi_threadsafe_function_call_js cb, napi_threadsafe_function *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if (initial_thread_count == 0) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if (function == NULL) {
    if (cb == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);
  } else {
    napi_status status = napi_check_value_type(env, function, js_function, napi_function_expected);

    if (status != napi_ok) return status;
  }

  int err = js_create_threadsafe_function(env, function, max_queue_size, initial_thread_count, thread_finalize_cb, thread_finalize_data, context, cb, result);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_get_threadsafe_function_context(napi_threadsafe_function function, void **result) {
  if (function == NULL || result == NULL) return napi_invalid_arg;

  int err = js_get_threadsafe_function_context(function, result);

  return err == 0 ? napi_ok : napi_invalid_arg;
}

napi_status
napi_call_threadsafe_function(napi_threadsafe_function function, void *data, napi_threadsafe_function_call_mode mode) {
  if (function == NULL) return napi_invalid_arg;

  int err = js_call_threadsafe_function(function, data, napi_convert_from_threadsafe_function_call_mode(mode));

  return err == 0 ? napi_ok : napi_invalid_arg;
}

napi_status
napi_acquire_threadsafe_function(napi_threadsafe_function function) {
  if (function == NULL) return napi_invalid_arg;

  int err = js_acquire_threadsafe_function(function);

  return err == 0 ? napi_ok : napi_invalid_arg;
}

napi_status
napi_release_threadsafe_function(napi_threadsafe_function function, napi_threadsafe_function_release_mode mode) {
  if (function == NULL) return napi_invalid_arg;

  int err = js_release_threadsafe_function(function, napi_convert_from_threadsafe_function_release_mode(mode));

  return err == 0 ? napi_ok : napi_invalid_arg;
}

napi_status
napi_ref_threadsafe_function(napi_env env, napi_threadsafe_function function) {
  if (env == NULL || function == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_ref_threadsafe_function(env, function);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_unref_threadsafe_function(napi_env env, napi_threadsafe_function function) {
  if (env == NULL || function == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_unref_threadsafe_function(env, function);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}

napi_status
napi_add_env_cleanup_hook(napi_env env, napi_cleanup_hook callback, void *data) {
  if (env == NULL || callback == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_add_teardown_callback(env, callback, data);

  if (err != 0) abort();

  return napi_clear_last_error_info(env);
}

napi_status
napi_remove_env_cleanup_hook(napi_env env, napi_cleanup_hook callback, void *data) {
  if (env == NULL || callback == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_remove_teardown_callback(env, callback, data);

  if (err != 0) abort();

  return napi_clear_last_error_info(env);
}

napi_status
napi_add_async_cleanup_hook(napi_env env, napi_async_cleanup_hook callback, void *data, napi_async_cleanup_hook_handle *result) {
  if (env == NULL || callback == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = js_add_deferred_teardown_callback(env, callback, data, result);

  if (err != 0) abort();

  return napi_clear_last_error_info(env);
}

napi_status
napi_remove_async_cleanup_hook(napi_async_cleanup_hook_handle handle) {
  if (handle == NULL) return napi_invalid_arg;

  int err = js_finish_deferred_teardown_callback(handle);

  assert(err == 0);

  return napi_ok;
}

napi_status
napi_async_init(napi_env env, napi_value async_resource, napi_value async_resource_name, napi_async_context *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  *result = NULL;

  return napi_clear_last_error_info(env);
}

napi_status
napi_async_destroy(napi_env env, napi_async_context async_context) {
  if (env == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  return napi_clear_last_error_info(env);
}

napi_status
napi_open_callback_scope(napi_env env, napi_value resource, napi_async_context context, napi_callback_scope *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  *result = NULL;

  return napi_clear_last_error_info(env);
}

napi_status
napi_close_callback_scope(napi_env env, napi_callback_scope scope) {
  if (env == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  return napi_clear_last_error_info(env);
}

struct napi_async_work {
  uv_work_t handle;
  napi_env env;
  void *data;
  napi_async_execute_callback execute;
  napi_async_complete_callback complete;
};

napi_status
napi_create_async_work(napi_env env, napi_value async_resource, napi_value async_resource_name, napi_async_execute_callback execute, napi_async_complete_callback complete, void *data, napi_async_work *result) {
  if (env == NULL || result == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  if (execute == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_async_work work = malloc(sizeof(struct napi_async_work));

  work->env = env;
  work->data = data;
  work->execute = execute;
  work->complete = complete;

  *result = work;

  return napi_clear_last_error_info(env);
}

napi_status
napi_delete_async_work(napi_env env, napi_async_work work) {
  if (env == NULL || work == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  free(work);

  return napi_clear_last_error_info(env);
}

static void
napi__on_async_work(uv_work_t *handle) {
  napi_async_work work = (napi_async_work) handle;

  work->execute(work->env, work->data);
}

static void
napi__on_after_async_work(uv_work_t *handle, int status) {
  int err;

  napi_async_work work = (napi_async_work) handle;

  js_env_t *env = work->env;

  js_handle_scope_t *scope;
  err = js_open_handle_scope(env, &scope);
  assert(err == 0);

  work->complete(
    env,
    status == UV_ECANCELED ? napi_cancelled
    : status < 0           ? napi_generic_failure
                           : napi_ok,
    work->data
  );

  err = js_close_handle_scope(env, scope);
  assert(err == 0);
}

napi_status
napi_queue_async_work(napi_env env, napi_async_work work) {
  if (env == NULL || work == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err;

  uv_loop_t *loop;
  err = js_get_env_loop(env, &loop);
  assert(err == 0);

  err = uv_queue_work(loop, &work->handle, napi__on_async_work, napi__on_after_async_work);
  assert(err == 0);

  return napi_clear_last_error_info(env);
}

napi_status
napi_cancel_async_work(napi_env env, napi_async_work work) {
  if (env == NULL || work == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  int err = uv_cancel((uv_req_t *) &work->handle);

  return napi_set_last_error_info(
    env,
    err == UV_EINVAL      ? napi_invalid_arg
    : err == UV_ECANCELED ? napi_cancelled
    : err < 0             ? napi_generic_failure
                          : napi_ok,
    0,
    NULL
  );
}

typedef struct {
  napi_env env;
  napi_finalize finalize_cb;
  void *finalize_data;
  void *finalize_hint;
} napi_finalizer;

static void
napi__on_post_finalize(js_env_t *env, void *data) {
  napi_finalizer *finalizer = (napi_finalizer *) data;

  finalizer->finalize_cb(finalizer->env, finalizer->finalize_data, finalizer->finalize_hint);

  free(finalizer);
}

napi_status
node_api_post_finalizer(napi_env env, napi_finalize finalize_cb, void *finalize_data, void *finalize_hint) {
  if (env == NULL || finalize_cb == NULL) return napi_set_last_error_info(env, napi_invalid_arg, 0, NULL);

  napi_finalizer *finalizer = malloc(sizeof(napi_finalizer));

  finalizer->env = env;
  finalizer->finalize_cb = finalize_cb;
  finalizer->finalize_data = finalize_data;
  finalizer->finalize_hint = finalize_hint;

  int err = js_queue_microtask_with_callback(env, napi__on_post_finalize, finalizer);

  return napi_set_last_error_info(env, napi_convert_to_status(err), (uint32_t) err, NULL);
}
