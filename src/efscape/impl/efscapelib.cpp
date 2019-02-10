// __COPYRIGHT_START__
// Package Name : efscape
// File Name : efscapelib.cpp
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
#include <efscape/impl/efscapelib.hpp>

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>

#include <boost/algorithm/string.hpp>

#include <fstream>
#include <sstream>

// definitions for accessing and managing local file system resources
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/filesystem/operations.hpp>
#include <boost/date_time/posix_time/posix_time.hpp> // date_time definitions
using namespace boost::gregorian;

namespace fs = boost::filesystem;

namespace efscape {

  namespace impl {

    char const gcp_libname[] = "libefscape-impl";
    char const gcp_liburl[] = "https://github.com/clinejc/efscape.git";

    /**
     * Helper function for injecting a bag events into a model.
     *
     * @param e time
     * @param xb bag of events
     * @param aCp_model handle to model
     */
    void inject_events(double e, const adevs::Bag<IO_Type>& xb,
		       DEVS* aCp_model)
    {
      if (aCp_model == nullptr) {
	return;
      }
      
      ATOMIC* lCp_atomic = NULL;
      if ( (lCp_atomic = aCp_model->typeIsAtomic() ) != NULL ) {
	lCp_atomic->delta_ext(e, xb);
      }
      else {
	NETWORK* lCp_network = aCp_model->typeIsNetwork();
	adevs::Set<DEVS*> components;
	lCp_network->getComponents(components);
	/*typename*/ adevs::Set<DEVS*>::iterator iter =
	  components.begin();
	for (; iter != components.end(); iter++) {
	  inject_events(e, xb, *iter);
	}
      }
    }

    /**
     * Helper function for extracting a bag events from a model.
     *
     * @param yb bag of events
     * @param aCp_model handle to model
     */
    void get_output(adevs::Bag<IO_Type>& xb,
		    DEVS* aCp_model)
    {
      if (aCp_model == NULL) {
	std::cout << "model is null!\n";
	return;
      }
      ATOMIC* lCp_atomic = NULL;
      if ( (lCp_atomic = aCp_model->typeIsAtomic() ) != NULL ) {
	lCp_atomic->output_func(xb);
      }
      else {
	NETWORK* lCp_network = NULL;
	if ( (lCp_network = aCp_model->typeIsNetwork()) == NULL)
	  return;
	adevs::Set<DEVS*> components;
	lCp_network->getComponents(components);
	/*typename*/ adevs::Set<DEVS*>::iterator iter =
	  components.begin();
	for (; iter != components.end(); iter++) {
	  get_output(xb, *iter);
	}
      }
    }

    DEVS* createModel( const char* acp_classname ) {
      return ( Singleton<ModelHomeI>::Instance().getModelFactory().createObject(acp_classname) );
    }

    std::string cplusTypeName2Posix( std::string aC_cplusTypeName ) {
      // 1. first replace ':'s from namespace separates with '_'s
      std::string lC_posixString =
	boost::replace_all_copy(aC_cplusTypeName,
				":",
				"_");   
      // 2. Next replace leading '<' for template class names with '.'
      lC_posixString =
	boost::replace_all_copy(lC_posixString,
				"<",
				".");
      // 3. Remove trailing '>' from template class names
      lC_posixString =
	boost::replace_all_copy(lC_posixString,
				">",
				"");
      return lC_posixString;
    }
    
    DEVSPtr cloneModelUsingJSON( const DEVSPtr& aCp_model ) {

      DEVSPtr lCp_clone;
      try {
	// serialize the model out to a buffer and then back in as a clone
	std::ostringstream lC_buffer_out;
	saveAdevsToJSON(aCp_model, lC_buffer_out);
	std::istringstream lC_buffer_in(lC_buffer_out.str().c_str());
	lCp_clone = loadAdevsFromJSON(lC_buffer_in);
      } catch(...) {
	LOG4CXX_ERROR(ModelHomeI::getLogger(),
		      "Exception encountered during attempt to clone adevs model");
      }

      return lCp_clone;
    }

