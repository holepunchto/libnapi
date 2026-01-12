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

napi_status
napi_set_last_error_info(napi_env env, napi_status error_code, uint32_t engine_error_code, void *engine_reserved) {
  napi__extended_error_info.error_code = error_code;
  napi__extended_error_info.engine_error_code = engine_error_code;
  napi__extended_error_info.engine_reserved = engine_reserved;

  return error_code;
}

napi_status
napi_clear_last_error_info(napi_env env) {
  napi__extended_error_info.error_code = napi_ok;
  napi__extended_error_info.error_message = NULL;
  napi__extended_error_info.engine_error_code = 0;
  napi__extended_error_info.engine_reserved = NULL;

  return napi_ok;
}

napi_status
napi_get_last_error_info(napi_env env, const napi_extended_error_info **result) {
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
  *result = napi__instance_data.data;

  return napi_clear_last_error_info(env);
}

extern napi_status
napi_convert_to_status(int err);

extern js_value_type_t
napi_convert_from_valuetype(napi_valuetype type);

extern napi_valuetype
napi_convert_to_valuetype(js_value_type_t type);

extern js_typedarray_type_t
napi_convert_from_typedarray_type(napi_typedarray_type type);

extern napi_typedarray_type
napi_convert_to_typedarray_type(js_typedarray_type_t type);

extern js_key_collection_mode_t
napi_convert_from_key_collection_mode(napi_key_collection_mode mode);

extern js_property_filter_t
napi_convert_from_key_filter(napi_key_filter filter);

extern js_key_conversion_mode_t
napi_convert_from_key_conversion(napi_key_conversion mode);

extern js_threadsafe_function_release_mode_t
napi_convert_from_threadsafe_function_release_mode(napi_threadsafe_function_release_mode mode);

extern napi_threadsafe_function_release_mode
napi_convert_to_threadsafe_function_release_mode(js_threadsafe_function_release_mode_t mode);

extern js_threadsafe_function_call_mode_t
napi_convert_from_threadsafe_function_call_mode(napi_threadsafe_function_call_mode mode);

extern napi_threadsafe_function_call_mode
napi_convert_to_threadsafe_function_call_mode(js_threadsafe_function_call_mode_t mode);

extern napi_status
napi_get_uv_event_loop(napi_env env, uv_loop_t **loop);

extern napi_status
napi_open_handle_scope(napi_env env, napi_handle_scope *result);

extern napi_status
napi_close_handle_scope(napi_env env, napi_handle_scope scope);

extern napi_status
napi_open_escapable_handle_scope(napi_env env, napi_escapable_handle_scope *result);

extern napi_status
napi_close_escapable_handle_scope(napi_env env, napi_escapable_handle_scope scope);

extern napi_status
napi_escape_handle(napi_env env, napi_escapable_handle_scope scope, napi_value escapee, napi_value *result);

extern napi_status
napi_run_script(napi_env env, napi_value source, napi_value *result);

extern napi_status
napi_create_reference(napi_env env, napi_value value, uint32_t initial_refcount, napi_ref *result);

extern napi_status
napi_delete_reference(napi_env env, napi_ref ref);

extern napi_status
napi_reference_ref(napi_env env, napi_ref ref, uint32_t *result);

extern napi_status
napi_reference_unref(napi_env env, napi_ref ref, uint32_t *result);

extern napi_status
napi_get_reference_value(napi_env env, napi_ref ref, napi_value *result);

extern napi_status
napi_define_class(napi_env env, const char *name, size_t len, napi_callback constructor, void *data, size_t properties_len, const napi_property_descriptor *properties, napi_value *result);

extern napi_status
napi_define_properties(napi_env env, napi_value object, size_t len, const napi_property_descriptor *properties);

extern napi_status
napi_wrap(napi_env env, napi_value object, void *data, napi_finalize finalize_cb, void *finalize_hint, napi_ref *result);

extern napi_status
napi_unwrap(napi_env env, napi_value object, void **result);

extern napi_status
napi_remove_wrap(napi_env env, napi_value object, void **result);

extern napi_status
napi_add_finalizer(napi_env env, napi_value object, void *data, napi_finalize finalize_cb, void *finalize_hint, napi_ref *result);

