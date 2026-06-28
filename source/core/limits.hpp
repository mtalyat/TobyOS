#pragma once

#include "types/types.hpp"

constexpr int8 INT8_MIN = -128;
constexpr int8 INT8_MAX = 127;
constexpr int16 INT16_MIN = -32768;
constexpr int16 INT16_MAX = 32767;
constexpr int32 INT32_MIN = -2147483648;
constexpr int32 INT32_MAX = 2147483647;
constexpr int64 INT64_MIN = -9223372036854775807LL - 1;
constexpr int64 INT64_MAX = 9223372036854775807LL;

constexpr uint8 UINT8_MAX = 255;
constexpr uint16 UINT16_MAX = 65535;
constexpr uint32 UINT32_MAX = 4294967295;
constexpr uint64 UINT64_MAX = 18446744073709551615ULL;