    void runSim( DEVS* aCp_model, double ad_timeMax ) {
      adevs::Simulator<IO_Type> lC_sim( aCp_model );

      // simulate model until infinity
      while (lC_sim.nextEventTime() < ad_timeMax) {
	lC_sim.execNextEvent();
      }
    }

    adevs::Simulator<IO_Type>*
    createSimSession(DEVS* aCp_model, Json::Value aC_info) {
      //------------------------------------------------------------------------
      // create new directory for this simulation session
      // $EFSCAPE_PATH/sessions/[today's date]/[modelName]-uuid
      //------------------------------------------------------------------------
      std::ostringstream lC_buffer;
      
      // Write todays date to a string
      date today = day_clock::local_day();
      date_facet* f = new date_facet("%Y-%m-%d");
      locale loc = locale(locale("en_US"), f);
      // following lines only needed if reusing the lC_buffer
      // lC_buffer.clear();
      // lC_buffer.str("");
      // lC_buffer.seekp(0);
      lC_buffer.imbue(loc);
      lC_buffer << today;

      // get model name
      std::string lC_modelName = "model";
      if ( aC_info.isMember("modelName") ) {
	lC_modelName = aC_info["modelName"].asString();
      } else if ( aC_info.isMember("typeName") ) {
	lC_modelName =
	  cplusTypeName2Posix( aC_info["typeName"].asString() );
      }

      // generate unique id for session
      boost::uuids::uuid uuid = boost::uuids::random_generator()();

      //------------------------------------------------------------------------
      // create session path: apparently requires two steps to work reliably
      // 1. create $EFSCAPE_PATH/sessions/[today's date]
      // 2. append [modelName]-uuid
      //------------------------------------------------------------------------
      fs::path lC_sessionPath =
	fs::path(efscape::impl::ModelHomeI::getHomeDir()) /
	fs::path("sessions") /
	fs::path(lC_buffer.str());

      // first attempt to create the session path if needed
      if (!fs::exists(lC_sessionPath)) {
      	try {
      	  fs::create_directory(lC_sessionPath);
      	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
      			"Created session directory <"
      			<< lC_sessionPath.string()
      			<< ">");
      	}
      	catch(exception& e) {
      	  LOG4CXX_ERROR(ModelHomeI::getLogger(),
      			"Attempt to create directory <"
      		  	<< lC_sessionPath.string()
      			<< ">: "
      			<< e.what());
      	}
      }

      // Now append model name
      lC_sessionPath =
      	lC_sessionPath /
      	fs::path(lC_modelName + "-" + boost::uuids::to_string(uuid));

      // attempt to create the session path if needed
      if (!fs::exists(lC_sessionPath)) {
      	try {
      	  fs::create_directory(lC_sessionPath);
      	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
      			"Created session directory <"
      			<< lC_sessionPath.string()
      			<< ">");
      	}
      	catch(exception& e) {
      	  LOG4CXX_ERROR(ModelHomeI::getLogger(),
      			"Attempt to create directory <"
      		  	<< lC_sessionPath.string()
      			<< ">: "
      			<< e.what());
      	}
      }

