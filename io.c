#include "io.h"

isize io_read(IO_Reader r, byte* buf, isize buflen){
	return r.func(r.impl, buf, buflen);
}

isize io_write(IO_Writer r, byte* buf, isize buflen){
	return r.func(r.impl, buf, buflen);
}

