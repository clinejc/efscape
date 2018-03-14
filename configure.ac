#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)

dnl --------------------------------
dnl Specify INIT and CONFIG options
dnl --------------------------------
AC_INIT([efscape],[1.0.0])
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE
AM_INIT_AUTOMAKE([subdir-objects])
LT_INIT
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/efscape/efscape.ice])
#AC_CONFIG_HEADER([config.h])

# required for libraries built by this package
AC_PROG_RANLIB

dnl -----------------------------------------------
dnl Adds configure options
dnl -----------------------------------------------

# Checks for programs.

# Set lang c++
AC_LANG(C++)
AC_PROG_CC([mpicc mpixlc])
AC_PROG_CXX([mpic++ mpixlcxx])
CXXFLAGS="-std=c++11 -fPIC"

AC_LIBTOOL_DLOPEN
AM_PROG_LIBTOOL
AM_SANITY_CHECK


dnl -----------------------------------------------
dnl Checks for libraries
dnl -----------------------------------------------

PKG_CHECK_MODULES(DEPS,
	adevs >= 3.0
	jsoncpp >= 1.8.0
	liblog4cxx > 0.1
	zeroc-ice >= 3.5
	repast >= 2.0
	libmongocxx >= 3.1)


BOOST_REQUIRE([1.60])

# Checks for libraries.
AC_CHECK_LIB([dl], [dlopen])
AC_CHECK_LIB([curl], [curl_version])
AC_CHECK_LIB([mpi], [MPI_Send])

BOOST_SERIALIZATION()
BOOST_MPI()
BOOST_FILESYSTEM()
BOOST_SYSTEM()
BOOST_PROGRAM_OPTIONS()
BOOST_DATE_TIME()
BOOST_REGEX()

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_STRUCT_TM
AC_TYPE_UINT32_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_HEADER_STDC
AC_CHECK_FUNCS([floor rint sqrt])

AC_CONFIG_FILES([Makefile
		 pkgconfig/efscape.pc
		 src/Makefile
		 src/efscape/Makefile
		 src/efscape/utils/Makefile
		 src/efscape/impl/Makefile
		 src/efscape/server/Makefile
		 examples/Makefile
		 examples/gpt/Makefile
		 examples/zombie/Makefile])
AC_OUTPUT
