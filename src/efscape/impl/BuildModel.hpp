// __COPYRIGHT_START__
// Package Name : efscape
// File Name : BuildModel.hh
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_BUILDMODEL_HH
#define EFSCAPE_IMPL_BUILDMODEL_HH

#include <efscape/utils/CommandOpt.hpp>	// parent class definition

#include <efscape/impl/adevs_config.hh>
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ClockI.hh>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/scoped_ptr.hpp>
#include <functional>
#include <sstream>

namespace efscape {

  namespace impl {

   /**
     * Implements a simple interactive model builder for the efscape modeling
     * framework. It provides a command-line interface derived from the
     * CommandOpt class.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.9.1 created 01 Feb 2008, revised 28 Apr 2017
     */
    class BuildModel : public efscape::utils::CommandOpt
    {
    public:

      BuildModel();
      virtual ~BuildModel();

      int parse_options( int argc, char *argv[]);
      int execute();

      const char* program_name();
      const char* program_version();

      static const char* ProgramName();

    protected:

      void usage( int exit_value = 0 );

      virtual void setClock();
      virtual void createModel()
	throw(std::logic_error);
      virtual void saveConfig()
	throw(std::logic_error);

      /** simulation clock (implementation) */
      ClockIPtr mCp_ClockI;

      /** handle to model */
      boost::scoped_ptr<DEVS> mCp_model;

      /** model class name */
      std::string mC_ClassName;

      /** model name */
      std::string mC_Name;

      /** file system path */
      std::string mC_ResourcePath;

      /** simulation time step*/
      double md_TimeDelta;

      /** maximum simulation time */
      double md_TimeMax;

      /** start date */
      boost::posix_time::ptime mC_StartDate;

      /** time units */
      boost::posix_time::time_duration mC_TimeUnits;

    private:

      /** program name */
      static const char* mScp_program_name;

      /** program version */
      static const char* mScp_program_version;

    };
  } // namespace impl

} // namespace efscape

#endif	// #ifndef EFSCAPE_IMPL_BUILDMODEL_HH
