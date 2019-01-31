// __COPYRIGHT_START__
// __COPYRIGHT_END__
#ifndef EFSCAPE_ZOMBIE_TEST_HPP
#define EFSCAPE_ZOMBIE_TEST_HPP

#include <efscape/utils/CommandOpt.hpp>

namespace zombie {

  /**
   * Test program for Repast HPC Relogo Zombie model example
   */
  class ZombieTest : public efscape::utils::CommandOpt
  {
  public:
    /** default constructor */
    ZombieTest();

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
}

#endif
