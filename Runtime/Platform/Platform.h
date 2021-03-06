#pragma once
#include <stdint.h>
namespace Joestar {
typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef int64_t I64;
typedef int32_t I32;
typedef int16_t I16;
typedef int8_t I8;
const U32 U32_MAX = UINT32_MAX;
const U64 U64_MAX = UINT64_MAX;
typedef float F32;
typedef double F64;
#define INDEX_NONE -1
#define M_EPSILON 1e-6
}