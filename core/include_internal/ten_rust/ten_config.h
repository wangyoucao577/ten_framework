//
// This file is part of the TEN Framework project.
// See https://github.com/TEN-framework/ten_framework/LICENSE for license
// information.
//
#pragma once

#ifdef __cplusplus
  #define TEN_EXTERN_C extern "C"
#else
  #define TEN_EXTERN_C extern
#endif

#if defined(_WIN32)

  #if defined(TEN_RUST_EXPORT)
    #if !defined(TEN_RUST_API)
      #define TEN_RUST_API TEN_EXTERN_C __declspec(dllexport)
    #endif
  #else
    #if !defined(TEN_RUST_API)
      #define TEN_RUST_API TEN_EXTERN_C __declspec(dllimport)
    #endif
  #endif

  #if !defined(TEN_RUST_PRIVATE_API)
    #define TEN_RUST_PRIVATE_API TEN_EXTERN_C
  #endif

#elif defined(__APPLE__)

  #include <TargetConditionals.h>

  #if !defined(TEN_RUST_API)
    #define TEN_RUST_API __attribute__((visibility("default"))) TEN_EXTERN_C
  #endif

  #if !defined(TEN_RUST_PRIVATE_API)
    #define TEN_RUST_PRIVATE_API \
      __attribute__((visibility("hidden"))) TEN_EXTERN_C
  #endif

#elif defined(__linux__)

  #if !defined(TEN_RUST_API)
    #define TEN_RUST_API TEN_EXTERN_C __attribute__((visibility("default")))
  #endif

  #if !defined(TEN_RUST_PRIVATE_API)
    #define TEN_RUST_PRIVATE_API \
      TEN_EXTERN_C __attribute__((visibility("hidden")))
  #endif

#else

  #if !defined(TEN_RUST_API)
    #define TEN_RUST_API TEN_EXTERN_C
  #endif

  #if !defined(TEN_RUST_PRIVATE_API)
    #define TEN_RUST_PRIVATE_API TEN_EXTERN_C
  #endif

#endif