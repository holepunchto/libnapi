#include "../include/napi.h"

napi_status
napi_open_handle_scope (napi_env env, napi_handle_scope *result) {
  js_open_handle_scope(env, result);
  return napi_ok;
}

napi_status
napi_close_handle_scope (napi_env env, napi_handle_scope scope) {
  js_close_handle_scope(env, scope);
  return napi_ok;
}

napi_status
napi_open_escapable_handle_scope (napi_env env, napi_escapable_handle_scope *result) {
  js_open_escapable_handle_scope(env, result);
  return napi_ok;
}

napi_status
napi_close_escapable_handle_scope (napi_env env, napi_escapable_handle_scope scope) {
  js_close_escapable_handle_scope(env, scope);
  return napi_ok;
}

napi_status
napi_escape_handle (napi_env env, napi_escapable_handle_scope scope, napi_value escapee, napi_value *result) {
  js_escape_handle(env, scope, escapee, result);
  return napi_ok;
}

napi_status
napi_create_reference (napi_env env, napi_value value, uint32_t initial_refcount, napi_ref *result) {
  js_create_reference(env, value, initial_refcount, result);
  return napi_ok;
}

napi_status
napi_delete_reference (napi_env env, napi_ref ref) {
  js_delete_reference(env, ref);
  return napi_ok;
}

napi_status
napi_reference_ref (napi_env env, napi_ref ref, uint32_t *result) {
  js_reference_ref(env, ref, result);
  return napi_ok;
}

napi_status
napi_reference_unref (napi_env env, napi_ref ref, uint32_t *result) {
  js_reference_unref(env, ref, result);
  return napi_ok;
}

napi_status
napi_get_reference_value (napi_env env, napi_ref ref, napi_value *result) {
  js_get_reference_value(env, ref, result);
  return napi_ok;
}

napi_status
napi_throw (napi_env env, napi_value error) {
  js_throw(env, error);
  return napi_ok;
}
