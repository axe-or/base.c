#pragma once
#include "prelude.h"

// Hash data using FNV-1a hash (32-bit)
i32 hash_fnv32_ex(void const* data, isize len, i32 seed);

// Hash data using FNV-1a hash (64-bit)
i64 hash_fnv64_ex(void const* data, isize len, i64 seed);

// Hash data using FNV-1a hash (32-bit)
i32 hash_fnv32(void const* data, isize len);

// Hash data using FNV-1a hash (64-bit)
i64 hash_fnv64(void const* data, isize len);
