#!/usr/bin/env sh

cc='gcc -std=c11'
cflags='-O2 -Wall -Wextra -fPIC -I. -fsanitize=address'
errflags='-Werror=vla'
ldflags=''

set -eu

Run(){ echo "$@"; $@; }

Run python bundle.py
Run $cc $errflags $cflags main.c $ldflags -o base.bin

Run ./base.bin


