#include "./io.h"
#include "./assert.h"

i8 io_query_stream(IO_Stream s){
	return s.func(s.impl, IO_Op_Query, NULL, 0);
}

isize io_read(IO_Reader r, byte* buf, isize buflen){
	IO_Stream s = r._stream;
	return s.func(s.impl, IO_Op_Read, buf, buflen);
}

isize io_write(IO_Writer w, byte const* buf, isize buflen){
	IO_Stream s = w._stream;
	return s.func(s.impl, IO_Op_Write, (byte*)(buf), buflen);
}

IO_Reader io_to_reader(IO_Stream s){
	i8 cap = io_query_stream(s);
	debug_assert(cap & IO_Op_Read, "Stream does not support reading.");
	IO_Reader r = { ._stream = s };
	return r;
}

IO_Writer io_to_writer(IO_Stream s){
	i8 cap = io_query_stream(s);
	debug_assert(cap & IO_Op_Write, "Stream does not support writing.");
	IO_Writer w = { ._stream = s };
	return w;
}
