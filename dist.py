import subprocess as sb
import multiprocessing as mp
from os import listdir, path, mkdir
from platform import system
from shutil import copy as file_copy, disk_usage

def main():
    sources = glob_extension('src', '.c', exclude=['main.c', 'base.c'])
    headers = glob_extension('src', '.h')

    data = ['/* Single compilation unit for whole library */']
    sources.sort()
    for src in sources:
        with open(src, 'r') as f:
            data.append(f'/* File: {src} */')
            data.append(f.read())
    data = '\n'.join(data) + '\n'

    DIR = 'dist'
    try:
        mkdir(DIR)
    except FileExistsError:
        pass

    with open(path.join(DIR, 'base.c'), 'w') as f:
        n = f.write(data)
        print(f'Wrote {n}B to base.c')

    for header in headers:
        file_copy(header, DIR)

def glob_extension(dirpath: str, ext: str, exclude = None):
    res = []
    if exclude is None: exclude = []

    for entry in listdir(dirpath):
        entrypath =path.join(dirpath, entry)
        if path.isfile(entrypath) and entry.endswith(ext) and entry not in exclude:
            res.append(entrypath)
    return res

if __name__ == '__main__': main()
