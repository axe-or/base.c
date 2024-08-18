#!/usr/bin/env sh

cc='gcc -std=c11'
cflags='-O2 -Wall -Wextra -fPIC -I.'
ldflags=''

Run(){ echo "$@"; $@; }

Obj(){ echo "$1" | sed 's/\.c$/.o/'; }

Run $cc $cflags main.c $ldflags -o test.bin

./test.bin
