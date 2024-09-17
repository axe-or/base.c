#!/usr/bin/env sh

cc='gcc -std=c11'
cflags='-O2 -Wall -Wextra -fPIC -I. -fsanitize=address,undefined'
errflags='-Werror=vla'
ldflags=''

set -eu

Run(){ echo "$@"; $@; }

{ Run python bundle.py > /dev/null ; } &

Run $cc $errflags $cflags main.c $ldflags -o base.bin

Run ./base.bin


