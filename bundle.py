#!/usr/bin/env python

from os import path
import re
import subprocess as sb
from zipfile import ZipFile
from math import ceil
import zipfile

# The order here is very crucial, includes must be in a DAG
deps = [
    'prelude.h',
    'assert.h', 'utf8.h', 'list.h', 'fnv_hash.h', 'spinlock.h',
    'memory.h', 'io.h',
    'string.h', 'bytes_buffer.h', 'arena_allocator.h',
    'heap_allocator.h', 'file.h', 'net.h',
]

src_out = []

def normalize_whitespace(s: str) -> str:
    ws_pattern = re.compile(r'[ \t\r]+')
    s = ws_pattern.sub(' ', s)
    return s.strip()

for dep in deps:
    lines = []
    with open(dep, 'r') as f:
        lines = f.readlines()

    out_lines = []
    for line in lines:
        normalized = normalize_whitespace(line)
        if not normalized.startswith('#include "') and not normalized.startswith('#pragma once'):
            out_lines.append(line.rstrip())

    src_out.append('\n'.join(out_lines))
    print(f'Added {dep}')

src_out.insert(0, '#pragma once')

src_out = '\n'.join(src_out).replace('\n\n\n', '\n\n') + '\n'

git_version = sb.check_output(['git', 'rev-parse', 'HEAD']).decode('utf-8').strip()
src_out = src_out.replace('{{BaseCVersion}}', git_version)

with open('base.h', 'w') as f:
    n = f.write(src_out)
    print(f'Wrote {ceil(n / 1024)}KiB to base.h ({git_version})')

with open('base.c', 'w') as f:
    f.write('#define BASE_C_IMPLEMENTATION 1\n'+
            '#include "base.h"\n')
    print(f'Created base.c')

sb.call(["git", "add", "base.h"])

with ZipFile('base.zip', 'w', compresslevel=9) as zf:
    zf.write('base.h', path.join('base', 'base.h'))
    zf.write('base.h', path.join('base', 'base.c'))
    zf.write('base.h', path.join('base', 'LICENSE'))
    print('Created source release: base.zip') 

