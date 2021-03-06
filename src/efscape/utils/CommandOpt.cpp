// __COPYRIGHT_START__
// Package Name : efscape
// File Name : CommandOpt.cpp
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
#include <CommandOpt.hpp>
 
using std::cerr;
using std::cout;
using std::endl;
using std::exit;
using std::vector;
using std::string;
// const char *const CommandOpt::_program_name = "comline";
// const char *const CommandOpt::_program_version = "version 0.01 (08/07/97)";

namespace po = boost::program_options;

namespace efscape {

  namespace utils {

    /** constructor */
    CommandOpt::CommandOpt() :
      mC_description("allowed options"),
      mb_debug_on( false )
    {}

    /** destructor */
    CommandOpt::~CommandOpt() {
      if (debug_on())
	cout << "Deleting command...\n";
    }

    /**
     * Parses the command line arguements and initializes the command
     * configuration.
     *
     * @param argc number of command line arguments
     * @param argv vector of command line arguments
     * @returns exit status
     */
    int
    CommandOpt::
    parse_options( int argc, char* argv[] ) {

      // add basic options
      mC_description.add_options()
	("version,v", "prints version information and exits")
	("help,h", "produce help message")
	("debug,d", "turns debugging on")
	("output-file,o", po::value<std::string>(),
	 "file within which to write out results")
	("input-file,i", po::value< std::vector<std::string> >(), "input files")
	;

      // add extended options
      mC_description.add(mC_extended_description);

      po::positional_options_description p;
      p.add("input-file", -1);

      // generate variable map
      po::store(po::command_line_parser(argc, argv).
		options(mC_description).positional(p).run(),
		mC_variable_map);
      po::notify(mC_variable_map);

      // help option
      if (mC_variable_map.count("help")) {
	std::cout << "-h found: help information\n";
	usage();
      }

      // check version option
      if (mC_variable_map.count("version")) {
	std::cout << program_name()
		  << " ::  "
		  << program_version()
		  << endl;
 
	return 1;
      }

      // set debug flag
      if (mC_variable_map.count("debug")) {
	std::cout << "--debug,-d found: debugging turned on\n";
	mb_debug_on = true;
      }

      // set input files
      if (mC_variable_map.count("input-file")) {
	mC_file_names =
	  mC_variable_map["input-file"].as< std::vector<std::string> >();
      }

      // set output file
      if (mC_variable_map.count("output-file")) {
	mC_out_file = mC_variable_map["output-file"].as<std::string>();
      }
 
      return 0;
    }

    void
    CommandOpt::
    usage( int exit_value )
    {
      std::cerr << "usage:\n"
		<< program_name() << mC_description << std::endl;
 
      exit( exit_value );
    }

  } // namespace utils

} // namespace efscape

