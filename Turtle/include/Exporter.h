#pragma once
#include "Cxx_Core.h"

using CXX::NativeClass;

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define Q_DECL_EXPORT __declspec(dllexport)
#define Q_DECL_IMPORT __declspec(dllimport)
#else
#define Q_DECL_EXPORT __attribute__((visibility("default")))
#define Q_DECL_IMPORT __attribute__((visibility("default")))
#endif

#ifdef EXT_EXPORT
#define EXTAPI Q_DECL_EXPORT
#else
#define EXTAPI Q_DECL_IMPORT
#endif

extern "C" {
	EXTAPI NativeClass* getClass_0();
	EXTAPI const char* getClassName_0();
	EXTAPI NativeClass* getClass_1();
	EXTAPI const char* getClassName_1();
}