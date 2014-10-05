// __COPYRIGHT_START__
// Package Name : efscape
// File Name : RunServer.hpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_SERVER_RUNSERVER_HPP
#define EFSCAPE_SERVER_RUNSERVER_HPP

#include <efscape/utils/CommandOpt.hh>
#include <boost/scoped_ptr.hpp>
#include <Ice/Application.h>

namespace efscape {

  namespace server {

    /**
     * Implements an ICE (Internet Connection Engine) server simple interactive
     * model for the efscape modeling framework. It provides a command-line
     * interface derived from the CommandOpt class.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.0.1 created 23 Sep 2014, revised 23 Sep 2014
     *
     * ChangeLog:
     *   - 2014-09-23 Created class RunServer.
     */
    class RunServer : public efscape::utils::CommandOpt,
		      public Ice::Application
    {
    public:

      RunServer();
      virtual ~RunServer();

      int parse_options( int argc, char *argv[]);
      int execute();

      const char* program_name();
      const char* program_version();

      static const char* ProgramName();

      // Implementation ICE application methods
      int run(int, char*[]);

    protected:

      void usage( int exit_value = 0 );

    private:

      /** program name */
      static const char* mScp_program_name;

      /** program version */
      static const char* mScp_program_version;

    };

  } // namespace server

} // namespace efscape

#endif	// #ifndef EFSCAPE_SERVER_RUNSERVER_HPP
