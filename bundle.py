#!/usr/bin/env python

import zipfile as zf

files = [
    'arena_allocator.h',
    'assert.h',
    'bytes_buffer.h',
    'heap_allocator.h',
    'io.h',
    'mem.h',
    'prelude.h',
    'string.h',
    'testing.h',
    'utf8.h',
]

OUT_ARCHIVE = 'libc2.zip'
OUT_SRC = 'libc2.c'
with open(OUT_SRC, 'w') as f:
    source =  ["/* Convenience file to have a translation unit that implements all of libc2's utilities. */"]
    source += ['#define LIBC2_IMPLEMENTATION 1']
    source += [f'#include "{name}"' for name in files]
    source += ['#undef LIBC2_IMPLEMENTATION']
    n = f.write('\n'.join(source) + '\n')
    print(f'Wrote {n}B to {OUT_SRC}')


ROOT = 'libc2'
include_dir = f'{ROOT}/include'
src_dir = f'{ROOT}/src'

with zf.ZipFile(OUT_ARCHIVE, 'w', compression=zf.ZIP_LZMA) as zfile:
    zfile.mkdir(ROOT)
    zfile.mkdir(include_dir)
    zfile.mkdir(src_dir)
    for f in files:
        zfile.write(f, f'{include_dir}/{f}')

    zfile.write(OUT_SRC, f'{src_dir}/{OUT_SRC}')
    print(f'Created {OUT_ARCHIVE}')
