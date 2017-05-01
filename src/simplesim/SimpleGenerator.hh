// __COPYRIGHT_START__
// __COPYRIGHT_END__
#ifndef SIMPLESIM_SIMPLEGENERATOR_HH
#define SIMPLESIM_SIMPLEGENERATOR_HH

// boost serialization definitions
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/scoped_ptr.hpp>

// parent class definitions
#include "efscape/impl/adevs_config.hpp"
#include "adevs/adevs_serialization.hpp"
#include "efscape/impl/EntityI.hpp"
#include "efscape/impl/InitObject.hpp"

// class data members
#include "efscape/impl/ClockI.hpp"
#include <boost/scoped_ptr.hpp>

namespace simplesim {

  /**
   * Defines a very simple model state class.
   */
  class SimpleState
  {
      friend class boost::serialization::access;
  public:
    /** constructor */
    SimpleState() :
      mCp_clock(0)
    {
    }

    /** destructor */
    ~SimpleState() {}

    /** @returns const reference count */
    int count() const { return mi_count; }

    /** @returns reference to count */
    int& count() { return mi_count; }

    /** @returns const reference to clock */
    const efscape::impl::ClockI& clock() const { return *mCp_clock; }

    /** @returns const handle to clock */
    efscape::impl::ClockI& clock() { return *mCp_clock; }

    /** @param aCp_clock handle to clock */
    void setClock(efscape::impl::ClockI* aCp_clock) {
      mCp_clock = aCp_clock;
    }

    /** @return handle to clock */
    efscape::impl::ClockI* getClockPtr() { return mCp_clock; }

  private:

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
      ar  & boost::serialization::make_nvp("clock",mCp_clock);
      ar  & boost::serialization::make_nvp("counter",mi_count);
    }

    /** handle to clock */
    efscape::impl::ClockI* mCp_clock;

    /** count */
    int mi_count;

  };				// class SimpleState

  /**
   * Defines a simple atomic model that drives the BasicModel simulation
   * example.
   *
   * @author Jon C. Cline <clinej@stanfordalumni.org>
   * @version 0.1.0 created 11 Oct 2006, revised 02 Oct 2014
   *
   * ChangeLog:
   *   - 2014-10-02 updated to support major revision of efscape
   *   - 2009-12-06 parent class changed from ModelI (removed) to EntityI
   *   - 2009-03-04 Extensive modifications due to API changes
   *     - changes to serialization interface
   *     - changes to implementation of adevs model hierarchy
   *   - 2007-10-09 changed port type as per changes to base model class
   *   - 2007-02-06 changes propagating from efscape::data interface
   *   - 2007-01-23 added support for efscape::data interface
   *   - 2007-01-12 implemented changes to ModelType_impl interface
   *   - 2007-01-06 added version support
   *   - 2006-10.17 added boost serialization
   *   - 2006-10-11 create class 'SimpleGenerator'
   */
  class SimpleGenerator : virtual public efscape::impl::EntityI,
			  public adevs::Atomic<efscape::impl::IO_Type>,
			  public efscape::impl::InitObject
  {
      friend class boost::serialization::access;

  public:

    SimpleGenerator();
    ~SimpleGenerator();

    void initialize()
      throw(std::logic_error);

    // Model output port
    const SimpleState* out;

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

    /** @returns model state */
    const SimpleState* getState() const { return mCp_state.get(); }

  private:

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
      ar & boost::serialization::make_nvp("EntityI",
					  boost::serialization::base_object<efscape::impl::EntityI >(*this) );
      ar & boost::serialization::make_nvp("Atomic",
					  boost::serialization::base_object<efscape::impl::ATOMIC >(*this) );

      ar & boost::serialization::make_nvp("SimpleState",mCp_state);
      ar & boost::serialization::make_nvp("status",mi_status);
    }

    /** smart handle to clock */
    efscape::impl::ClockIPtr mCp_clock;

    /** handle to random variable */
    boost::scoped_ptr<SimpleState> mCp_state;

    /** simple attribute */
    int mi_status;

    /** active flag */
    bool mb_IsActive;

  };

  // class SimpleGenerator reflection
  struct SimpleGenerator_strings {
    static const char name[];
    static const char f_out[];
  };

} // namespace simplesim

BOOST_CLASS_VERSION(simplesim::SimpleGenerator, 9)

#endif	// #ifndef SIMPLESIM_SIMPLEGENERATOR_HH
