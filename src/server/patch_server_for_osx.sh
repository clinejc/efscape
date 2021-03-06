#!/bin/bash

PREFIX_DIR=/usr/local/lib
RHPC_VERSION=2.3.1

if [ $# -gt '0' ]; then
    echo "prefix="$1
    PREFIX_DIR=$1
fi

echo "directory prefix="$PREFIX_DIR

# run if <otool> command exists
if command -v otool &> /dev/null
then
	cmds=( "efserver" "efserver2" )
	libs=( "librepast_hpc-"$RHPC_VERSION".dylib" "librelogo-"$RHPC_VERSION".dylib")

	for i in "${cmds[@]}"
	do

		echo "patching command <$i>..."

		for j in "${libs[@]}"
		do
			echo "with library="$j
			install_name_tool -change "@rpath/"$j $PREFIX_DIR"/"$j "./.libs/"$i
		done
	done
fi

