
# efscape

This project implements a C++-based modeling and simulation (M&S) framework that
utilizes [ZeroC Ice](https://zeroc.com/) RPC middleware to deploy a M&S client-server application.

The efscape package uses [adevs](https://web.ornl.gov/~nutarojj/adevs/), C++-based implementation of the DEVS (Discrete EVent System Specification) modeling methdology (Zeigler 1974) as the core simulation engine, and [Repast HPC](https://repast.github.io/repast_hpc.html), an HPC agent-based modeling library. The adevs library has been patched to support serialization of model states in XML ([boost serialization](https://www.boost.org/)) and JSON ([cereal](https://uscilab.github.io/cereal/)) format. The Repast HPC library has been patched to support interative execution of simulations -- an adevs model wrapper is provided to embed Repast HPC models in an adevs simulation.

This project is derived from earlier efforts by this author to design and deply an object-oriented framework for implementing spatially-explicit ecological models. It implements a limited set of features from the following projects:
 * SimApp (Cline 1999-2004), a C++ CORBA-based simulation framework,
 * ecological framework or "ef" (Saponara 1997), a C++ framework for discrete-
   time, discrete-space, individual-based ecological simulations, and
 * simdriver (Cline 2001-2003),a generic SimApp-based [ATLSS](http://atlss/org) driver.

## Prerequisites

### general
+ a C++ compiler for your platform (c++-11 compliant)
+ [automake](https://www.gnu.org/software/automake/)  (>=1.16.1)
+ [autoconf](https://www.gnu.org/software/autoconf) (>=2.69)
+ [Boost C++ libraries](https://www.boost.org/) (>=1.60)
+ [adevs (A Discrete EVent System simulator)](https://web.ornl.gov/~nutarojj/adevs/) (>= 3.2) -- a C++ library for constructing discrete event simulations
    * Added patch to add XML and JSON serialization
+ [Repast HPC](https://repast.github.io/repast_hpc.html) (>= 2.2) --  an agent-based modeling
    * Added patch to support interactive execution of simulations
and simulation (ABMS) toolkit for high performance distributed computing platforms
+ [ZeroC Ice (Interconnect Connection Engine)](https://zeroc.com/) (>=3.7) -- Comprehensive RPC framework
+ [cereal](https://uscilab.github.io/cereal/) () -- C++11 library for serialization

## Installation
First patch and install adevs:
```
$ cd adevs-3.3
$ patch -p1 -i ../efscape/ext/adevs-3.3.patch
$ chmod +x autogen.sh
$ ./autogen.sh
$ ./configure --prefix=[PREFIX]
$ make install
```

Next, patch and install repast_hpc
```
$ cd ../repast_hpc-2.2.0
$ patch -p1 -i ../efscape/ext/repast_hpc-2.2.0.patch
$ cp -p pkgconfig/repast.pc [PREFIX]/lib/pkgconfig
```

The current version of efscape uses autoconf, so the standard autoconf install
instructions apply. For initial set-up of the configure, run

```
$ ./autogen.sh
$ ./configure --prefix=[PREFIX]
$ make install
```

### Key environment variables:
+ EFSCAPE_PATH: path of the source
+ EFSCAPE_REPOSITORY: libdir path for all efscape model libraries (default location: PREFIX/lib/efscape)
+ EFSCAPE_HOME: efscape server home
    * Suggested location: ~/www/ice/efscape (note: create alias or copy from SOURCEDIR/src/efscape/server)

### Running the M&S server

To run the efscape M&S server (with the debug option):

```
$ efserver -d
```

A simple c++ client has been included. Other client applications, including a JavaScript/AngularJS/node efscape client application, have been implemented.