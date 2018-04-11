// __COPYRIGHT_START__
// Package Name : efscape
// File Name : CommandOpt.hpp
// Copyright (C) 2006-2018 Jon C. Cline
// 
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.
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
