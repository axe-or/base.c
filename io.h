#pragma once

#include "prelude.h"

typedef isize (*IO_Func)(void* impl, byte* data, isize len);

typedef struct {
	void* impl;
	IO_Func func;
} IO_Writer;

typedef struct {
	void* impl;
	IO_Func func;
} IO_Reader;

typedef enum {
	IO_Err_None = 0,
	IO_Err_End = -1,
	IO_Err_Closed = -2,
	IO_Err_Out_Of_Memory = -3,

	IO_Err_Unknown = -127,
} IO_Error;

// Read into buffer, returns number of bytes read into buffer. On error returns
// the negative valued IO_Error code.
isize io_read(IO_Reader r, byte* buf, isize buflen);

// Write into buffer, returns number of bytes written to buffer. On error returns
// the negative valued IO_Error code.
isize io_write(IO_Writer r, byte* buf, isize buflen);

