#pragma once

#include "prelude.h"

typedef enum {
	IO_Op_Query = 0,
	IO_Op_Read  = 1 << 0,
	IO_Op_Write = 1 << 1,
} IO_Operation;

typedef isize (*IO_Func)(void* impl, IO_Operation op, byte* data, isize len);

typedef struct {
	void* impl;
	IO_Func func;
} IO_Stream;

typedef struct { IO_Stream _stream; } IO_Writer;

typedef struct { IO_Stream _stream; } IO_Reader;

typedef enum {
	IO_Err_None = 0,
	IO_Err_End = -1,
	IO_Err_Closed = -2,
	IO_Err_Out_Of_Memory = -3,
	IO_Socket_Error = -4,
	IO_Err_Unknown = -127,
} IO_Error;


// TODO: Use _Generic for better ergonomics

// Read into buffer, returns number of bytes read into buffer. On error returns
// the negative valued IO_Error code.
isize io_read(IO_Reader r, byte* buf, isize buflen);

// Write into buffer, returns number of bytes written to buffer. On error returns
// the negative valued IO_Error code.
isize io_write(IO_Writer r, byte const* buf, isize buflen);

// Query the capabilites of the IO stream as per IO_Operation
i8 io_query_stream(IO_Stream s);

// Cast a stream to a IO reader, in debug mode this uses a query to assert that
// the stream supports reading
IO_Reader io_to_reader(IO_Stream s);

// Cast a stream to a IO writer, in debug mode this uses a query to assert that
// the stream supports writing
IO_Writer io_to_writer(IO_Stream s);
