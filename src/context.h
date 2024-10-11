/* Discover more about the current build context */
#pragma once

#include "./prelude.h"

#define CTX_COMPILER_VENDORS \
	X(Unknown, "(unknown)", 0) \
	X(TCC    , "TCC", 1) \
	X(GCC    , "GCC", 2) \
	X(Clang  , "Clang", 3) \
	X(MSVC   , "MSVC", 4)

enum Ctx_Compiler_Vendor {
	#define X(name, _, val) Ctx_CC_##name = val,
	CTX_COMPILER_VENDORS
	#undef X
};

// The order is very important especially for choosing between clang and gcc
#if defined(__TINYC__)
	#define CTX_CC_VENDOR Ctx_CC_TCC
	#define CTX_CC_TCC 1
#elif defined(__clang__)
	#define CTX_CC_VENDOR Ctx_CC_Clang
	#define CTX_CC_CLANG 1
#elif defined(__GNUC__) || defined(__GNUG__)
	#define CTX_CC_VENDOR Ctx_CC_GCC
	#define CTX_CC_GCC 1
#elif defined(_MSC_VER)
	#define CTX_CC_VENDOR Ctx_CC_GCC
	#define CTX_CC_MSVC 1
#else
	#define CTX_CC_VENDOR Ctx_CC_Unknown
#endif

#define context_compiler_vendor ((enum Ctx_Compiler_Vendor)CTX_CC_VENDOR)

static char const * const context_compiler_vendor_names[] = {
	#define X(name, string, _) [Ctx_CC_##name] = string,
	CTX_COMPILER_VENDORS
	#undef X
};

enum Ctx_Endianess {
	Ctx_Endian_Big = 0x0000beef,
	Ctx_Endian_Little = 0xefbe0000,
};

static const union {
	byte b[4];
	u32 value;
} context_endianness_impl = {{0x00, 0x00, 0xbe, 0xef}};

#define context_endianness ((enum Ctx_Endianess)context_endianness_impl.value)

#undef CTX_COMPILER_VENDORS
