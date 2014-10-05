// __COPYRIGHT_START__
// Package Name : efscape
// File Name : RunSim.hh
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_UTILS_RUNSIM_HH
#define EFSCAPE_UTILS_RUNSIM_HH

#include <efscape/utils/CommandOpt.hh>
#include <boost/scoped_ptr.hpp>

namespace efscape {

  namespace impl {

    /**
     * Implements a simple interactive model builder for the efscape modeling
     * framework. It provides a command-line interface derived from the
     * CommandOpt class.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.02 created 01 Feb 2008, revised 14 Jul 2014
     *
     * ChangeLog:
     *   - 2014-07-14 Implementation pared down to bare essentials.
     *   - 2008-02-01 Created class RunSim.
     */
    class RunSim : public efscape::utils::CommandOpt
    {
    public:

      RunSim();
      virtual ~RunSim();

      int parse_options( int argc, char *argv[]);
      int execute();

      const char* program_name();
      const char* program_version();

      static const char* ProgramName();

    protected:

      void usage( int exit_value = 0 );

    private:

      /** program name */
      static const char* mScp_program_name;

      /** program version */
      static const char* mScp_program_version;

    };
  }

}

#endif	// #ifndef EFSCAPE_UTILS_RUNSIM_HH
