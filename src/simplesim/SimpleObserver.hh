// __COPYRIGHT_START__
// __COPYRIGHT_END__
#ifndef SIMPLESIM_SIMPLEOBSERVER_HH
#define SIMPLESIM_SIMPLEOBSERVER_HH

#include <boost/serialization/version.hpp>

// parent class definitions
#include <efscape/impl/adevs_config.hpp>
#include <efscape/impl/EntityI.hpp>
#include <efscape/impl/InitObject.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

namespace simplesim {

  class SimpleState;

  /**
   * Defines a simple observer class that records output from a SimpleGenerator
   * object in a property tree that is then translated into a JSON array of
   * objects representing a time series.
   *
   * @author Jon C. Cline <clinej@stanfordalumni.org>
   * @version 0.2.0 created 11 Oct 2006, revised 30 Apr 2017
   *
   * ChangeLog:
   *   - 2017-04-30 removed InitObject implementation
   *   - 2014-11-01 added property tree for generating JSON
   *   - 2014-10-02 updated to support major revision of efscape
   *   - 2010.05.05 fixed bug (parent class InitObject undeclared)
   *   - 2009.12.06 parent class changed from ModelI (removed) to EntityI
   *   - 2009.03.04 Extensive modifications due to API changes
   *     - changes to serialization interface
   *     - changes to implementation of adevs model hierarchy
   *   - 2008.01.04 Extensive modifications due to API changes
   *     - base class efscape::impl::ObserverBase remove (deprecated)
   *     - dataset implemented with efscape::impl::DataFrameI
   *   - 2007.10.09 changed port type as per changes to base model class
   *   - 2007.01.23 implemented changes to SimpleGenerator interface
   *   - 2007.01.12 implemented changes to ModelType_impl interface
   *   - 2007.01.06 changed ModelType to ModelType_impl<>
   *   - 2006.10.24 added private static data member and accessor methods
   *     - efscape::impl::ModelType
   *   - 2006.10.17 created class 'SimpleObserver'
   */
  class SimpleObserver : virtual public efscape::impl::EntityI,
			 public adevs::Atomic<efscape::impl::IO_Type>
  {
    friend class boost::serialization::access;

  public:

    SimpleObserver();
    ~SimpleObserver();

    // Observer input port
    void input(const SimpleState* aCp_value) {}

    /// Internal transition function.
    void delta_int();

    /// External transition function.
    void delta_ext(double e, const adevs::Bag<efscape::impl::IO_Type>& xb);

    /// Confluent transition function.
    void delta_conf(const adevs::Bag<efscape::impl::IO_Type>& xb);
 
    /// Output function.  
    void output_func(adevs::Bag<efscape::impl::IO_Type>& yb);

    /// Time advance function.
    double ta();

    /// Output value garbage collection.
    void gc_output(adevs::Bag<efscape::impl::IO_Type>& g);

  private:

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
      ar & boost::serialization::make_nvp("EntityI",
					  boost::serialization::base_object<efscape::impl::EntityI >(*this) );
      ar & boost::serialization::make_nvp("Atomic",
					  boost::serialization::base_object<efscape::impl::ATOMIC >(*this) );
    }

    /** handle to property tree */
    boost::scoped_ptr<boost::property_tree::ptree> mCp_ptree;

    /** flag whether the observer is active */
    bool mb_is_active;

  };				// class SimpleObserver definition

  // class SimpleObserver reflection
  struct SimpleObserver_strings {
    static const char name[];
    static const char m_input[];
    static const char f_output[];
  };

} // namespace simplesim

BOOST_CLASS_VERSION(simplesim::SimpleObserver, 10)

#endif	// #ifndef SIMPLESIM_SIMPLEOBSERVER_HH
