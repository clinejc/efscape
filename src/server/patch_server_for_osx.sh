#!/bin/bash

PREFIX_DIR=/usr/local

if [ $# -gt '0' ]; then
    echo "prefix="$1
    PREFIX_DIR=$1
fi

echo "diretory prefix="$PREFIX_DIR

# run if <otool> command exists
if command -v otool &> /dev/null
then
	cmds=( "efserver" "efserver2" )
	libs=( "librepast_hpc-2.3.0.dylib" "librelogo-2.3.0.dylib")

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


# patch `efserver2`
#install_name_tool -change @rpath/librepast_hpc-2.3.0.dylib ./.libs/efserver2
#install_name_tool -change @rpath/librelogo-2.3.0.dylib ./.libs/efserver2