      if ( chdir(lC_sessionPath.string().c_str()) ) {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Changed directory to <"
		      << lC_sessionPath.string()
		      << ">");
      } else {
	LOG4CXX_ERROR(ModelHomeI::getLogger(),
		      "Attempt to change directory to <"
		      <<lC_sessionPath.string()
		      << "> failed!");
      }

      // write metadata/parameter file to session directory
      std::string lC_fileName = lC_modelName + ".json";
      std::ofstream ofs(lC_fileName.c_str());
      ofs << aC_info << std::endl;

      //-----------------------------------------------------------
      // initialize the model by injecting -a message to the "setup"
      //----------------------------------------------------------
      adevs::Bag<efscape::impl::IO_Type> xb;
      efscape::impl::IO_Type x("setup_in",
			       "");
      xb.insert(x);
      inject_events(0., xb, aCp_model);
      
      // return handle to new simulator with model
      return ( new adevs::Simulator<IO_Type>(aCp_model) );
    }

    adevs::Devs<IO_Type>*
    getRootModel(adevs::Devs<IO_Type>* aCp_model)
    {
      adevs::Devs<IO_Type>* lCp_rootmodel = aCp_model;

      while (lCp_rootmodel->getParent() != 0)
	lCp_rootmodel = (adevs::Devs<IO_Type>*)lCp_rootmodel->getParent();

      return lCp_rootmodel;

    } // getRooModel(adevs::Devs<IO_Type>*)

    const adevs::Devs<IO_Type>*
    getRootModel(const adevs::Devs<IO_Type>* aCp_model)
    {
      const adevs::Devs<IO_Type>* lCp_rootmodel = aCp_model;

      while (lCp_rootmodel->getParent() != 0)
	lCp_rootmodel = (const adevs::Devs<IO_Type>*)lCp_rootmodel->getParent();

      return lCp_rootmodel;

    } // getRooModel(const adevs::Devs<IO_Type>*)


    /**
     * constructor
     *
     * @param aC_InputPort input port
     * @param aCp_CellSpace handle to cell space
     */
    CellBroadcastConsumer::CellBroadcastConsumer(PortType aC_InputPort,
						 efscape::impl::CELLSPACE*
						 aCp_CellSpace) :
      mC_InputPort(aC_InputPort),
      mCp_CellSpace(aCp_CellSpace)
    {
    }

    /**
     * Consumes input and generates a collection of input events
     *
     * @param aCr_input external input
     * @param aCr_InputBuffer bag of cell events
     */
    bool
    CellBroadcastConsumer::operator()(const adevs::Bag<IO_Type>& aCr_input,
				      adevs::Bag< adevs::Event<CellEvent> >&
				      aCCr_InputBuffer)
    {
      if (mCp_CellSpace == 0)
	return false;

      adevs::Bag<IO_Type>::iterator iter;
      for (iter = aCr_input.begin(); iter != aCr_input.end();
	   iter++) {

	if ((*iter).port != mC_InputPort)
	  return false;

	for (long iZ = 0; iZ < mCp_CellSpace->getDepth(); iZ++)
	  for (long iY = 0; iY < mCp_CellSpace->getHeight(); iY++)
	    for (long iX = 0; iX < mCp_CellSpace->getWidth(); iX++) {
	      adevs::Event< CellEvent > lC_event;
	      lC_event.model = mCp_CellSpace;
	      lC_event.value.x = iX;
	      lC_event.value.y = iY;
	      lC_event.value.z = iZ;
	      lC_event.value.value = (*iter).value;
	      aCCr_InputBuffer.insert(lC_event);
	    }
      }

      return true;
    }

    /**
     * constructor
     *
     * @param aC_InputPort output port
     * @param aCp_model handle to model
     */
    CellInputConsumer::CellInputConsumer(PortType aC_InputPort,
					 adevs::Devs<IO_Type>*
					 aCp_model) :
      mC_InputPort(aC_InputPort),
      mCp_model(aCp_model)
    {}

    /**
     * Consumes input and generates a collection of input events
     *
     * @param aCr_input external input
     * @param aCr_InputBuffer bag of events
     */
    bool
    CellInputConsumer::operator()(const adevs::Bag<CellEvent>& aCr_input,
				  adevs::Bag< adevs::Event<IO_Type> >&
				  aCCr_InputBuffer)
    {
      adevs::Bag<CellEvent>::iterator iter;
      for (iter = aCr_input.begin(); iter != aCr_input.end();
	   iter++) {

	// default: route external input to first port (0)
	aCCr_InputBuffer.insert(adevs::Event<IO_Type>(mCp_model,
						      IO_Type(mC_InputPort,
							      *iter)));
      }

      return true;
    }

  } // namespace impl

} // namespace efscape