extern napi_status
napi_type_tag_object(napi_env env, napi_value object, const napi_type_tag *tag);

extern napi_status
napi_check_object_type_tag(napi_env env, napi_value object, const napi_type_tag *tag, bool *result);

extern napi_status
napi_create_int32(napi_env env, int32_t value, napi_value *result);

extern napi_status
napi_create_uint32(napi_env env, uint32_t value, napi_value *result);

extern napi_status
napi_create_int64(napi_env env, int64_t value, napi_value *result);

extern napi_status
napi_create_double(napi_env env, double value, napi_value *result);

extern napi_status
napi_create_bigint_int64(napi_env env, int64_t value, napi_value *result);

extern napi_status
napi_create_bigint_uint64(napi_env env, uint64_t value, napi_value *result);

extern napi_status
napi_create_bigint_words(napi_env env, int sign_bit, size_t word_count, const uint64_t *words, napi_value *result);

extern napi_status
napi_create_string_utf8(napi_env env, const char *str, size_t len, napi_value *result);

extern napi_status
napi_create_string_utf16(napi_env env, const char16_t *str, size_t len, napi_value *result);

extern napi_status
napi_create_string_latin1(napi_env env, const char *str, size_t len, napi_value *result);

extern napi_status
napi_create_symbol(napi_env env, napi_value description, napi_value *result);

extern napi_status
napi_create_object(napi_env env, napi_value *result);

extern napi_status
napi_create_function(napi_env env, const char *name, size_t len, napi_callback cb, void *data, napi_value *result);

extern napi_status
napi_create_array(napi_env env, napi_value *result);

extern napi_status
napi_create_array_with_length(napi_env env, size_t len, napi_value *result);

extern napi_status
napi_create_external(napi_env env, void *data, napi_finalize finalize_cb, void *finalize_hint, napi_value *result);

extern napi_status
napi_create_date(napi_env env, double time, napi_value *result);

extern napi_status
napi_create_error(napi_env env, napi_value code, napi_value message, napi_value *result);

extern napi_status
napi_create_type_error(napi_env env, napi_value code, napi_value message, napi_value *result);

extern napi_status
napi_create_range_error(napi_env env, napi_value code, napi_value message, napi_value *result);

extern napi_status
napi_create_promise(napi_env env, napi_deferred *deferred, napi_value *promise);

extern napi_status
napi_resolve_deferred(napi_env env, napi_deferred deferred, napi_value resolution);

extern napi_status
napi_reject_deferred(napi_env env, napi_deferred deferred, napi_value resolution);

extern napi_status
napi_create_arraybuffer(napi_env env, size_t len, void **data, napi_value *result);

extern napi_status
napi_create_external_arraybuffer(napi_env env, void *data, size_t len, napi_finalize finalize_cb, void *finalize_hint, napi_value *result);

extern napi_status
napi_detach_arraybuffer(napi_env env, napi_value arraybuffer);

extern napi_status
napi_create_typedarray(napi_env env, napi_typedarray_type type, size_t len, napi_value arraybuffer, size_t offset, napi_value *result);

extern napi_status
napi_create_buffer(napi_env env, size_t len, void **data, napi_value *result);

extern napi_status
napi_create_buffer_copy(napi_env env, size_t len, const void *data, void **result_data, napi_value *result);

extern napi_status
napi_create_external_buffer(napi_env env, size_t len, void *data, napi_finalize finalize_cb, void *finalize_hint, napi_value *result);

extern napi_status
napi_create_dataview(napi_env env, size_t len, napi_value arraybuffer, size_t offset, napi_value *result);

extern napi_status
napi_coerce_to_bool(napi_env env, napi_value value, napi_value *result);

extern napi_status
napi_coerce_to_number(napi_env env, napi_value value, napi_value *result);

extern napi_status
napi_coerce_to_string(napi_env env, napi_value value, napi_value *result);

extern napi_status
napi_coerce_to_object(napi_env env, napi_value value, napi_value *result);

extern napi_status
napi_typeof(napi_env env, napi_value value, napi_valuetype *result);

extern napi_status
napi_is_array(napi_env env, napi_value value, bool *result);

