// __COPYRIGHT_START__
// __COPYRIGHT_END__
#ifndef SIMPLESIM_CREATESIMPLESIM_HH
#define SIMPLESIM_CREATESIMPLESIM_HH

#include "efscape/impl/BuildModel.hpp"	// parent class definition

namespace simplesim {

  /**
   * Implements a command-line program that create a simplesim model.
   *
   * @author Jon Cline <clinej@stanfordalumni.org>
   * @version 0.01 created 08 Feb 2008, revised 08 Feb 2008
   *
   * ChangeLog:
   *   - 2008.02.08 Created class CreateSimpleSim.
   */
  class CreateSimpleSim : public efscape::impl::BuildModel
  {
  public:

    CreateSimpleSim();
    ~CreateSimpleSim();

    int parse_options( int argc, char *argv[]);
    int execute();

    const char* program_name();
    const char* program_version();

    static const char* ProgramName();

  protected:

    void usage( int exit_value = 0 );

    void setClock();
    void createModel()
      throw(std::logic_error);

  private:

    /** program name */
    static const char* mScp_program_name;

    /** program version */
    static const char* mScp_program_version;

  };

} // namespace simplesim

#endif	// #ifndef SIMPLESIM_CREATESIMPLESIM_HH
