// __COPYRIGHT_START__
// Package Name : efscape
// File Name : CommandOpt.cpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
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
	std::cout << "--input-file,-i found: input file(s)\n";
	mC_file_names =
	  mC_variable_map["input-file"].as< std::vector<std::string> >();
      }

      // set output file
      if (mC_variable_map.count("output-file")) {
	std::cout << "-output-file,-o found: output file\n";
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

