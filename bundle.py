#!/usr/bin/env python

import zipfile as zf
import subprocess as sb
from os import remove

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

VERSION_FILE = 'version.txt'
revision = sb.check_output(["git", "rev-parse", "--short=16", "HEAD"]).decode('utf-8')
with open(VERSION_FILE, 'w') as f:
    f.write(revision + '\n')

with open(OUT_SRC, 'w') as f:
    source =  ["/* Convenience file to have a translation unit that implements all of libc2's utilities. */"]
    source += ['#define LIBC2_IMPLEMENTATION 1']
    source += [f'#include "{name}"' for name in files]
    source += ['#undef LIBC2_IMPLEMENTATION']
    n = f.write('\n'.join(source) + '\n')
    print(f'Wrote {n}B to {OUT_SRC}')

ROOT = 'libc2'

with zf.ZipFile(OUT_ARCHIVE, 'w', compression=zf.ZIP_LZMA, compresslevel=9) as zfile:
    zfile.mkdir(ROOT)
    for f in files:
        zfile.write(f, f'{ROOT}/{f}')

    zfile.write(OUT_SRC, f'{ROOT}/{OUT_SRC}')
    zfile.write('LICENSE', f'{ROOT}/LICENSE')
    zfile.write('version.txt', f'{ROOT}/version.txt')
    remove(VERSION_FILE)
    print(f'Created {OUT_ARCHIVE}')

