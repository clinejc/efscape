// __COPYRIGHT_START__
// Package Name : efscape
// File Name : BuildModel.hh
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_BUILDMODEL_HH
#define EFSCAPE_IMPL_BUILDMODEL_HH

#include <efscape/utils/CommandOpt.hh>	// parent class definition

#include <efscape/impl/adevs_config.hh>
#include <efscape/impl/ModelHomeI.hh>
#include <efscape/impl/AdevsModel.hh>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/scoped_ptr.hpp>
#include <functional>
#include <sstream>

namespace efscape {

  namespace impl {

    // forward declarations
    class BuildCmd;
    typedef boost::shared_ptr<BuildCmd> BuildCmdPtr;

    /**
     * Implements a simple interactive model builder for the efscape modeling
     * framework. It provides a command-line interface derived from the
     * CommandOpt class.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.04 created 01 Feb 2008, revised 31 Aug 2014
     *
     * ChangeLog:
     *   - 2014-08-31 Implementation pared down to bare essentials.
     *   = 2009.03.04 Modifications due to revisions to adevs and ModelI API.
     *   - 2008.04.14 Removed data member mCp_digraph (no longer needed).
     *   - 2008.02.01 Created class BuildModel.
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

      /** handle to root model */
      AdevsModel* mCp_RootModel;

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



    /**
     * Provides a base class for model builder commands.
     *
     * @version 0.01 created 06 Feb 2008, revised Feb 2008
     */
    class BuildCmd : std::unary_function<int,void>
    {

    public:

      BuildCmd() {}
      virtual ~BuildCmd() {}

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

  } // namespace impl

} // namespace efscape

#endif	// #ifndef EFSCAPE_IMPL_BUILDMODEL_HH
