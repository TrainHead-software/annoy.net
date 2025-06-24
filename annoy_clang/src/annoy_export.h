#pragma once

#ifdef _WIN32
  #ifdef BUILDING_ANNOY_C_DLL
    #define ANNOY_API __declspec(dllexport)
  #else
    #define ANNOY_API __declspec(dllimport)
  #endif
#else
  #define ANNOY_API
#endif
