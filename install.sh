#!/bin/bash

set -eu

# Note: Currently installs adevs and Repast HPC
PREFIX_DIR=/usr/local

# install adevs
function install_adevs() {
  if [ -e $BUILD_DIR/adevs ]
  then
    echo " A directory named $BUILD_DIR/adevs already exists; you must delete it before it can be rebuilt."
    echo "Skipping..."
    return
  fi
  cd $BUILD_DIR
  git clone git@github.com:clinejc/adevs.git
  cd adevs
  git checkout serialization-and-autotools
  chmod +x autogen.sh
  ./autogen.sh --prefix=$PREFIX_DIR
  make install
}

# install repast hpc
function install_repast_hpc {
  if [ -e $BUILD_DIR/repast.hpc ]
  then
    echo " A directory named $BUILD_DIR/repas._hpc already exists; you must delete it before it can be rebuilt."
    echo "Skipping..."
    return
  fi
  cd $BUILD_DIR
  git clone git@github.com:clinejc/repast.hpc.git
  cd repast.hpc
  git checkout add-interactive-simulator
  mkdir Release
  cd Release
  cmake --DCMAKE=Release -DCMAKE_INSTALL_PREFIX=$PREFIX_DIR ../src
  make install
}

function install_efscape {
    cd $EFSCAPE_PATH
    ./autogen.sh --prefix=$PREFIX_DIR
    make

    # patch server
    cwd=$PWD
    cd $PWD/src/server
    ./patch_server_for_osx.sh $PREFIX_DIR/lib
    cd $cwd

    # resume installation
    make install
}

function install_server {
    cd $EFSCAPE_PATH/src/server
    cp -p -R certs config.client config.props config.server log4j.properties $EFSCAPE_HOME
    if [ ! -e $EFSCAPE_HOME/sessions ]; then
      mkdir $EFSCAPE_HOME/sessions
    fi
}

function install_all {
    install_adevs
    install_repast_hpc
    install_efscape
    install_server
}

function help_info() {
    echo "Command Help"

    echo "./install.sh [OPTION [prefix_dir]]"
    echo "[adevs]      install adevs."
    echo "[repast_hpc] install repast_hpc."
    echo "[efscape]    install efscape."
    echo "[server]     install efscape server."
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
    'server')
	install_server
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

