#include "./fnv_hash.h"

i32 hash_fnv32_ex(void const* data, isize len, i32 seed){
	i32 h = seed;
	byte const* buf = (byte const*)data;
	for(isize i = 0; i < len; i ++){
		h = h ^ buf[i];
		h = h * 0x01000193;
	}
	return h;
}

i64 hash_fnv64_ex(void const* data, isize len, i64 seed){
	i64 h = seed;
	byte const* buf = (byte const*)data;
	for(isize i = 0; i < len; i ++){
		h = h ^ buf[i];
		h = h * 0x00000100000001b3;
	}
	return h;
}

i64 hash_fnv64(void const* data, isize len){
	return hash_fnv64_ex(data, len, (i32)0x811c9dc5);
}

i32 hash_fnv32(void const* data, isize len){
	return hash_fnv64_ex(data, len, (i64)0xcbf29ce484222325);
}
