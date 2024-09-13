import re

deps = [
    'prelude.h',
    'assert.h', 'utf8.h',
    'memory.h', 'io.h',
    'string.h', 'arena_allocator.h', 'heap_allocator.h', 'bytes_buffer.h', 'file.h',
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
        if (not normalized.startswith('#include "') and
            not normalized.startswith('#pragma once') and
            not normalized.startswith('///')):

            out_lines.append(line.rstrip())

    src_out.append('\n'.join(out_lines))

src_out.insert(0, '#pragma once')

src_out = '\n'.join(src_out).replace('\n\n\n', '\n\n')

with open('base.h', 'w') as f:
    f.write(src_out)

