#!/usr/bin/env sh

set -eu

outFile='base.zip'

files='
    arena_allocator.h
    assert.h
    bytes_buffer.h
    heap_allocator.h
    io.h
    memory.h
    prelude.h
    string.h
    testing.h
    utf8.h
'

GenerateBaseFile(){
	echo '/* Utility file if you want to compile the library without potentially polluting a file with static functions. */'
	echo "#define BASE_C_IMPLEMENTATION 1"
	for file in $(echo $files | sed 's/\s+//g'); do
		echo "#include \"$file\""
	done
	echo
}

CreateSourceArchive(){
	rm -f "$outFile"
	version="$(git rev-parse --short=16 HEAD)"
	mkdir -p "./base"
	for file in $(echo $files | sed 's/\s+//g'); do
		cp "$file" "./base/$file"
	done
	cp "LICENSE" "./base"
	GenerateBaseFile > "./base/base.c"
	sed "s/{{BaseCVersion}}/$version/g" "./base/prelude.h" -i

	zip -q -r -9 "$outFile" "./base"

	rm -rf "./base"
}

CreateSourceArchive
echo "Created $outFile"

# For pre-commit hook
git add "$outFile" || exit 0

