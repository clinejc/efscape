// __COPYRIGHT_START__
// Package Name : efscape
// File Name : CreateGpt.hpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_UTILS_CREATEGPT_HPP
#define EFSCAPE_UTILS_CREATEGPT_HPP

#include <efscape/utils/CommandOpt.hpp>

namespace gpt {

  /**
   */
  class CreateGpt : public efscape::utils::CommandOpt
  {
  public:
    
    /** default constructor */
    CreateGpt();

    /**
     * Parses the command line arguments and initializes the command
     * configuration.
     *
     * @param argc number of command line arguments
     * @param argv vector of command line arguments
     * @returns exit status
     */
    int parse_options( int argc, char *argv[]);

    /**
     * Executes the command
     *
     * @returns exit state
     */
    int execute();

    /**
     * Returns the program name
     *
     * @returns the program name
     */
    const char* program_name();

    /**
     * Returns the program version.
     *
     * @returns the program version
     */
    const char* program_version();

    /**
     * Returns the program name (class version)
     *
     * @returns the program name
     */
    static const char* ProgramName();

  protected:

    /**
     * Prints out usage message for this command/program
     *
     * @args  exit_value exit value
     */
    void usage( int exit_value = 0 );

  private:

    /** program name */
    static const char* mScp_program_name;

    /** program version */
    static const char* mScp_program_version;
    
  };
  
} // namespace gpt

#endif	// 
