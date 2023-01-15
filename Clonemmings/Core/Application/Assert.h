#pragma once
#include "Core/Log/Log.h"
#include <filesystem>
#define EXPAND_MACRO(x) x
#define STRINGIFY_MACRO(x) #x
#ifndef DEBUG
#define ASSERT(...)
#else
#define INTERNAL_ASSERT_IMPL(type, check, msg, ...) {if(!(check)) {LOGERROR(msg,__VA_ARGS__); __debugbreak(); }}
#define INTERNAL_ASSERT_MSG(type, check, ...) INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define INTERNAL_ASSERT_NO_MSG(type, check) INTERNAL_ASSERT_IMPL(type, check, "Assertion failed '{0}' at {1}:{2}",STRINGIFY_MACRO(check),std::filesystem::path(__FILE__).filename().string(),__LINE__)
#define INTERNAL_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define INTERNAL_GET_MACRO(...) EXPAND_MACRO(INTERNAL_GET_MACRO_NAME(__VA_ARGS__,INTERNAL_ASSERT_MSG, INTERNAL_ASSERT_NO_MSG))
#define ASSERT(...) EXPAND_MACRO(INTERNAL_GET_MACRO(__VA_ARGS__)(_,__VA_ARGS__))
#endif