extern napi_status
napi_is_date(napi_env env, napi_value value, bool *result);

extern napi_status
napi_is_error(napi_env env, napi_value value, bool *result);

extern napi_status
napi_is_promise(napi_env env, napi_value value, bool *result);

extern napi_status
napi_is_arraybuffer(napi_env env, napi_value value, bool *result);

extern napi_status
napi_is_detached_arraybuffer(napi_env env, napi_value value, bool *result);

extern napi_status
napi_is_typedarray(napi_env env, napi_value value, bool *result);

extern napi_status
napi_is_buffer(napi_env env, napi_value value, bool *result);

extern napi_status
napi_is_dataview(napi_env env, napi_value value, bool *result);

extern napi_status
napi_strict_equals(napi_env env, napi_value a, napi_value b, bool *result);

extern napi_status
napi_get_global(napi_env env, napi_value *result);

extern napi_status
napi_get_undefined(napi_env env, napi_value *result);

extern napi_status
napi_get_null(napi_env env, napi_value *result);

extern napi_status
napi_get_boolean(napi_env env, bool value, napi_value *result);

extern napi_status
napi_get_value_bool(napi_env env, napi_value value, bool *result);

extern napi_status
napi_get_value_int32(napi_env env, napi_value value, int32_t *result);

extern napi_status
napi_get_value_uint32(napi_env env, napi_value value, uint32_t *result);

extern napi_status
napi_get_value_int64(napi_env env, napi_value value, int64_t *result);

extern napi_status
napi_get_value_double(napi_env env, napi_value value, double *result);

extern napi_status
napi_get_value_bigint_int64(napi_env env, napi_value value, int64_t *result, bool *lossless);

extern napi_status
napi_get_value_bigint_uint64(napi_env env, napi_value value, uint64_t *result, bool *lossless);

extern napi_status
napi_get_value_bigint_words(napi_env env, napi_value value, int *sign_bit, size_t *word_count, uint64_t *words);

extern napi_status
napi_get_value_string_utf8(napi_env env, napi_value value, char *str, size_t len, size_t *result);

extern napi_status
napi_get_value_string_utf16(napi_env env, napi_value value, char16_t *str, size_t len, size_t *result);

extern napi_status
napi_get_value_string_latin1(napi_env env, napi_value value, char *str, size_t len, size_t *result);

extern napi_status
napi_get_value_external(napi_env env, napi_value value, void **result);

extern napi_status
napi_get_value_date(napi_env env, napi_value value, double *result);

extern napi_status
napi_get_array_length(napi_env env, napi_value value, uint32_t *result);

extern napi_status
napi_get_property(napi_env env, napi_value object, napi_value key, napi_value *result);

extern napi_status
napi_get_property_names(napi_env env, napi_value object, napi_value *result);

extern napi_status
napi_get_all_property_names(napi_env env, napi_value object, napi_key_collection_mode key_mode, napi_key_filter key_filter, napi_key_conversion key_conversion, napi_value *result);

extern napi_status
napi_has_property(napi_env env, napi_value object, napi_value key, bool *result);

extern napi_status
napi_has_own_property(napi_env env, napi_value object, napi_value key, bool *result);

extern napi_status
napi_set_property(napi_env env, napi_value object, napi_value key, napi_value value);

extern napi_status
napi_delete_property(napi_env env, napi_value object, napi_value key, bool *result);

extern napi_status
napi_get_named_property(napi_env env, napi_value object, const char *name, napi_value *result);

extern napi_status
napi_has_named_property(napi_env env, napi_value object, const char *name, bool *result);

extern napi_status
napi_set_named_property(napi_env env, napi_value object, const char *name, napi_value value);

extern napi_status
napi_get_element(napi_env env, napi_value object, uint32_t index, napi_value *result);

extern napi_status
napi_has_element(napi_env env, napi_value object, uint32_t index, bool *result);

extern napi_status
napi_set_element(napi_env env, napi_value object, uint32_t index, napi_value value);

extern napi_status
napi_delete_element(napi_env env, napi_value object, uint32_t index, bool *result);

extern napi_status
napi_get_cb_info(napi_env env, napi_callback_info info, size_t *argc, napi_value argv[], napi_value *self, void **data);

