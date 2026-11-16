if(TARGET napi_shared)
  set(library napi::shared)
else()
  set(library napi::static)
endif()

add_test_executables(
  basic.c
  callback-uncaught-exception.c
  cleanup-hook-errors.c
  create-async-work.c
  create-async-work-without-complete.c
  create-buffer-copy.c
  create-buffer-from-arraybuffer.c
  create-view-out-of-range.c
  define-properties-const-descriptors.c
  get-buffer-info-uint8array.c
  get-buffer-info-uint16array.c
  get-info-type-mismatch.c
  get-instance-data.c
  get-all-property-names-filter.c
  get-version.c
  get-value-string-utf8.c
  get-value-string-utf8-length.c
  get-value-string-utf8-truncate.c
  get-value-string-null-buffer.c
  get-value-type-mismatch.c
  invalid-args.c
  module.c
  module-constructor.c
  null-args.c
  object-freeze.c
  object-seal.c
  object-type-mismatch.c
  post-finalizer.c
  run-script-uncaught-exception.c
  LINK
    ${library}
)
