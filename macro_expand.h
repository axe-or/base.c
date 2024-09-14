#pragma once

#include "prelude.h"
#include "memory.h"
#include "string.h"
#include "utf8.h"
#include "bytes_buffer.h"

typedef struct Macro_Symbol Macro_Symbol;
typedef struct Macro_Table Macro_Table;

struct Macro_Symbol {
	String key;
	String value;
};

struct Macro_Table {
	Macro_Symbol* symbols;
	isize symbol_slots;

	isize current;
	Bytes_Buffer buffer;

	Mem_Allocator allocator;
};

void macro_table_init(Macro_Table* table, Mem_Allocator allocator);

bool macro_table_set(Macro_Table* table, String key, String value);

bool macro_table_del(Macro_Table* table, String key, String value);

// void macro_table_destroy(Macro_Table* table);

// bool macro_table_expand(Macro_Table* table, String source);

// String macro_table_extract(Macro_Table* table, String source);

#ifdef BASE_C_IMPLEMENTATION

#endif
