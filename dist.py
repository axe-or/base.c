from os import listdir, path, mkdir
from shutil import copy as file_copy
from zipfile import ZIP_DEFLATED, ZipFile

def remove_includes(data: str, rem_set: list[str]):
    for rm in rem_set:
        data = data.replace(f'#include "{rm}"', f'/* #include "{rm}" */')
        data = data.replace(f'#include "./{rm}"', f'/* #include "./{rm}" */')
    return data

def main():
    sources = glob_extension('src', '.c', exclude=['main.c', 'base.c'])
    headers = [
        # Foundations (Freestanding headers)
        'prelude.h',
        'context.h',
        'spinlock.h',
        # Important definitions
        'assert.h',
        'io.h',
        'memory.h',
        'list.h',
        'utf8.h',
        # General utilities and platform-agnostic implementations
        'string.h',
        'bytes_buffer.h',
        'arena_allocator.h',
        'fnv_hash.h',
        'command_line_opt.h',
        # Platform specific implementation
        'heap_allocator.h',
    ]

    DIR = 'base'
    try: mkdir(DIR)
    except FileExistsError: pass

    # Create .h amalagamation
    with open(path.join(DIR, 'base.h'), 'w') as amalgam:
        data = ['#pragma once']
        for header in headers:
            with open(path.join('src', header), 'r') as f:
                data.append(f'/* File: {header} */')
                code = remove_includes(f.read(), headers)
                data.append(code.replace('#pragma once', ''))
            data.append('')
        n = amalgam.write('\n'.join(data))
        print(f'Wrote {n}B to base.h')

    # Create .c amalagamation
    with open(path.join(DIR, 'base.c'), 'w') as amalgam:
        data = ['#include "./base.h"']
        sources.sort()
        for src in sources:
            with open(src, 'r') as f:
                data.append(f'/* File: {src} */')
                code = remove_includes(f.read(), headers)
                data.append(code)
        data.append('')

        n = amalgam.write('\n'.join(data))
        print(f'Wrote {n}B to base.c')

    file_copy('LICENSE', DIR)

    with ZipFile('base.zip', 'w', compression=ZIP_DEFLATED, compresslevel=9) as zf:
        zf.mkdir(DIR)
        for header in listdir(DIR):
            zf.write(path.join(DIR, header))
    print(f'Created base.zip ({path.getsize("base.zip") // 1024}KiB)')


def glob_extension(dirpath: str, ext: str, exclude = None):
    res = []
    if exclude is None: exclude = []

    for entry in listdir(dirpath):
        entrypath =path.join(dirpath, entry)
        if path.isfile(entrypath) and entry.endswith(ext) and entry not in exclude:
            res.append(entrypath)
    return res

if __name__ == '__main__': main()