extern napi_status
napi_get_new_target(napi_env env, napi_callback_info info, napi_value *result);

extern napi_status
napi_get_arraybuffer_info(napi_env env, napi_value arraybuffer, void **data, size_t *len);

extern napi_status
napi_get_typedarray_info(napi_env env, napi_value typedarray, napi_typedarray_type *type, size_t *len, void **data, napi_value *arraybuffer, size_t *offset);

extern napi_status
napi_get_buffer_info(napi_env env, napi_value buffer, void **data, size_t *len);

extern napi_status
napi_get_dataview_info(napi_env env, napi_value dataview, size_t *len, void **data, napi_value *arraybuffer, size_t *offset);

extern napi_status
napi_call_function(napi_env env, napi_value recv, napi_value fn, size_t argc, const napi_value argv[], napi_value *result);

extern napi_status
napi_make_callback(napi_env env, napi_async_context async_hook, napi_value recv, napi_value fn, size_t argc, const napi_value argv[], napi_value *result);

extern napi_status
napi_new_instance(napi_env env, napi_value constructor, size_t argc, napi_value *argv, napi_value *result);

extern napi_status
napi_throw(napi_env env, napi_value error);

extern napi_status
napi_throw_error(napi_env env, const char *code, const char *msg);

extern napi_status
napi_throw_type_error(napi_env env, const char *code, const char *message);

extern napi_status
napi_throw_range_error(napi_env env, const char *code, const char *message);

extern napi_status
napi_is_exception_pending(napi_env env, bool *result);

extern napi_status
napi_get_and_clear_last_exception(napi_env env, napi_value *result);

extern napi_status
napi_fatal_exception(napi_env env, napi_value err);

extern void
napi_fatal_error(const char *location, size_t location_len, const char *message, size_t message_len);

extern napi_status
napi_adjust_external_memory(napi_env env, int64_t change_in_bytes, int64_t *result);

extern napi_status
napi_create_threadsafe_function(napi_env env, napi_value function, napi_value async_resource, napi_value async_resource_name, size_t max_queue_size, size_t initial_thread_count, void *thread_finalize_data, napi_finalize thread_finalize_cb, void *context, napi_threadsafe_function_call_js cb, napi_threadsafe_function *result);

extern napi_status
napi_get_threadsafe_function_context(napi_threadsafe_function function, void **result);

extern napi_status
napi_call_threadsafe_function(napi_threadsafe_function function, void *data, napi_threadsafe_function_call_mode mode);

extern napi_status
napi_acquire_threadsafe_function(napi_threadsafe_function function);

extern napi_status
napi_release_threadsafe_function(napi_threadsafe_function function, napi_threadsafe_function_release_mode mode);

extern napi_status
napi_ref_threadsafe_function(napi_env env, napi_threadsafe_function function);

extern napi_status
napi_unref_threadsafe_function(napi_env env, napi_threadsafe_function function);

extern napi_status
napi_add_env_cleanup_hook(napi_env env, napi_cleanup_hook callback, void *data);

extern napi_status
napi_remove_env_cleanup_hook(napi_env env, napi_cleanup_hook callback, void *data);

extern napi_status
napi_add_async_cleanup_hook(napi_env env, napi_async_cleanup_hook callback, void *data, napi_async_cleanup_hook_handle *result);

extern napi_status
napi_remove_async_cleanup_hook(napi_async_cleanup_hook_handle handle);

extern napi_status
napi_async_init(napi_env env, napi_value async_resource, napi_value async_resource_name, napi_async_context *result);

extern napi_status
napi_async_destroy(napi_env env, napi_async_context async_context);

extern napi_status
napi_open_callback_scope(napi_env env, napi_value resource, napi_async_context context, napi_callback_scope *result);

extern napi_status
napi_close_callback_scope(napi_env env, napi_callback_scope scope);

struct napi_async_work {
  uv_work_t handle;
  napi_env env;
  void *data;
  napi_async_execute_callback execute;
  napi_async_complete_callback complete;
};

napi_status
napi_create_async_work(napi_env env, napi_value async_resource, napi_value async_resource_name, napi_async_execute_callback execute, napi_async_complete_callback complete, void *data, napi_async_work *result) {
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
