#pragma once

#include "prelude.h"
#include "memory.h"
#include "string.h"

typedef struct Dict_Slot Dict_Slot;

typedef struct {
    String data;
    Dict_Slot* next;
} Dict_Slot;

typedef struct {
    Dict_Slot* slots;
    isize slot_count;
    Mem_Allocator allocator;
} Dictionary;

bool dict_init(isize capacity){
}

bool dict_get(String key);

bool dict_set(String key, String value);

String* dict_get_ptr(String key);

#ifdef BASE_C_IMPLEMENTATION
#endif