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
	#define X(name, _, val) CC_##name = val,
	CTX_COMPILER_VENDORS
	#undef X
};

// The order is very important especially for choosing between clang and gcc
#if defined(__TINY_C__)
	#define CTX_CC_VENDOR CC_TCC
#elif defined(__clang__)
	#define CTX_CC_VENDOR CC_Clang
#elif defined(__GNUC__) || defined(__GNUG__)
	#define CTX_CC_VENDOR CC_GCC
#elif defined(_MSC_VER)
	#define CTX_CC_VENDOR CC_MSVC
#else
	#define CTX_CC_VENDOR CC_Unknown
#endif

static const enum Ctx_Compiler_Vendor context_compiler_vendor = CTX_CC_VENDOR ;

static char const * const context_compiler_vendor_names[] = {
	#define X(name, string, _) [CC_##name] = string,
	CTX_COMPILER_VENDORS
	#undef X
};

enum Ctx_Endianess {
	Endian_Big = 0x0000beef,
	Endian_Little = 0xefbe0000,
};

static const union {
	byte b[4];
	u32 value;
} context_endianness_impl = {{0x00, 0x00, 0xbe, 0xef}};

#define context_endianness ((enum Ctx_Endianess)(context_endianness_impl.value))

#undef CTX_CC_VENDOR
#undef CTX_COMPILER_VENDORS

