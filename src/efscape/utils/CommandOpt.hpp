// __COPYRIGHT_START__
// Package Name : efscape
// File Name : CommandOpt.hpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_UTILS_COMMANDOPT_HH
#define EFSCAPE_UTILS_COMMANDOPT_HH

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <ctype.h>
#include <assert.h>
#include <boost/program_options.hpp>

namespace efscape {

  namespace utils {

    /**
     * class CommandOpt
     *
     * @author Jon C. Cline <clinej@stanfordalumni.org>
     * @version 1.1.0 created:	11 November 1999, revised 10 Jul 2017
     *
     * This class encapsulates the details of processing the command line
     * options.\n\n
     *
     * From the reference below with minor modifications
     * Reference:
     *  - Section 7.8 in Lippman, Stanley and Josee Lajoie. 1999.
     *    C++ Primer - 3rd ed. Addison Wesley. Reading, MA. 
     */
    class CommandOpt {

    public:
      CommandOpt();
      virtual ~CommandOpt();

      virtual int parse_options( int argc, char *argv[] );
 
      std::string out_file() { return mC_out_file; }
      bool   debug_on() { return mb_debug_on; }
      int    files()    { return mC_file_names.size(); }
 
      std::string& operator[]( int ix ) {
	assert( ix < (int)mC_file_names.size() );
	return mC_file_names[ ix ];
      }

      virtual const char* program_name() = 0;
      virtual const char* program_version() = 0;

      virtual int execute() = 0;
 
    protected:

      virtual void usage( int exit_value = 0 );

      /** program option descriptions */
      boost::program_options::options_description mC_description;

      /** program option extended descriptions */
      boost::program_options::options_description mC_extended_description;
      
      /** program variables */
      boost::program_options::variables_map mC_variable_map;

      std::string mC_out_file;
      std::vector<std::string> mC_file_names;

    private:
 
      bool mb_debug_on;
 
      //     static const char *const _program_name;
      //     static const char *const _program_version;
    };

  } // namespace utils

} // namespace efscape

#endif	// #ifndef EFSCAPE_UTILS_COMMANDOPT_HH
