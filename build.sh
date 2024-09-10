#!/usr/bin/env sh

cc='gcc -std=c11'
cflags='-O2 -Wall -Wextra -fPIC -I.'
errflags='-Werror=vla'
ldflags=''

set -eu

Run(){ echo "$@"; $@; }

Run $cc $errflags $cflags main.c $ldflags -o base.bin

Run ./base.bin


