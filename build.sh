#!/usr/bin/env sh

cc='gcc -std=c11'
cflags='-O2 -Wall -Wextra -fPIC -I.'
ldflags=''

Run(){ echo "$@"; $@; }

Obj(){ echo "$1" | sed 's/\.c$/.o/'; }

mkdir -p ./bin

set -e
for f in *.c; do
	Run $cc $cflags -c "$f" -o "bin/$(Obj "$f")" &
done
wait

Run ar rcs bin/libc2.a bin/*.o
Run $cc bin/libc2.a $ldflags -o test.bin

./test.bin
