// __COPYRIGHT_START__
// Package Name : efscape
// File Name : TestCode.hh
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_GIS_TESTCODE_HH
#define EFSCAPE_GIS_TESTCODE_HH

#include <functional>
#include <vector>
#include <string>
#include <sstream>

#include <stdlib.h>

namespace gistest {

  /**
   * Provides a base class for test functions
   */
  class TestCode : public std::unary_function<int,void>
  {

  public:

    TestCode() {}
    virtual ~TestCode() {}

    virtual int operator()() {
      return EXIT_SUCCESS;
    }
    virtual const char* usage() const { return ""; }
    virtual int init(const std::vector<std::string>& aC1_args) = 0;
    const char* name() const { return mC_name.c_str(); }
    const char* info() const { return mC_info.c_str(); }
    const char* errorMsg() const { return mC_ErrorMsg.str().c_str(); }

  protected:

    void name(const char* acp_name) { mC_name = acp_name; }
    void info(const char* acp_info) { mC_info = acp_info; }
    std::ostringstream mC_ErrorMsg;

  private:

    std::string mC_name;
    std::string mC_info;

  };

  /**
   * Tests opening an existing GDAL dataset
   */
  class OpenGDAL : public TestCode {

  public:

    OpenGDAL();

    const char* usage() const {
      return "takes a single argument <filename>";
    }
    int operator()();
    int init(const std::vector<std::string>& aC1r_args);

  private:
    std::string mC_filename;
  };

  /**
   * Test opening an new GDAL dataset
   */
  class CreateGDAL : public TestCode {
  public:
    CreateGDAL();

    const char* usage() const {
      return "takes two arguments: <filename> <file format>";
    }
    int operator()();
    int init(const std::vector<std::string>& aC1r_args);

  private:

    std::string mC_filename;
    std::string mC_drivername;
  };


  /**
   * Test opening an new NetCDF dataset
   */
  class CreateGeoNetCDF : public TestCode {
  public:
    CreateGeoNetCDF();

    const char* usage() const {
      return "takes a single argument <filename>";
    }
    int operator()();
    int init(const std::vector<std::string>& aC1r_args);

  private:
    std::string mC_filename;
  };
}

#endif	// #ifndef EFSCAPE_GIS_TESTCODE_HH
