#!/bin/bash

set -eu

# Note: Currently installs adevs and Repast HPC
PREFIX_DIR=/usr/local

# install adevs
function install_adevs() {
  if [ -e $BUILD_DIR/adevs-3.3 ]
  then
    echo " A directory named $BUILD_DIR/adevs-3.3 already exists; you must delete it before it can be rebuilt."
    echo "Skipping..."
    return
  fi
  cd $BUILD_DIR
  if [ ! -e adevs-3.3.zip ]
  then
    wget https://web.ornl.gov/~nutarojj/adevs/adevs-3.3.zip
  fi
  unzip adevs-3.3.zip
  cd adevs-3.3
  patch -p1 -i $EFSCAPE_PATH/ext/adevs-3.3.patch
  chmod +x autogen.sh
  ./autogen.sh
  ./configure --prefix=$PREFIX_DIR
  make install
}

# install repast hpc
function install_repast_hpc {
  if [ -e $BUILD_DIR/repast_hpc-2.2.0 ]
  then
    echo " A directory named $BUILD_DIR/repast_hpc-2.2.0 already exists; you must delete it before it can be rebuilt."
    echo "Skipping..."
    return
  fi
  cd $BUILD_DIR
  if [ ! -e $BUILD_DIR/repast_hpc-2.2.0.tgz ]; then
      wget https://github.com/Repast/repast.hpc/releases/download/v2.2.0/repast_hpc-2.2.0.tgz
  fi
  tar xvzf repast_hpc-2.2.0.tgz
  cd repast_hpc-2.2.0
  patch -p1 -i $EFSCAPE_PATH/ext/repast_hpc-2.2.0.patch
  mkdir Release
  cd Release
  cmake -DCMAKE=Release -DCMAKE_INSTALL_PREFIX=$PREFIX_DIR ../src
  make install
  cd ..
  ./configure --prefix=$PREFIX_DIR
  make install
}

function install_efscape {
    cd $EFSCAPE_PATH
    ./autogen.sh
    ./configure --prefix=$PREFIX_DIR
    make install
}

function install_all {
    install_adevs
    install_repast_hpc
    install_efscape
}

function help_info() {
    echo "Command Help"

    echo "./install.sh [OPTION [prefix_dir]]"
    echo "[adevs]      install adevs."
    echo "[repast_hpc] install repast_hpc."
    echo "[efscape]    install efscape."
    echo "[all]        install all packages including efscape."
    echo "[help]       show this help info."
}

if [ $# -lt '1' ]; then
    help_info
    exit -1
fi

if [ $# -gt '1' ]; then
    echo "prefix="$2
    PREFIX_DIR=$2
fi

BUILD_DIR=$PWD/ext/build
if [ ! -e $BUILD_DIR ]; then
  mkdir $BUILD_DIR
fi

case $1 in
    'adevs')
	install_adevs
	;;
    'repast_hpc')
	install_repast_hpc
	;;
    'efscape')
	install_efscape
	;;
    'all')
	install_all
	;;
    'help')
        help_info
	;;
    *)
	echo "Option [" $1 "] not recognized."
	help_info
	;;
esac

