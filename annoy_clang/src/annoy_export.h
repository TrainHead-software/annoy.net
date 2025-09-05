#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
  #ifdef BUILDING_ANNOY_C_DLL
    #define ANNOY_API __declspec(dllexport)
  #else
    #define ANNOY_API __declspec(dllimport)
  #endif
#elif defined(__GNUC__) || defined(__clang__)
  // Ensure functions are visible when building shared library on Unix-like systems
  #define ANNOY_API __attribute__((visibility("default")))
#else
  #define ANNOY_API
#endif

