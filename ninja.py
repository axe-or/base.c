from os import listdir, path

RULES = {
    'compile': '$cc -o $out $cflags -c $in',
    'link': '$linker -o $out $ldflags $in',
}

VALUES = {
    'cc': 'musl-gcc',
    'linker': 'musl-gcc',
    'cflags': '-I. -O2 -Wall -Wextra -fPIC',
    'ldflags': '-static',
    'ignoreflags': '-Wno-unused-label',
}

def generate():
    headers = glob_extension('.', '.h', exclude=['base.h'])
    sources = glob_extension('.', '.c', exclude=['main.c'])
    objects = list(map(lambda s: path.join('bin', s.removesuffix('.c') + '.o'), sources))
    executable = 'base.bin'

    # Ninja source
    VALUES['cflags'] += ' ' + VALUES['ignoreflags']
    lines = []

    for var, value in VALUES.items():
        lines.append(f'{var} = {value}')
    lines.append('')
    for rule, command in RULES.items():
        lines.append(f'rule {rule}')
        lines.append(f'  command = {command}')
    lines.append('')
    for i, source in enumerate(sources):
        lines.append(build(objects[i], 'compile', source, headers))
    lines.append(build(executable, 'link', 'main.c ' + ' '.join(objects), headers))

    with open('build.ninja', 'w') as f:
        lines.insert(0, '# Auto generated file by ninja.py')
        n = f.write('\n'.join(lines) + '\n\n')
        print(f'Wrote {n}B to build.ninja')


def build(output: str, rule: str, args: str, deps = None):
    cmd = f'build {output}: {rule} {args}'
    if deps is not None:
        cmd += f' | {" ".join(deps)}'
    return cmd

def glob_extension(dirpath: str, ext: str, exclude = None):
    res = []
    if exclude is None: exclude = []

    for entry in listdir(dirpath):
        if path.isfile(entry) and entry.endswith(ext) and entry not in exclude:
            res.append(entry)
    return res

if __name__ == '__main__': generate()
