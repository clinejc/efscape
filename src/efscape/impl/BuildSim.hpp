// __COPYRIGHT_START__
// Package Name : efscape
// File Name : BuildSim.hpp
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
#ifndef EFSCAPE_IMPL_BUILDSIM_HPP
#define EFSCAPE_IMPL_BUILDSIM_HPP

#include <efscape/utils/CommandOpt.hpp>

namespace efscape {
  namespace impl {

    /**
     */
    class BuildSim : public efscape::utils::CommandOpt
    {
    public:
    
      /** default constructor */
      BuildSim();

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

      /** model type name */
      std::string mC_modelTypeName;

      /** model name */
      std::string mC_modelName;

    private:

      /** program name */
      static const char* mScp_program_name;

      /** program version */
      static const char* mScp_program_version;
    
    };

  } // namespace impl
} // namespace efscape

#endif	// #ifndef EFSCAPE_IMPL_BUILDSIM_HPP
